#include "NetworkManager.hpp"

NetworkManager::NetworkManager(int port) :
    acceptor_(io_context_,
              boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
{}

void NetworkManager::start()
{
    startAccept();
    io_context_.run();
}

void NetworkManager::stop()
{
    io_context_.stop();
}

void NetworkManager::connectToPeer(const std::string& address, int port)
{
    auto socket = std::make_shared<boost::asio::ip::tcp::socket>(io_context_);
    socket->async_connect(
        boost::asio::ip::tcp::endpoint(
            boost::asio::ip::address::from_string(address), port),
        [this, address, port, socket](const boost::system::error_code& error) {
            handleConnect(address + ":" + std::to_string(port), error);
        });
}

void NetworkManager::sendMessage(const std::string& peerId,
                                 const std::string& message)
{
    auto it = peers_.find(peerId);
    if (it != peers_.end())
    {
        it->second->send(message);
    }
}

void NetworkManager::setMessageHandler(
    std::function<void(const std::string&, const std::string&)> handler)
{
    messageHandler_ = std::move(handler);
}

void NetworkManager::startAccept()
{
    auto newConnection = std::make_shared<PeerConnection>(io_context_, "");
    acceptor_.async_accept(
        newConnection->socket(),
        [this, newConnection](const boost::system::error_code& error) {
            handleAccept(newConnection, error);
        });
}

void NetworkManager::handleAccept(std::shared_ptr<PeerConnection> newConnection,
                                  const boost::system::error_code& error)
{
    if (!error)
    {
        std::string peerId =
            newConnection->socket().remote_endpoint().address().to_string() +
            ":" +
            std::to_string(newConnection->socket().remote_endpoint().port());
        newConnection->setMessageHandler(
            [this](const std::string& message, const std::string& peerId) {
                handleMessage(message, peerId);
            });
        peers_[peerId] = newConnection;
        newConnection->start();
    }
    startAccept();
}

void NetworkManager::handleConnect(const std::string&               peerId,
                                   const boost::system::error_code& error)
{
    if (!error)
    {
        auto newConnection =
            std::make_shared<PeerConnection>(io_context_, peerId);
        newConnection->setMessageHandler(
            [this](const std::string& message, const std::string& peerId) {
                handleMessage(message, peerId);
            });
        peers_[peerId] = newConnection;
        newConnection->start();
    } else {
        std::cerr << "Connection failed: " << error.message() << std::endl;
    }
}

void NetworkManager::handleMessage(const std::string& message,
                                   const std::string& peerId)
{
    if (messageHandler_)
    {
        messageHandler_(message, peerId);
    }
}
