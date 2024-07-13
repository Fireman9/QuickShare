#ifndef __MESSAGE_HEADER_HPP__
#define __MESSAGE_HEADER_HPP__

#include <cstdint>
#include <string>

#include <boost/serialization/serialization.hpp>

struct MessageHeader
{
    enum class Type { TEXT, FILE };

    Type        type;
    std::string textContent;
    std::string fileName;
    uint64_t    totalFileSize;
    uint32_t    chunkNumber;
    uint32_t    totalChunks;
    uint32_t    bodySize;

    template <class Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
        ar & type;
        ar & textContent;
        ar & fileName;
        ar & totalFileSize;
        ar & chunkNumber;
        ar & totalChunks;
        ar & bodySize;
    }
};

#endif // __MESSAGE_HEADER_HPP__
