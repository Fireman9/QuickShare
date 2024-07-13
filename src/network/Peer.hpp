#ifndef PEER_HPP
#define PEER_HPP

#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>
#include <cstdint>
#include <string>
#include <string_view>

class Peer
{
  public:
    enum class ConnectionStatus : std::uint8_t {
        Disconnected,
        Connecting,
        Connected
    };

    Peer(boost::asio::io_context& ioContext, std::string_view uniqueCode,
         std::string_view ip, std::uint16_t port);

    std::string_view getUniqueCode() const noexcept;
    std::string_view getIP() const noexcept;
    std::uint16_t    getPort() const noexcept;
    ConnectionStatus getConnectionStatus() const noexcept;
    void             setConnectionStatus(ConnectionStatus status) noexcept;

  private:
    std::string                  m_uniqueCode;
    std::string                  m_ip;
    std::uint16_t                m_port;
    ConnectionStatus             m_connectionStatus;
    boost::asio::ip::tcp::socket m_socket;
};

#endif // PEER_HPP
