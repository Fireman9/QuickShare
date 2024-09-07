#ifndef TEXT_MESSAGE_HPP
#define TEXT_MESSAGE_HPP

#include "Message.hpp"

class TextMessage : public Message
{
  public:
    TextMessage() = default;
    explicit TextMessage(const std::string& text) : text_(text) {}

    MessageType getType() const override { return MessageType::TEXT; }

    const std::string& getText() const { return text_; }
    void               setText(const std::string& text) { text_ = text; }

  private:
    std::string text_;
};

#endif // TEXT_MESSAGE_HPP
