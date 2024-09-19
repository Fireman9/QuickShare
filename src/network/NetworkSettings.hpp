#ifndef NETWORK_SETTINGS_HPP
#define NETWORK_SETTINGS_HPP

#include <boost/asio.hpp>
#include <cstdint>

#include "logger.hpp"

using socket_base = boost::asio::socket_base;
using tcp = boost::asio::ip::tcp;

class NetworkSettings
{
  public:
    NetworkSettings() :
        window_size_(65536), // 64KB
        disable_nagle_(true), keep_alive_(true), reuse_address_(true),
        send_buffer_size_(1048576),    // 1MB
        receive_buffer_size_(1048576), // 1MB
        min_buffer_size_(8192),        // 8KB
        max_buffer_size_(16777216)     // 16MB
    {}

    void setWindowSize(int size) { window_size_ = size; }
    int  getWindowSize() const { return window_size_; }

    void setDisableNagle(bool disable) { disable_nagle_ = disable; }
    bool getDisableNagle() const { return disable_nagle_; }

    void setKeepAlive(bool enable) { keep_alive_ = enable; }
    bool getKeepAlive() const { return keep_alive_; }

    void setReuseAddress(bool enable) { reuse_address_ = enable; }
    bool getReuseAddress() const { return reuse_address_; }

    void setSendBufferSize(int size) { send_buffer_size_ = size; }
    int  getSendBufferSize() const { return send_buffer_size_; }

    void setReceiveBufferSize(int size) { receive_buffer_size_ = size; }
    int  getReceiveBufferSize() const { return receive_buffer_size_; }

    void updateBufferSizes(size_t current_chunk_size)
    {
        size_t optimal_buffer_size = current_chunk_size * 2;
        optimal_buffer_size =
            std::clamp(optimal_buffer_size, min_buffer_size_, max_buffer_size_);

        setSendBufferSize(optimal_buffer_size);
        setReceiveBufferSize(optimal_buffer_size);
    }

    template <typename SocketType>
    void applyToSocket(SocketType& socket) const
    {
        boost::system::error_code ec;

        socket.set_option(socket_base::send_buffer_size(send_buffer_size_), ec);
        if (ec)
        {
            LOG_WARNING << "Failed to set send buffer size: " << ec.message();
        }

        socket.set_option(
            socket_base::receive_buffer_size(receive_buffer_size_), ec);
        if (ec)
        {
            LOG_WARNING << "Failed to set receive buffer size: "
                        << ec.message();
        }

        socket.set_option(tcp::no_delay(disable_nagle_), ec);
        if (ec)
        {
            LOG_WARNING << "Failed to set TCP no delay option: "
                        << ec.message();
        }

        socket.set_option(socket_base::keep_alive(keep_alive_), ec);
        if (ec)
        {
            LOG_WARNING << "Failed to set keep alive option: " << ec.message();
        }

        socket.set_option(socket_base::reuse_address(reuse_address_), ec);
        if (ec)
        {
            LOG_WARNING << "Failed to set reuse address option: "
                        << ec.message();
        }
    }

  private:
    int    window_size_;
    bool   disable_nagle_;
    bool   keep_alive_;
    bool   reuse_address_;
    int    send_buffer_size_;
    int    receive_buffer_size_;
    size_t min_buffer_size_;
    size_t max_buffer_size_;
};

#endif // NETWORK_SETTINGS_HPP
