#include "NetworkManager.hpp"

std::shared_ptr<NetworkManager> NetworkManager::create()
{
    return std::shared_ptr<NetworkManager>(new NetworkManager());
}

NetworkManager::NetworkManager() :
    work_(std::make_shared<io_context::work>(io_context_))
{}

void NetworkManager::start(uint16_t port)
{
    try
    {
        acceptor_ = std::make_unique<tcp::acceptor>(
            io_context_, tcp::endpoint(tcp::v4(), port));
        doAccept();

        LOG_INFO << "NetworkManager started on port " << port;

        std::thread([this]() { io_context_.run(); }).detach();
    } catch (const std::exception& e)
    {
        LOG_ERROR << "Error starting NetworkManager: " << e.what();
    }
}

void NetworkManager::stop()
{
    work_.reset();
    io_context_.stop();

    for (auto& peer : peers_)
    {
        peer.second->stop();
    }
    peers_.clear();

    if (acceptor_ && acceptor_->is_open())
    {
        acceptor_->close();
    }

    LOG_INFO << "NetworkManager stopped";
}

void NetworkManager::connectToPeer(const std::string& address, uint16_t port)
{
    auto new_connection = PeerConnection::create(io_context_);

    new_connection->socket().async_connect(
        tcp::endpoint(boost::asio::ip::address::from_string(address), port),
        [this, new_connection](const error_code& error) {
            handleConnect(new_connection, error);
        });
}

void NetworkManager::broadcastMessage(const Message& message)
{
    for (auto& peer : peers_)
    {
        peer.second->sendMessage(message);
    }
}

void NetworkManager::sendMessage(const Message&     message,
                                 const std::string& peer_key)
{
    auto it = peers_.find(peer_key);
    if (it != peers_.end())
    {
        LOG_INFO << "Sending message to peer: " << peer_key;
        it->second->sendMessage(message);
    } else {
        LOG_ERROR << "Peer: " << peer_key << ", not found";
    }
}

void NetworkManager::setMessageHandler(
    const MessageHandler::MessageCallback& handler)
{
    // TODO: now only text messages work
    message_handler_.registerHandler(MessageType::TEXT, handler);
}

void NetworkManager::doAccept()
{
    auto new_connection = PeerConnection::create(io_context_);

    acceptor_->async_accept(new_connection->socket(),
                            [this, new_connection](const error_code& error) {
                                handleAccept(new_connection, error);
                            });
}

void NetworkManager::handleAccept(
    std::shared_ptr<PeerConnection> new_connection, const error_code& error)
{
    if (!error)
    {
        LOG_INFO << "New connection accepted from "
                 << new_connection->socket().remote_endpoint();

        new_connection->setMessageHandler([this](const Message& msg) {
            message_handler_.handleMessage(msg);
        });

        new_connection->start();

        std::string peer_key =
            getPeerKey(new_connection->socket().remote_endpoint());
        peers_[peer_key] = new_connection;

        doAccept();
    } else {
        LOG_ERROR << "Error accepting new connection: " << error.message();
    }
}

void NetworkManager::handleConnect(
    std::shared_ptr<PeerConnection> new_connection, const error_code& error)
{
    if (!error)
    {
        LOG_INFO << "Connected to peer "
                 << new_connection->socket().remote_endpoint();

        new_connection->setMessageHandler([this](const Message& msg) {
            message_handler_.handleMessage(msg);
        });

        new_connection->start();

        std::string peer_key =
            getPeerKey(new_connection->socket().remote_endpoint());
        peers_[peer_key] = new_connection;
    } else {
        LOG_ERROR << "Error connecting to peer: " << error.message();
    }
}

std::string NetworkManager::getPeerKey(const tcp::endpoint& endpoint) const
{
    return endpoint.address().to_string() + ":" +
           std::to_string(endpoint.port());
}
