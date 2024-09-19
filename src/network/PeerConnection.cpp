#include "PeerConnection.hpp"

std::shared_ptr<PeerConnection> PeerConnection::create(io_context& io_context)
{
    return std::shared_ptr<PeerConnection>(new PeerConnection(io_context));
}

PeerConnection::PeerConnection(io_context& io_context) :
    socket_(io_context), is_writing_(false), message_length_(0),
    is_connected_(false)
{
    read_buffer_.resize(1024);
}

void PeerConnection::start()
{
    is_connected_ = true;
    applyNetworkSettings();
    doRead();
}

void PeerConnection::stop()
{
    is_connected_ = false;
    error_code ec;
    socket_.close(ec);

    if (ec)
    {
        LOG_ERROR << "Error closing socket: " << ec.message();
    }
}

void PeerConnection::sendMessage(const Message& message)
{
    std::vector<uint8_t> data_to_send;

    switch (message.getType())
    {
        case MessageType::TEXT:
            data_to_send =
                serializeMessage(static_cast<const TextMessage&>(message));
            break;
        case MessageType::FILE_METADATA:
            data_to_send =
                serializeMessage(static_cast<const FileMetadata&>(message));
            break;
        case MessageType::CHUNK:
        {
            data_to_send =
                serializeMessage(static_cast<const ChunkMessage&>(message));
            network_settings_.updateBufferSizes(data_to_send.size());
            applyNetworkSettings();
            break;
        }
        case MessageType::CHUNK_METRICS:
            data_to_send =
                serializeMessage(static_cast<const ChunkMetrics&>(message));
            break;
        default: LOG_ERROR << "Unknown message type"; return;
    }

    LOG_INFO << "Sending message of type: "
             << static_cast<int>(message.getType())
             << ", size: " << data_to_send.size();

    bool write_in_progress = !write_queue_.empty();
    write_queue_.push(std::move(data_to_send));
    if (!write_in_progress)
    {
        doWrite();
    }
}

void PeerConnection::setMessageHandler(MessageHandler handler)
{
    message_handler_ = std::move(handler);
}

void PeerConnection::setNetworkSettings(const NetworkSettings& settings)
{
    network_settings_ = settings;
    if (is_connected_)
    {
        applyNetworkSettings();
    }
}

boost::asio::ip::tcp::socket& PeerConnection::socket()
{
    return socket_;
}

void PeerConnection::doRead()
{
    readMessageType();
}

void PeerConnection::readMessageType()
{
    auto self(shared_from_this());
    boost::asio::async_read(
        socket_, boost::asio::buffer(&current_message_type_, MESSAGE_TYPE_SIZE),
        [this, self](const error_code& error, std::size_t bytes_transferred) {
            if (!error)
            {
                readMessageLength();
            } else {
                handleRead(error, bytes_transferred);
            }
        });
}

void PeerConnection::readMessageLength()
{
    auto self(shared_from_this());
    boost::asio::async_read(
        socket_, boost::asio::buffer(&message_length_, MESSAGE_LENGTH_SIZE),
        [this, self](const error_code& error, std::size_t bytes_transferred) {
            if (!error)
            {
                readMessageBody();
            } else {
                handleRead(error, bytes_transferred);
            }
        });
}

void PeerConnection::readMessageBody()
{
    read_buffer_.resize(message_length_);
    auto self(shared_from_this());
    boost::asio::async_read(
        socket_, boost::asio::buffer(read_buffer_),
        [this, self](const error_code& error, std::size_t bytes_transferred) {
            handleRead(error, bytes_transferred);
        });
}

void PeerConnection::handleRead(const error_code& error,
                                size_t            bytes_transferred)
{
    if (!error)
    {
        LOG_INFO << "Received message of type: "
                 << static_cast<int>(current_message_type_)
                 << ", size: " << bytes_transferred;

        if (current_message_type_ == MessageType::CHUNK)
        {
            network_settings_.updateBufferSizes(bytes_transferred);
            applyNetworkSettings();
        }

        if (message_handler_)
        {
            processReceivedMessage();
        }
        doRead();
    } else {
        LOG_ERROR << "Read error: " << error.message();
        stop();
    }
}

void PeerConnection::processReceivedMessage()
{
    switch (current_message_type_)
    {
        case MessageType::TEXT:
        {
            TextMessage text_message = TextMessage::deserialize(read_buffer_);
            message_handler_(text_message);
            break;
        }
        case MessageType::FILE_METADATA:
        {
            FileMetadata file_metadata =
                FileMetadata::deserialize(read_buffer_);
            message_handler_(file_metadata);
            break;
        }
        case MessageType::CHUNK:
        {
            ChunkMessage chunk_message =
                ChunkMessage::deserialize(read_buffer_);
            message_handler_(chunk_message);
            break;
        }
        case MessageType::CHUNK_METRICS:
        {
            ChunkMetrics chunk_metrics =
                ChunkMetrics::deserialize(read_buffer_);
            message_handler_(chunk_metrics);
            break;
        }
        default:
            LOG_ERROR << "Unknown message type received: "
                      << static_cast<int>(current_message_type_);
            break;
    }
}

void PeerConnection::doWrite()
{
    if (write_queue_.empty())
    {
        is_writing_ = false;
        return;
    }

    is_writing_ = true;
    auto self(shared_from_this());
    boost::asio::async_write(socket_, boost::asio::buffer(write_queue_.front()),
                             [this, self](const error_code& error,
                                          std::size_t /*bytes_transferred*/) {
                                 handleWrite(error);
                             });
}

void PeerConnection::handleWrite(const error_code& error)
{
    if (!error)
    {
        write_queue_.pop();
        doWrite();
    } else {
        LOG_ERROR << "Write error: " << error.message();
        stop();
    }
}

void PeerConnection::applyNetworkSettings()
{
    if (!is_connected_)
    {
        LOG_ERROR << "Failed to apply network settings, not connected";
        return;
    }

    try
    {
        network_settings_.applyToSocket(socket_);
    } catch (const boost::system::system_error& e)
    {
        LOG_ERROR << "Failed to apply network settings: " << e.what();
    }
}
