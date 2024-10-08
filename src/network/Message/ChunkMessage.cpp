#include "ChunkMessage.hpp"

ChunkMessage::ChunkMessage(const std::string& file_id, size_t offset,
                           const std::vector<uint8_t>& data) :
    file_id_(file_id),
    offset_(offset), data_(data)
{}

std::vector<uint8_t> ChunkMessage::serialize() const
{
    std::ostringstream              oss;
    boost::archive::binary_oarchive oa(oss, boost::archive::no_header);
    oa << *this;
    const std::string& str = oss.str();
    return std::vector<uint8_t>(str.begin(), str.end());
}

ChunkMessage ChunkMessage::deserialize(const std::vector<uint8_t>& serialized)
{
    ChunkMessage                    chunk;
    std::string                     str(serialized.begin(), serialized.end());
    std::istringstream              iss(str);
    boost::archive::binary_iarchive ia(iss, boost::archive::no_header);
    ia >> chunk;
    return chunk;
}
