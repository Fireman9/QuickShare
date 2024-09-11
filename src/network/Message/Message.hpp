#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <boost/serialization/access.hpp>
#include <boost/serialization/string.hpp>
#include <cstdint>
#include <string>
#include <vector>

enum class MessageType : uint8_t {
    TEXT,
    FILE_METADATA,
    CHUNK,
};

class Message
{
  public:
    Message() = default;
    virtual ~Message() = default;

    virtual MessageType getType() const = 0;

    virtual std::vector<uint8_t> serialize() const = 0;
};

#endif // MESSAGE_HPP
