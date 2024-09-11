#include "PeerConnection.hpp"

std::shared_ptr<PeerConnection> PeerConnection::create(io_context& io_context)
{
    return std::shared_ptr<PeerConnection>(new PeerConnection(io_context));
}

PeerConnection::PeerConnection(io_context& io_context) :
    socket_(io_context), is_writing_(false), message_length_(0)
{
    read_buffer_.resize(1024);
}

void PeerConnection::start()
{
    doRead();
}

void PeerConnection::stop()
{
    error_code ec;
    socket_.close(ec);

    if (ec)
    {
        LOG_ERROR << "Error closing socket: " << ec.message();
    }
}

void PeerConnection::sendMessage(const Message& message)
{
    std::vector<uint8_t> serialized_data;
    MessageType          type = message.getType();

    switch (type)
    {
        case MessageType::TEXT:
        {
            const TextMessage& text_message =
                static_cast<const TextMessage&>(message);
            serialized_data = text_message.serialize();
            break;
        }
        case MessageType::FILE_METADATA:
        {
            const FileMetadata& file_metadata =
                static_cast<const FileMetadata&>(message);
            serialized_data = file_metadata.serialize();
            break;
        }
        case MessageType::CHUNK:
        {
            const ChunkMessage& chunk_message =
                static_cast<const ChunkMessage&>(message);
            serialized_data = chunk_message.serialize();
            break;
        }
        default: LOG_ERROR << "Unknown message type"; return;
    }

    uint32_t             length = static_cast<uint32_t>(serialized_data.size());
    std::vector<uint8_t> data_to_send(sizeof(MessageType) + sizeof(length) +
                                      serialized_data.size());

    std::memcpy(data_to_send.data(), &type, sizeof(MessageType));
    std::memcpy(data_to_send.data() + sizeof(MessageType), &length,
                sizeof(length));
    std::memcpy(data_to_send.data() + sizeof(MessageType) + sizeof(length),
                serialized_data.data(), serialized_data.size());

    LOG_INFO << "Sending message of type: " << static_cast<int>(type)
             << ", size: " << serialized_data.size();

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

boost::asio::ip::tcp::socket& PeerConnection::socket()
{
    return socket_;
}

void PeerConnection::doRead()
{
    auto self(shared_from_this());
    boost::asio::async_read(
        socket_,
        boost::asio::buffer(&current_message_type_, sizeof(MessageType)),
        [this, self](const error_code& error,
                     std::size_t /*bytes_transferred*/) {
            if (!error)
            {
                boost::asio::async_read(
                    socket_,
                    boost::asio::buffer(&message_length_,
                                        sizeof(message_length_)),
                    [this, self](const error_code& error,
                                 std::size_t /*bytes_transferred*/) {
                        if (!error)
                        {
                            read_buffer_.resize(message_length_);
                            boost::asio::async_read(
                                socket_, boost::asio::buffer(read_buffer_),
                                [this, self](const error_code& error,
                                             std::size_t bytes_transferred) {
                                    handleRead(error, bytes_transferred);
                                });
                        } else {
                            LOG_ERROR << "Read error (message length): "
                                      << error.message();
                            stop();
                        }
                    });
            } else {
                LOG_ERROR << "Read error (message type): " << error.message();
                stop();
            }
        });
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

void PeerConnection::handleRead(const error_code& error,
                                size_t            bytes_transferred)
{
    // TODO: separate functions
    if (!error)
    {
        LOG_INFO << "Received message of type: "
                 << static_cast<int>(current_message_type_)
                 << ", size: " << bytes_transferred;

        if (message_handler_)
        {
            switch (current_message_type_)
            {
                case MessageType::TEXT:
                {
                    TextMessage text_message =
                        TextMessage::deserialize(read_buffer_);
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
                default:
                    LOG_ERROR << "Unknown message type received: "
                              << static_cast<int>(current_message_type_);
                    break;
            }
        }

        doRead();
    } else {
        LOG_ERROR << "Read error: " << error.message();
        stop();
    }
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
