#ifndef P2P_NODE_HPP
#define P2P_NODE_HPP

#include <boost/asio.hpp>
#include <memory>
#include <vector>

#include "FileTransferModule.hpp"
#include "Peer.hpp"

class P2PNode
{
  public:
    P2PNode(boost::asio::io_context& io_context);
    ~P2PNode();

    bool connect(const Peer& peer);
    void disconnect();

    bool                 sendData(const std::vector<uint8_t>& data);
    std::vector<uint8_t> receiveData();

    bool  sendFile(const std::string& filePath);
    bool  receiveFile(const std::string& saveDir, const std::string& fileName,
                      std::size_t fileSize);
    float getFileTransferProgress() const;

    std::shared_ptr<Peer> getCurrentPeer() const;
    bool                  isPeerConnected() const;

  private:
    using error_code = boost::system::error_code;
    using tcp = boost::asio::ip::tcp;
    using io_context = boost::asio::io_context;

    void handleConnect(const error_code& error);
    void handleSend(const error_code& error, std::size_t bytes_transferred);
    void handleReceive(const error_code& error, std::size_t bytes_transferred);

    io_context&                  m_ioContext;
    std::unique_ptr<tcp::socket> m_socket;
    std::shared_ptr<Peer>        m_currentPeer;
    std::vector<uint8_t>         m_receiveBuffer;
    static const std::size_t     MAX_BUFFER_SIZE = 65536; // 64KB buffer

    FileTransferModule m_fileTransfer;
};

#endif // P2P_NODE_HPP
