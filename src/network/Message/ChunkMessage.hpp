#ifndef CHUNK_MESSAGE_HPP
#define CHUNK_MESSAGE_HPP

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
#include <sstream>
#include <vector>

#include "Message.hpp"

class ChunkMessage : public Message
{
  public:
    ChunkMessage() = default;
    ChunkMessage(const std::string& file_id, size_t offset,
                 const std::vector<uint8_t>& data);

    MessageType getType() const override { return MessageType::CHUNK; }

    const std::string&          getFileId() const { return file_id_; }
    size_t                      getOffset() const { return offset_; }
    const std::vector<uint8_t>& getData() const { return data_; }

    std::vector<uint8_t> serialize() const override;
    static ChunkMessage  deserialize(const std::vector<uint8_t>& serialized);

  private:
    friend class boost::serialization::access;

    template <class Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
        ar & file_id_;
        ar & offset_;
        ar & data_;
    }

    std::string          file_id_;
    size_t               offset_;
    std::vector<uint8_t> data_;
};

#endif // CHUNK_MESSAGE_HPP
