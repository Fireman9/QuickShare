#ifndef CHUNK_MESSAGE_HPP
#define CHUNK_MESSAGE_HPP

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
#include <sstream>
#include <vector>

#include "Message/Message.hpp"

class ChunkMessage : public Message
{
  public:
    ChunkMessage() = default;
    ChunkMessage(const std::string& file_id, size_t chunk_number, size_t offset,
                 const std::vector<char>& data);

    MessageType getType() const override { return MessageType::CHUNK; }

    const std::string&       getFileId() const { return file_id_; }
    size_t                   getChunkNumber() const { return chunk_number_; }
    size_t                   getOffset() const { return offset_; }
    const std::vector<char>& getData() const { return data_; }

    std::string         serialize() const;
    static ChunkMessage deserialize(const std::string& serialized);

  private:
    friend class boost::serialization::access;

    template <class Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
        ar & file_id_;
        ar & chunk_number_;
        ar & offset_;
        ar & data_;
    }

    std::string       file_id_;
    size_t            chunk_number_;
    size_t            offset_;
    std::vector<char> data_;
};

#endif // CHUNK_MESSAGE_HPP
