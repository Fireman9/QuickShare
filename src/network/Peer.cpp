#include "Peer.hpp"

Peer::Peer(boost::asio::io_context& ioContext, std::string_view uniqueCode,
           std::string_view ip, std::uint16_t port) :
    m_uniqueCode(uniqueCode),
    m_ip(ip), m_port(port), m_connectionStatus(ConnectionStatus::Disconnected),
    m_socket(ioContext)
{}

std::string_view Peer::getUniqueCode() const noexcept
{
    return m_uniqueCode;
}

std::string_view Peer::getIP() const noexcept
{
    return m_ip;
}

std::uint16_t Peer::getPort() const noexcept
{
    return m_port;
}

Peer::ConnectionStatus Peer::getConnectionStatus() const noexcept
{
    return m_connectionStatus;
}

void Peer::setConnectionStatus(ConnectionStatus status) noexcept
{
    m_connectionStatus = status;
}
