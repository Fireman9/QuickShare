#include "NetworkManager.hpp"

std::shared_ptr<NetworkManager> NetworkManager::create()
{
    return std::shared_ptr<NetworkManager>(new NetworkManager());
}

NetworkManager::NetworkManager() :
    work_(std::make_shared<io_context::work>(io_context_)),
    file_transfer_(
        std::make_shared<FileTransfer>(std::make_shared<FileSystemManager>()))
{
    file_transfer_->setChunkReadyCallback([this](const ChunkMessage& chunk) {
        auto it = findPeerByFileId(chunk.getFileId());
        if (it != peers_.end())
        {
            it->second->sendMessage(chunk);
        }
    });

    file_transfer_->setFileMetadataCallback(
        [this](const FileMetadata& metadata) {
            auto it = findPeerByFileId(metadata.getFileId());
            if (it != peers_.end())
            {
                it->second->sendMessage(metadata);
            }
        });

    file_transfer_->setTransferCompleteCallback(
        [this](const std::string& file_id, bool success) {
            LOG_INFO << "File transfer " << (success ? "completed" : "failed")
                     << " for file ID: " << file_id;
            // TODO: add more logic here
        });
}

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

void NetworkManager::startSendingFile(const std::string& file_path,
                                      const std::string& peer_key)
{
    LOG_INFO << "Starting to send file: " << file_path
             << " to peer: " << peer_key;
    file_transfer_->startSending(file_path, peer_key);
}

void NetworkManager::cancelFileTransfer(const std::string& file_id)
{
    LOG_INFO << "Cancelling file transfer for file ID: " << file_id;
    file_transfer_->cancelTransfer(file_id);
}

void NetworkManager::pauseFileTransfer(const std::string& file_id)
{
    LOG_INFO << "Pausing file transfer for file ID: " << file_id;
    file_transfer_->pauseTransfer(file_id);
}

void NetworkManager::resumeFileTransfer(const std::string& file_id)
{
    LOG_INFO << "Resuming file transfer for file ID: " << file_id;
    file_transfer_->resumeTransfer(file_id);
}

double NetworkManager::getFileTransferProgress(const std::string& file_id)
{
    return file_transfer_->getTransferProgress(file_id);
}

void NetworkManager::setMessageHandler(
    const MessageHandler::MessageCallback& handler)
{
    message_handler_.registerHandler(MessageType::TEXT, handler);

    for (const auto& peer : peers_)
    {
        peer.second->setMessageHandler(
            [this, peer_key = peer.first](const Message& msg) {
                this->handleIncomingMessage(msg, peer_key);
            });
    }
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

        std::string peer_key =
            getPeerKey(new_connection->socket().remote_endpoint());
        peers_[peer_key] = new_connection;

        new_connection->setMessageHandler([this, peer_key](const Message& msg) {
            this->handleIncomingMessage(msg, peer_key);
        });

        new_connection->start();
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

        std::string peer_key =
            getPeerKey(new_connection->socket().remote_endpoint());
        peers_[peer_key] = new_connection;

        new_connection->setMessageHandler([this, peer_key](const Message& msg) {
            this->handleIncomingMessage(msg, peer_key);
        });

        new_connection->start();
    } else {
        LOG_ERROR << "Error connecting to peer: " << error.message();
    }
}

void NetworkManager::handleIncomingMessage(const Message&     message,
                                           const std::string& peer_key)
{
    switch (message.getType())
    {
        case MessageType::TEXT: message_handler_.handleMessage(message); break;
        case MessageType::FILE_METADATA:
            handleFileMetadata(static_cast<const FileMetadata&>(message),
                               peer_key);
            break;
        case MessageType::CHUNK:
            handleChunkMessage(static_cast<const ChunkMessage&>(message),
                               peer_key);
            break;
        case MessageType::CHUNK_METRICS:
            handleChunkMetrics(static_cast<const ChunkMetrics&>(message),
                               peer_key);
            break;
        default: LOG_ERROR << "Unknown message type received";
    }
}

void NetworkManager::handleFileMetadata(const FileMetadata& metadata,
                                        const std::string&  peer_key)
{
    LOG_INFO << "Received file metadata for file: " << metadata.getFileName()
             << " from peer: " << peer_key;
    file_transfer_->startReceiving(metadata);
}

void NetworkManager::handleChunkMessage(const ChunkMessage& chunk_msg,
                                        const std::string&  peer_key)
{
    LOG_INFO << "Received chunk with offset " << chunk_msg.getOffset()
             << " for file ID: " << chunk_msg.getFileId();
    file_transfer_->handleIncomingChunk(chunk_msg);

    ChunkMetrics metrics(chunk_msg.getFileId(), chunk_msg.getOffset(),
                         chunk_msg.getData().size(),
                         std::chrono::system_clock::now());
    auto         it = peers_.find(peer_key);
    if (it != peers_.end())
    {
        it->second->sendMessage(metrics);
    } else {
        LOG_ERROR << "Peer not found for sending chunk metrics";
    }
}

void NetworkManager::handleChunkMetrics(const ChunkMetrics& metrics,
                                        const std::string&  peer_key)
{
    auto received_time = metrics.getReceivedTime();
    auto current_time = std::chrono::system_clock::now();
    auto latency = std::chrono::duration_cast<std::chrono::microseconds>(
        current_time - received_time);

    LOG_INFO << "Received metrics for chunk with offset " << metrics.getOffset()
             << " of file ID: " << metrics.getFileId()
             << ", size: " << metrics.getChunkSize() << " bytes"
             << ", latency: " << latency.count() << " microseconds";

    file_transfer_->handleChunkMetrics(metrics.getFileId(), metrics.getOffset(),
                                       metrics.getChunkSize(), latency);
}

std::string NetworkManager::getPeerKey(const tcp::endpoint& endpoint) const
{
    return endpoint.address().to_string() + ":" +
           std::to_string(endpoint.port());
}

std::unordered_map<std::string, std::shared_ptr<PeerConnection>>::iterator
NetworkManager::findPeerByFileId(const std::string& file_id)
{
    std::string peer_key = file_id.substr(file_id.find('_') + 1);
    return peers_.find(peer_key);
}
