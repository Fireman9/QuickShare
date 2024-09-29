#ifndef MESSAGE_HANDLER_HPP
#define MESSAGE_HANDLER_HPP

#include <functional>
#include <memory>
#include <unordered_map>

#include "Logger.hpp"
#include "Message/Message.hpp"

class MessageHandler
{
  public:
    using MessageCallback = std::function<void(const Message&)>;

    MessageHandler() = default;
    ~MessageHandler() = default;

    void registerHandler(MessageType type, MessageCallback callback);
    void handleMessage(const Message& message);

    template <typename T>
    void handleTypedMessage(const T& message)
    {
        if constexpr (std::is_base_of_v<Message, T>)
        {
            handleMessage(message);
        } else {
            static_assert(std::is_base_of_v<Message, T>,
                          "T must be derived from Message");
        }
    }

  private:
    std::unordered_map<MessageType, MessageCallback> handlers_;
};

#endif // MESSAGE_HANDLER_HPP
