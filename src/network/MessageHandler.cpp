#include "MessageHandler.hpp"

void MessageHandler::registerHandler(MessageType type, MessageCallback callback)
{
    handlers_[type] = std::move(callback);
}

void MessageHandler::handleMessage(const Message& message)
{
    auto it = handlers_.find(message.getType());
    if (it != handlers_.end())
    {
        it->second(message);
    } else {
        LOG_ERROR("No handler registered for message type");
    }
}