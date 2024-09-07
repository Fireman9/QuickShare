#ifndef MESSAGE_HANDLER_HPP
#define MESSAGE_HANDLER_HPP

#include <functional>
#include <memory>
#include <unordered_map>

#include "Message.hpp"
#include "logger.hpp"

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
        // TODO: assert rework
        static_assert(std::is_base_of<Message, T>::value,
                      "T must be derived from Message");
        handleMessage(message);
    }

  private:
    std::unordered_map<MessageType, MessageCallback> handlers_;
};

#endif // MESSAGE_HANDLER_HPP
