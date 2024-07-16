#include "Peer.hpp"

Peer::Peer(std::string_view peerId, std::string_view ipAddress,
           std::uint16_t port) :
    m_peerId(peerId),
    m_ip(ipAddress), m_port(port), m_isConnected(false)
{}

std::string_view Peer::getPeerId() const
{
    return m_peerId;
}

std::string_view Peer::getIpAddress() const
{
    return m_ip;
}

std::uint16_t Peer::getPort() const
{
    return m_port;
}

void Peer::setConnected(bool status)
{
    m_isConnected = status;
}

bool Peer::isConnected() const
{
    return m_isConnected;
}
