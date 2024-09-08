#ifndef PEER_CONNECTION_HPP
#define PEER_CONNECTION_HPP

#include <boost/asio.hpp>
#include <functional>
#include <memory>
#include <queue>

#include "Message/Message.hpp"
#include "Message/TextMessage.hpp"
#include "logger.hpp"

class PeerConnection : public std::enable_shared_from_this<PeerConnection>
{
  public:
    using tcp = boost::asio::ip::tcp;
    using error_code = boost::system::error_code;
    using io_context = boost::asio::io_context;
    using MessageHandler = std::function<void(const Message&)>;

    static std::shared_ptr<PeerConnection> create(io_context& io_context);

    void start();
    void stop();

    void sendMessage(const Message& message);
    void setMessageHandler(MessageHandler handler);

    tcp::socket& socket();

  private:
    explicit PeerConnection(io_context& io_context);

    void doRead();
    void doWrite();
    void handleRead(const error_code& error);
    void handleWrite(const error_code& error);

    tcp::socket                   socket_;
    std::vector<char>             read_buffer_;
    std::queue<std::vector<char>> write_queue_;
    MessageHandler                message_handler_;
    bool                          is_writing_;
    uint32_t                      message_length_;
};

#endif // PEER_CONNECTION_HPP
