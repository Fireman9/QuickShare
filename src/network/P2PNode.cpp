#include "P2PNode.hpp"
#include <iostream>

P2PNode::P2PNode(io_context& io_context) :
    m_ioContext(io_context),
    m_socket(std::make_unique<tcp::socket>(io_context)),
    m_receiveBuffer(MAX_BUFFER_SIZE)
{}

P2PNode::~P2PNode()
{
    disconnect();
}

bool P2PNode::connect(const Peer& peer)
{
    if (isPeerConnected())
    {
        std::cout << "Already connected to a peer. Disconnect first.\n";
        return false;
    }

    tcp::endpoint endpoint(
        boost::asio::ip::address::from_string(std::string(peer.getIpAddress())),
        peer.getPort());

    error_code error;
    m_socket->connect(endpoint, error);

    if (!error)
    {
        m_currentPeer = std::make_shared<Peer>(peer);
        m_currentPeer->setConnected(true);
        std::cout << "Connected to peer: " << peer.getPeerId() << "\n";
        return true;
    } else {
        std::cout << "Connection failed: " << error.message() << "\n";
        return false;
    }
}

void P2PNode::disconnect()
{
    if (m_socket->is_open())
    {
        error_code error;
        m_socket->shutdown(tcp::socket::shutdown_both, error);
        m_socket->close();
    }
    if (m_currentPeer)
    {
        m_currentPeer->setConnected(false);
        m_currentPeer.reset();
    }
    std::cout << "Disconnected from peer.\n";
}

bool P2PNode::sendData(const std::vector<uint8_t>& data)
{
    if (!isPeerConnected())
    {
        std::cout << "Not connected to any peer.\n";
        return false;
    }

    error_code error;
    boost::asio::write(*m_socket, boost::asio::buffer(data), error);

    if (!error)
    {
        std::cout << "Data sent successfully.\n";
        return true;
    } else {
        std::cout << "Send failed: " << error.message() << "\n";
        return false;
    }
}

std::vector<uint8_t> P2PNode::receiveData()
{
    if (!isPeerConnected())
    {
        std::cout << "Not connected to any peer.\n";
        return {};
    }

    error_code  error;
    std::size_t len =
        m_socket->read_some(boost::asio::buffer(m_receiveBuffer), error);

    if (!error)
    {
        std::cout << "Received " << len << " bytes.\n";
        return std::vector<uint8_t>(m_receiveBuffer.begin(),
                                    m_receiveBuffer.begin() + len);
    } else {
        std::cout << "Receive failed: " << error.message() << "\n";
        return {};
    }
}

bool P2PNode::sendFile(const std::string& filePath)
{
    if (!isPeerConnected())
    {
        std::cout << "Not connected to any peer.\n";
        return false;
    }

    return m_fileTransfer.sendFile(filePath,
                                   [this](const std::vector<uint8_t>& chunk) {
                                       return this->sendData(chunk);
                                   });
}

bool P2PNode::receiveFile(const std::string& saveDir,
                          const std::string& fileName, std::size_t fileSize)
{
    if (!isPeerConnected())
    {
        std::cout << "Not connected to any peer.\n";
        return false;
    }

    return m_fileTransfer.receiveFile(saveDir, fileName, fileSize,
                                      [this]() { return this->receiveData(); });
}

float P2PNode::getFileTransferProgress() const
{
    return m_fileTransfer.getProgress();
}

std::shared_ptr<Peer> P2PNode::getCurrentPeer() const
{
    return m_currentPeer;
}

bool P2PNode::isPeerConnected() const
{
    return m_currentPeer && m_currentPeer->isConnected() && m_socket->is_open();
}

void P2PNode::handleConnect(const error_code& error)
{
    if (!error)
    {
        std::cout << "Async connection successful.\n";
        // Start receiving data or perform other post-connection operations
    } else {
        std::cout << "Async connection failed: " << error.message() << "\n";
    }
}

void P2PNode::handleSend(const error_code& error, std::size_t bytes_transferred)
{
    if (!error)
    {
        std::cout << "Async send successful. Bytes transferred: "
                  << bytes_transferred << "\n";
    } else {
        std::cout << "Async send failed: " << error.message() << "\n";
    }
}

void P2PNode::handleReceive(const error_code& error,
                            std::size_t       bytes_transferred)
{
    if (!error)
    {
        std::cout << "Async receive successful. Bytes received: "
                  << bytes_transferred << "\n";
        // Process received data
    } else {
        std::cout << "Async receive failed: " << error.message() << "\n";
    }
}
