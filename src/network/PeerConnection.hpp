#ifndef __PEER_CONNECTION_HPP__
#define __PEER_CONNECTION_HPP__

#include <functional>
#include <queue>
#include <string>

#include <boost/asio.hpp>

class PeerConnection : public std::enable_shared_from_this<PeerConnection>
{
  public:
    using tcp = boost::asio::ip::tcp;
    using message_handler =
        std::function<void(const std::string&, const std::string&)>;

    PeerConnection(boost::asio::io_context& io_context, const std::string& id);

    void         start();
    void         send(const std::string& message);
    tcp::socket& socket();
    std::string  id() const;
    void         setMessageHandler(message_handler handler);

  private:
    void readHeader();
    void readBody();
    void writeMessage();

    tcp::socket     socket_;
    std::string     id_;
    message_handler messageHandler_;

    enum { header_length = 4 };
    char                    inbound_header_[header_length];
    std::vector<char>       inbound_data_;
    std::queue<std::string> outbound_data_;
};

#endif // __PEER_CONNECTION_HPP__
