#include "Peer.hpp"

std::shared_ptr<Peer> Peer::create(boost::asio::io_context& io_context,
                                   uint16_t                 listen_port)
{
    return std::shared_ptr<Peer>(new Peer(io_context, listen_port));
}

Peer::Peer(boost::asio::io_context& io_context, uint16_t listen_port) :
    io_context_(io_context),
    acceptor_(io_context, tcp::endpoint(tcp::v4(), listen_port)),
    socket_(io_context), data_(initial_buffer_size)
{}

void Peer::start()
{
    do_accept();
}

void Peer::connect(const std::string& host, const std::string& port)
{
    tcp::resolver resolver(io_context_);
    auto          endpoints = resolver.resolve(host, port);

    boost::asio::async_connect(
        socket_, endpoints,
        [this, self = shared_from_this()](error_code ec, tcp::endpoint) {
            if (!ec)
            {
                LOG_INFO << "Connected to peer.";
                do_read();
            } else {
                LOG_ERROR << "Connection error: " << ec.message();
            }
        });
}

void Peer::write(const std::string& message)
{
    boost::asio::async_write(socket_, boost::asio::buffer(message),
                             [this, self = shared_from_this()](
                                 error_code ec, std::size_t /*length*/) {
                                 if (!ec)
                                 {
                                     LOG_INFO << "Message sent.";
                                 } else {
                                     LOG_ERROR << "Write error: "
                                               << ec.message();
                                 }
                             });
}

void Peer::do_accept()
{
    acceptor_.async_accept([this, self = shared_from_this()](
                               error_code ec, tcp::socket new_socket) {
        if (!ec)
        {
            LOG_INFO << "Accepted connection.";
            socket_ = std::move(new_socket);
            do_read();
        }
        do_accept();
    });
}

void Peer::do_read()
{
    socket_.async_read_some(
        boost::asio::buffer(data_),
        [this, self = shared_from_this()](boost::system::error_code ec,
                                          std::size_t               length) {
            if (!ec)
            {
                std::string received_message(data_.begin(),
                                             data_.begin() + length);
                LOG_INFO << "Received: " << received_message;

                if (on_message_)
                {
                    on_message_(received_message);
                }

                do_read();
            } else {
                LOG_ERROR << "Read error: " << ec.message();
            }
        });
}

void Peer::set_message_handler(message_handler handler)
{
    on_message_ = std::move(handler);
}
