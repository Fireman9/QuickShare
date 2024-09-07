#include "TextMessage.hpp"

std::string TextMessage::serialize() const
{
    std::ostringstream            oss;
    boost::archive::text_oarchive oa(oss);
    oa << *this;
    return oss.str();
}

TextMessage TextMessage::deserialize(const std::string& serialized)
{
    TextMessage                   msg;
    std::istringstream            iss(serialized);
    boost::archive::text_iarchive ia(iss);
    ia >> msg;
    return msg;
}
