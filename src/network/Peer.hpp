#ifndef __PEER_HPP__
#define __PEER_HPP__

#include <boost/asio.hpp>
#include <cstdint>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "logger.hpp"

using tcp = boost::asio::ip::tcp;
using error_code = boost::system::error_code;
using boost::asio::io_context;

class Peer : public std::enable_shared_from_this<Peer>
{
  public:
    using message_handler = std::function<void(const std::string&)>;

    static std::shared_ptr<Peer> create(boost::asio::io_context& io_context,
                                        uint16_t                 listen_port);

    void start();
    void connect(const std::string& host, const std::string& port);
    void write(const std::string& message);
    void set_message_handler(message_handler handler);

  private:
    Peer(boost::asio::io_context& io_context, uint16_t listen_port);

    void do_accept();
    void do_read();

    boost::asio::io_context& io_context_;
    tcp::acceptor            acceptor_;
    tcp::socket              socket_;
    std::vector<char>        data_;
    message_handler          on_message_;
    static constexpr size_t  initial_buffer_size = 1024;
};

#endif // __PEER_HPP__
