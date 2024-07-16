#ifndef PEER_HPP
#define PEER_HPP

#include <cstdint>
#include <string>
#include <string_view>

class Peer
{
  public:
    Peer(std::string_view peerId, std::string_view ipAddress,
         std::uint16_t port);

    std::string_view getPeerId() const;
    std::string_view getIpAddress() const;
    std::uint16_t    getPort() const;

    void setConnected(bool status);
    bool isConnected() const;

  private:
    std::string   m_peerId;
    std::string   m_ip;
    std::uint16_t m_port;
    bool          m_isConnected;
};

#endif // PEER_HPP
