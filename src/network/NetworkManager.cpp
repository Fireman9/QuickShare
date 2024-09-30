#include "NetworkManager.hpp"

std::shared_ptr<NetworkManager> NetworkManager::create()
{
    return std::shared_ptr<NetworkManager>(new NetworkManager());
}

NetworkManager::NetworkManager() :
    QObject(), work_(std::make_shared<io_context::work>(io_context_)),
    file_transfer_(
        std::make_shared<FileTransfer>(std::make_shared<FileSystemManager>())),
    network_settings_(), current_port_(8080)
{
    file_transfer_->setChunkReadyCallback([this](const ChunkMessage& chunk) {
        auto it = findPeerByFileId(chunk.getFileId());
        if (it != peers_.end())
        {
            it->second->sendMessage(chunk);
        }
        updateFileTransferProgress(chunk.getFileId());
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
            LOG_INFO(QString("File transfer %1 for file ID: %2")
                         .arg(success ? "completed" : "failed")
                         .arg(file_id.c_str()));
            handleTransferComplete(file_id, success);
        });

    m_sendProgressUpdateTimer.start();
    m_receiveProgressUpdateTimer.start();
}

void NetworkManager::start(uint16_t port)
{
    current_port_ = port;
    try
    {
        acceptor_ = std::make_unique<tcp::acceptor>(
            io_context_, tcp::endpoint(tcp::v4(), port));
        doAccept();

        LOG_INFO(QString("NetworkManager started on port: %1").arg(port));
        std::thread([this]() { io_context_.run(); }).detach();
    } catch (const std::exception& e)
    {
        LOG_ERROR(QString("Error starting NetworkManager: %1").arg(e.what()));
    }
}

void NetworkManager::stop()
{
    work_.reset();

    if (acceptor_ && acceptor_->is_open())
    {
        boost::system::error_code ec;
        acceptor_->close(ec);
        if (ec)
        {
            LOG_ERROR(QString("Error closing acceptor: %1")
                          .arg(ec.message().c_str()));
        }
    }

    for (auto& peer : peers_)
    {
        peer.second->stop();
    }
    peers_.clear();

    io_context_.stop();

    LOG_INFO("NetworkManager stopped");
}

bool NetworkManager::changePort(uint16_t newPort)
{
    if (newPort == current_port_)
    {
        return true;
    }

    // TODO:
    // LOG_INFO << "Changing port from " << current_port_ << " to " << newPort;

    stop();

    io_context_.restart();
    work_ = std::make_shared<io_context::work>(io_context_);

    try
    {
        start(newPort);
        emit portChanged(newPort);
        return true;
    } catch (const std::exception& e)
    {
        LOG_ERROR(QString("Error changing port: %1").arg(e.what()));
        start(current_port_);
        return false;
    }
}

void NetworkManager::connectToPeer(const std::string& address, uint16_t port)
{
    auto new_connection = PeerConnection::create(io_context_);
    new_connection->setNetworkSettings(network_settings_);

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
        LOG_INFO(QString("Sending message to peer: %1").arg(peer_key.c_str()));
        it->second->sendMessage(message);
    } else {
        LOG_ERROR(QString("Peer: %1, not found").arg(peer_key.c_str()));
    }
}

void NetworkManager::startSendingFile(const QString& filePath,
                                      const QString& peerKey)
{
    QFileInfo fileInfo(filePath);
    emit      fileSendStarted(fileInfo.fileName(), fileInfo.filePath(),
                              fileInfo.size());
    file_transfer_->startSending(filePath.toStdString(), peerKey.toStdString());
    m_sendProgressUpdateTimer.start();
}

void NetworkManager::cancelFileTransfer(const QString& file_id)
{
    LOG_INFO(QString("Cancelling file transfer for file ID: %1").arg(file_id));
    file_transfer_->cancelTransfer(file_id.toStdString());
}

void NetworkManager::pauseFileTransfer(const QString& file_id)
{
    LOG_INFO(QString("Pausing file transfer for file ID: %1").arg(file_id));
    file_transfer_->pauseTransfer(file_id.toStdString());
}

void NetworkManager::resumeFileTransfer(const QString& file_id)
{
    LOG_INFO(QString("Resuming file transfer for file ID: %1").arg(file_id));
    file_transfer_->resumeTransfer(file_id.toStdString());
}

void NetworkManager::updateFileTransferProgress(const std::string& file_id)
{
    if (m_sendProgressUpdateTimer.elapsed() >= m_progressUpdateInterval)
    {
        double progress = file_transfer_->getTransferProgress(file_id);
        emit   fileSendProgressUpdated(static_cast<int>(progress));
        m_sendProgressUpdateTimer.restart();
    }
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

void NetworkManager::updateNetworkSettings(const NetworkSettings& settings)
{
    network_settings_ = settings;

    for (auto& peer : peers_)
    {
        peer.second->setNetworkSettings(network_settings_);
    }
}

uint16_t NetworkManager::getCurrentPort()
{
    return current_port_;
}

void NetworkManager::doAccept()
{
    auto new_connection = PeerConnection::create(io_context_);

    if (acceptor_ && acceptor_->is_open())
    {
        acceptor_->async_accept(
            new_connection->socket(),
            [this, new_connection](const error_code& error) {
                handleAccept(new_connection, error);
            });
    } else {
        LOG_ERROR("Acceptor is not open, cannot accept new connections");
    }
}

void NetworkManager::handleAccept(
    std::shared_ptr<PeerConnection> new_connection, const error_code& error)
{
    if (!error)
    {
        LOG_INFO(QString("New connection accepted from %1")
                     .arg(new_connection->socket()
                              .remote_endpoint()
                              .address()
                              .to_string()
                              .c_str()));

        std::string peer_key =
            getPeerKey(new_connection->socket().remote_endpoint());
        peers_[peer_key] = new_connection;

        new_connection->setMessageHandler([this, peer_key](const Message& msg) {
            this->handleIncomingMessage(msg, peer_key);
        });

        new_connection->setNetworkSettings(network_settings_);
        new_connection->start();
        doAccept();
    } else {
        LOG_ERROR(QString("Error accepting new connection: %1")
                      .arg(error.message().c_str()));
    }
}

void NetworkManager::handleConnect(
    std::shared_ptr<PeerConnection> new_connection, const error_code& error)
{
    if (!error)
    {
        LOG_INFO(QString("Connected to peer %1")
                     .arg(new_connection->socket()
                              .remote_endpoint()
                              .address()
                              .to_string()
                              .c_str()));

        std::string peer_key =
            getPeerKey(new_connection->socket().remote_endpoint());
        peers_[peer_key] = new_connection;

        new_connection->setMessageHandler([this, peer_key](const Message& msg) {
            this->handleIncomingMessage(msg, peer_key);
        });

        new_connection->start();
    } else {
        LOG_ERROR(QString("Error connecting to peer: %1")
                      .arg(error.message().c_str()));
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
        {
            handleChunkMessage(static_cast<const ChunkMessage&>(message),
                               peer_key);
            // TODO:
            // updateFileTransferProgress(
            //     static_cast<const ChunkMessage&>(message).getFileId());
            break;
        }
        case MessageType::CHUNK_METRICS:
            handleChunkMetrics(static_cast<const ChunkMetrics&>(message),
                               peer_key);
            break;
        default: LOG_ERROR("Unknown message type received");
    }
}

void NetworkManager::handleFileMetadata(const FileMetadata& metadata,
                                        const std::string&  peer_key)
{
    LOG_INFO(QString("Received file metadata for file: %1 from peer: %2")
                 .arg(metadata.getFileName().c_str())
                 .arg(peer_key.c_str()));
    file_transfer_->startReceiving(metadata);

    QString fileName = QString::fromStdString(metadata.getFileName());
    QString filePath = QDir::currentPath() + "/" + fileName;
    qint64  fileSize = metadata.getFileSize();

    emit fileReceiveStarted(fileName, filePath, fileSize);
    m_receiveProgressUpdateTimer.start();
}

void NetworkManager::handleChunkMessage(const ChunkMessage& chunk_msg,
                                        const std::string&  peer_key)
{
    LOG_INFO(QString("Received chunk with offset %1 for file ID: %2")
                 .arg(chunk_msg.getOffset())
                 .arg(chunk_msg.getFileId().c_str()));
    file_transfer_->handleIncomingChunk(chunk_msg);

    if (m_receiveProgressUpdateTimer.elapsed() >= m_progressUpdateInterval)
    {
        double progress =
            file_transfer_->getTransferProgress(chunk_msg.getFileId());
        emit fileReceiveProgressUpdated(static_cast<int>(progress));
        m_receiveProgressUpdateTimer.restart();
    }

    ChunkMetrics metrics(chunk_msg.getFileId(), chunk_msg.getOffset(),
                         chunk_msg.getData().size(),
                         std::chrono::system_clock::now());
    auto         it = peers_.find(peer_key);
    if (it != peers_.end())
    {
        it->second->sendMessage(metrics);
    } else {
        LOG_ERROR("Peer not found for sending chunk metrics");
    }
}

void NetworkManager::handleChunkMetrics(const ChunkMetrics& metrics,
                                        const std::string&  peer_key)
{
    auto received_time = metrics.getReceivedTime();
    auto current_time = std::chrono::system_clock::now();
    auto latency = std::chrono::duration_cast<std::chrono::microseconds>(
        current_time - received_time);

    LOG_INFO(QString("Received metrics for chunk with offset %1 of file ID: "
                     "%2, size: %3 bytes, latency: %4 microseconds")
                 .arg(metrics.getOffset())
                 .arg(metrics.getFileId().c_str())
                 .arg(metrics.getChunkSize())
                 .arg(latency.count()));

    file_transfer_->handleChunkMetrics(metrics.getFileId(), metrics.getOffset(),
                                       metrics.getChunkSize(), latency);

    size_t optimal_chunk_size =
        file_transfer_->getOptimalChunkSize(metrics.getFileId());
    network_settings_.updateBufferSizes(optimal_chunk_size);
    for (auto& peer : peers_)
    {
        peer.second->setNetworkSettings(network_settings_);
    }
}

void NetworkManager::handleTransferComplete(const std::string& file_id,
                                            bool               success)
{
    int finalProgress = success ? 100 : 0;

    bool isSending = file_transfer_->isFileSending(file_id);
    if (isSending)
    {
        emit fileSendProgressUpdated(finalProgress);
    } else {
        emit fileReceiveProgressUpdated(finalProgress);
    }
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
