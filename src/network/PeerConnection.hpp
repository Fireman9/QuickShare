#ifndef PEER_CONNECTION_HPP
#define PEER_CONNECTION_HPP

#include <boost/asio.hpp>
#include <functional>
#include <memory>
#include <queue>

#include "Logger.hpp"
#include "Message/ChunkMessage.hpp"
#include "Message/ChunkMetrics.hpp"
#include "Message/FileMetadata.hpp"
#include "Message/Message.hpp"
#include "Message/TextMessage.hpp"
#include "NetworkSettings.hpp"

class PeerConnection : public std::enable_shared_from_this<PeerConnection>
{
  public:
    using tcp = boost::asio::ip::tcp;
    using error_code = boost::system::error_code;
    using io_context = boost::asio::io_context;
    using MessageHandler = std::function<void(const Message&)>;

    static std::shared_ptr<PeerConnection> create(io_context& io_context);

    void start();
    void stop();

    void sendMessage(const Message& message);
    void setMessageHandler(MessageHandler handler);
    void setNetworkSettings(const NetworkSettings& settings);

    tcp::socket& socket();

  private:
    explicit PeerConnection(io_context& io_context);

    void doRead();
    void readMessageType();
    void readMessageLength();
    void readMessageBody();
    void handleRead(const error_code& error, size_t bytes_transferred);
    void processReceivedMessage();

    void doWrite();
    void handleWrite(const error_code& error);

    void applyNetworkSettings();

    template <typename T>
    std::vector<uint8_t> serializeMessage(const T& message);

    tcp::socket                      socket_;
    std::vector<uint8_t>             read_buffer_;
    std::queue<std::vector<uint8_t>> write_queue_;
    MessageHandler                   message_handler_;
    bool                             is_writing_;
    bool                             is_connected_;
    uint32_t                         message_length_;
    MessageType                      current_message_type_;
    NetworkSettings                  network_settings_;

    static constexpr size_t MESSAGE_TYPE_SIZE = sizeof(MessageType);
    static constexpr size_t MESSAGE_LENGTH_SIZE = sizeof(uint32_t);
};

template <typename T>
std::vector<uint8_t> PeerConnection::serializeMessage(const T& message)
{
    std::vector<uint8_t> serialized_data = message.serialize();
    uint32_t             length = static_cast<uint32_t>(serialized_data.size());
    MessageType          type = message.getType();

    std::vector<uint8_t> data_to_send(sizeof(MessageType) + sizeof(length) +
                                      serialized_data.size());

    std::memcpy(data_to_send.data(), &type, sizeof(MessageType));
    std::memcpy(data_to_send.data() + sizeof(MessageType), &length,
                sizeof(length));
    std::memcpy(data_to_send.data() + sizeof(MessageType) + sizeof(length),
                serialized_data.data(), serialized_data.size());

    return data_to_send;
}

#endif // PEER_CONNECTION_HPP
