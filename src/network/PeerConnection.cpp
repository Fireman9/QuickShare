#include "PeerConnection.hpp"

PeerConnection::PeerConnection(boost::asio::io_context& io_context,
                               const std::string&       id) :
    socket_(io_context),
    id_(id)
{}

void PeerConnection::start()
{
    readHeader();
}

void PeerConnection::send(const std::string& message)
{
    bool write_in_progress = !outbound_data_.empty();
    outbound_data_.push(message);
    if (!write_in_progress)
    {
        writeMessage();
    }
}

boost::asio::ip::tcp::socket& PeerConnection::socket()
{
    return socket_;
}

std::string PeerConnection::id() const
{
    return id_;
}

void PeerConnection::setMessageHandler(message_handler handler)
{
    messageHandler_ = std::move(handler);
}

void PeerConnection::readHeader()
{
    auto self(shared_from_this());
    boost::asio::async_read(
        socket_, boost::asio::buffer(inbound_header_, header_length),
        [this, self](boost::system::error_code ec, std::size_t) {
            if (!ec)
            {
                std::size_t body_length =
                    std::stoul(std::string(inbound_header_, header_length));
                inbound_data_.resize(body_length);
                readBody();
            }
        });
}

void PeerConnection::readBody()
{
    auto self(shared_from_this());
    boost::asio::async_read(
        socket_, boost::asio::buffer(inbound_data_),
        [this, self](boost::system::error_code ec, std::size_t) {
            if (!ec)
            {
                std::string message(inbound_data_.begin(), inbound_data_.end());
                if (messageHandler_)
                {
                    messageHandler_(message, id_);
                }
                readHeader();
            }
        });
}

void PeerConnection::writeMessage()
{
    auto        self(shared_from_this());
    std::string message = outbound_data_.front();
    std::string header = std::to_string(message.length());
    header.resize(header_length);

    std::vector<boost::asio::const_buffer> buffers;
    buffers.push_back(boost::asio::buffer(header));
    buffers.push_back(boost::asio::buffer(message));

    boost::asio::async_write(
        socket_, buffers,
        [this, self](boost::system::error_code ec, std::size_t) {
            if (!ec)
            {
                outbound_data_.pop();
                if (!outbound_data_.empty())
                {
                    writeMessage();
                }
            }
        });
}
