#include "PeerConnection.hpp"

std::shared_ptr<PeerConnection> PeerConnection::create(io_context& io_context)
{
    return std::shared_ptr<PeerConnection>(new PeerConnection(io_context));
}

PeerConnection::PeerConnection(io_context& io_context) :
    socket_(io_context), is_writing_(false)
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
    // TODO: MessageType file
    if (message.getType() != MessageType::TEXT)
    {
        LOG_ERROR << "Only TEXT messages are supported at the moment";
        return;
    }

    const TextMessage& text_message = static_cast<const TextMessage&>(message);
    std::string        serialized = text_message.serialize();

    uint32_t          length = static_cast<uint32_t>(serialized.size());
    std::vector<char> data_to_send(sizeof(length) + serialized.size());
    std::memcpy(data_to_send.data(), &length, sizeof(length));
    std::memcpy(data_to_send.data() + sizeof(length), serialized.data(),
                serialized.size());

    LOG_INFO << "Sending message: " << text_message.getText();

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
        socket_, boost::asio::buffer(&message_length_, sizeof(message_length_)),
        [this, self](const error_code& error,
                     std::size_t /*bytes_transferred*/) {
            if (!error)
            {
                read_buffer_.resize(message_length_);
                boost::asio::async_read(
                    socket_, boost::asio::buffer(read_buffer_),
                    [this, self](const error_code& error,
                                 std::size_t /*bytes_transferred*/) {
                        handleRead(error);
                    });
            } else {
                LOG_ERROR << "Read error: " << error.message();
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

void PeerConnection::handleRead(const error_code& error)
{
    if (!error)
    {
        std::string received_data(read_buffer_.begin(), read_buffer_.end());
        LOG_INFO << "Received raw data, bytes: " << received_data.size();

        TextMessage text_message = TextMessage::deserialize(received_data);
        if (message_handler_)
        {
            message_handler_(text_message);
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
