#ifndef CHUNK_ACKNOWLEDGEMENT_HPP
#define CHUNK_ACKNOWLEDGEMENT_HPP

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/string.hpp>
#include <chrono>
#include <sstream>
#include <vector>

#include "Message.hpp"

class ChunkMetrics : public Message
{
  public:
    ChunkMetrics() = default;
    ChunkMetrics(const std::string& file_id, size_t chunk_number,
                 size_t                                chunk_size,
                 std::chrono::system_clock::time_point received_time);

    MessageType getType() const override { return MessageType::CHUNK_METRICS; }

    const std::string& getFileId() const { return file_id_; }
    size_t             getChunkNumber() const { return chunk_number_; }
    size_t             getChunkSize() const { return chunk_size_; }
    std::chrono::system_clock::time_point getReceivedTime() const;

    std::vector<uint8_t> serialize() const override;
    static ChunkMetrics  deserialize(const std::vector<uint8_t>& serialized);

  private:
    friend class boost::serialization::access;

    template <class Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
        ar & file_id_;
        ar & chunk_number_;
        ar & chunk_size_;
        ar & received_time_;
    }

    std::string file_id_;
    size_t      chunk_number_;
    size_t      chunk_size_;
    int64_t     received_time_;
};

#endif // CHUNK_ACKNOWLEDGEMENT_HPP
