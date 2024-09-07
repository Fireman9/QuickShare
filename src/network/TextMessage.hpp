#ifndef TEXT_MESSAGE_HPP
#define TEXT_MESSAGE_HPP

#include "Message.hpp"
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/string.hpp>
#include <sstream>

class TextMessage : public Message
{
  public:
    TextMessage() = default;
    explicit TextMessage(const std::string& text) : text_(text) {}

    MessageType getType() const override { return MessageType::TEXT; }

    const std::string& getText() const { return text_; }
    void               setText(const std::string& text) { text_ = text; }

    std::string        serialize() const;
    static TextMessage deserialize(const std::string& serialized);

  private:
    friend class boost::serialization::access;

    template <class Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
        ar & text_;
    }

    std::string text_;
};

#endif // TEXT_MESSAGE_HPP
