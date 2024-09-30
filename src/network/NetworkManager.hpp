#ifndef NETWORK_MANAGER_HPP
#define NETWORK_MANAGER_HPP

#include <QElapsedTimer>
#include <QObject>
#include <boost/asio.hpp>
#include <memory>
#include <string>
#include <thread>
#include <unordered_map>

#include "FileTransfer.hpp"
#include "Logger.hpp"
#include "MessageHandler.hpp"
#include "NetworkSettings.hpp"
#include "PeerConnection.hpp"

class NetworkManager : public QObject,
                       public std::enable_shared_from_this<NetworkManager>
{
    Q_OBJECT

  public:
    using tcp = boost::asio::ip::tcp;
    using error_code = boost::system::error_code;
    using io_context = boost::asio::io_context;

    static std::shared_ptr<NetworkManager> create();

    void start(uint16_t port);
    void stop();
    bool changePort(uint16_t newPort);

    void connectToPeer(const std::string& address, uint16_t port);
    void broadcastMessage(const Message& message);

    void sendMessage(const Message& message, const std::string& peer_key);

    void setMessageHandler(const MessageHandler::MessageCallback& handler);
    void updateNetworkSettings(const NetworkSettings& settings);

    uint16_t getCurrentPort();

  public slots:
    void startSendingFile(const QString& filePath, const QString& peerKey);
    void cancelFileTransfer(const QString& file_id);
    void pauseFileTransfer(const QString& file_id);
    void resumeFileTransfer(const QString& file_id);

  signals:
    void peerConnectionResult(const QString& peerKey, bool success);
    void fileSendProgressUpdated(int progress);
    void portChanged(uint16_t newPort);
    void fileReceiveStarted(const QString& fileName, const QString& filePath,
                            qint64 fileSize);
    void fileReceiveProgressUpdated(int progress);
    void fileSendStarted(const QString& fileName, const QString& filePath,
                         qint64 fileSize);

  private:
    NetworkManager();

    void doAccept();
    void handleAccept(std::shared_ptr<PeerConnection> new_connection,
                      const error_code&               error);
    void handleConnect(std::shared_ptr<PeerConnection> new_connection,
                       const error_code&               error);

    void handleIncomingMessage(const Message&     message,
                               const std::string& peer_key);
    void handleFileMetadata(const FileMetadata& metadata,
                            const std::string&  peer_key);
    void handleChunkMessage(const ChunkMessage& chunk_msg,
                            const std::string&  peer_key);
    void handleChunkMetrics(const ChunkMetrics& metrics,
                            const std::string&  peer_key);
    void handleTransferComplete(const std::string& file_id, bool success);

    std::string getPeerKey(const tcp::endpoint& endpoint) const;
    std::unordered_map<std::string, std::shared_ptr<PeerConnection>>::iterator
    findPeerByFileId(const std::string& file_id);

    void updateFileTransferProgress(const std::string& file_id);

    io_context                        io_context_;
    std::unique_ptr<tcp::acceptor>    acceptor_;
    std::shared_ptr<io_context::work> work_;
    MessageHandler                    message_handler_;
    std::unordered_map<std::string, std::shared_ptr<PeerConnection>> peers_;
    std::shared_ptr<FileTransfer> file_transfer_;
    NetworkSettings               network_settings_;
    uint16_t                      current_port_;

    QElapsedTimer m_receiveProgressUpdateTimer;
    QElapsedTimer m_sendProgressUpdateTimer;
    int           m_progressUpdateInterval = 200;
};

#endif // NETWORK_MANAGER_HPP
