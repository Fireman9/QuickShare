#ifndef __NETWORK_MANAGER_HPP__
#define __NETWORK_MANAGER_HPP__

#include <functional>
#include <iostream>
#include <memory>
#include <unordered_map>

#include <boost/asio.hpp>

#include "PeerConnection.hpp"

class NetworkManager
{
  public:
    NetworkManager(int port);

    void start();
    void stop();
    void connectToPeer(const std::string& address, int port);
    void sendMessage(const std::string& peerId, const std::string& message);
    void setMessageHandler(
        std::function<void(const std::string&, const std::string&)> handler);

  private:
    void startAccept();
    void handleAccept(std::shared_ptr<PeerConnection>  newConnection,
                      const boost::system::error_code& error);
    void handleConnect(const std::string&               peerId,
                       const boost::system::error_code& error);
    void handleMessage(const std::string& message, const std::string& peerId);

    boost::asio::io_context        io_context_;
    boost::asio::ip::tcp::acceptor acceptor_;
    std::unordered_map<std::string, std::shared_ptr<PeerConnection>> peers_;
    std::function<void(const std::string&, const std::string&)> messageHandler_;
};

#endif // __NETWORK_MANAGER_HPP__
