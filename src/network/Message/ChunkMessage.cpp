#include "Message/ChunkMessage.hpp"

ChunkMessage::ChunkMessage(const std::string& file_id, size_t chunk_number,
                           size_t offset, const std::vector<char>& data) :
    file_id_(file_id),
    chunk_number_(chunk_number), offset_(offset), data_(data)
{}

std::string ChunkMessage::serialize() const
{
    std::ostringstream            oss;
    boost::archive::text_oarchive oa(oss);
    oa << *this;
    return oss.str();
}

ChunkMessage ChunkMessage::deserialize(const std::string& serialized)
{
    ChunkMessage                  chunk;
    std::istringstream            iss(serialized);
    boost::archive::text_iarchive ia(iss);
    ia >> chunk;
    return chunk;
}
