#ifndef TEXT_MESSAGE_HPP
#define TEXT_MESSAGE_HPP

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/string.hpp>
#include <sstream>
#include <vector>

#include "Message.hpp"

class TextMessage : public Message
{
  public:
    TextMessage() = default;
    explicit TextMessage(const std::string& text);

    MessageType getType() const override { return MessageType::TEXT; }

    const std::string& getText() const { return text_; }
    void               setText(const std::string& text) { text_ = text; }

    std::vector<uint8_t> serialize() const;
    static TextMessage   deserialize(const std::vector<uint8_t>& serialized);

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
