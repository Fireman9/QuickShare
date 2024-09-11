#include "Message/TextMessage.hpp"

TextMessage::TextMessage(const std::string& text) : text_(text) {}

std::vector<uint8_t> TextMessage::serialize() const
{
    std::ostringstream              oss;
    boost::archive::binary_oarchive oa(oss, boost::archive::no_header);
    oa << *this;
    const std::string& str = oss.str();
    return std::vector<uint8_t>(str.begin(), str.end());
}

TextMessage TextMessage::deserialize(const std::vector<uint8_t>& serialized)
{
    TextMessage                     msg;
    std::string                     str(serialized.begin(), serialized.end());
    std::istringstream              iss(str);
    boost::archive::binary_iarchive ia(iss, boost::archive::no_header);
    ia >> msg;
    return msg;
}
