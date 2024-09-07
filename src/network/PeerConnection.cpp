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
    // TODO: serialize the message
    // serialization logic
    if (message.getType() != MessageType::TEXT)
    {
        LOG_ERROR << "Only TEXT messages are supported at the moment";
        return;
    }

    const TextMessage& text_message = static_cast<const TextMessage&>(message);
    std::string        text_to_send = text_message.getText();

    std::vector<char> data_to_send(text_to_send.begin(), text_to_send.end());

    LOG_INFO << "Sending message: " << text_to_send;

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
    socket_.async_read_some(
        boost::asio::buffer(read_buffer_),
        [this, self](const error_code& error, std::size_t bytes_transferred) {
            LOG_INFO << "Message received";
            handleRead(error, bytes_transferred);
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
                                std::size_t       bytes_transferred)
{
    if (!error)
    {
        // TODO: deserialize the message
        std::string received_data(read_buffer_.begin(),
                                  read_buffer_.begin() + bytes_transferred);

        LOG_INFO << "Received raw data: " << received_data
                 << ", bytes: " << bytes_transferred;

        TextMessage text_message(received_data);
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
