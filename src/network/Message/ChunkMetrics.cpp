#include "ChunkMetrics.hpp"

ChunkMetrics::ChunkMetrics(
    const std::string& file_id, size_t offset, size_t chunk_size,
    std::chrono::system_clock::time_point received_time) :
    file_id_(file_id),
    offset_(offset), chunk_size_(chunk_size)
{
    received_time_ = std::chrono::duration_cast<std::chrono::microseconds>(
                         received_time.time_since_epoch())
                         .count();
}

std::chrono::system_clock::time_point ChunkMetrics::getReceivedTime() const
{
    return std::chrono::system_clock::time_point(
        std::chrono::microseconds(received_time_));
}

std::vector<uint8_t> ChunkMetrics::serialize() const
{
    std::ostringstream              oss;
    boost::archive::binary_oarchive oa(oss, boost::archive::no_header);
    oa << *this;
    const std::string& str = oss.str();
    return std::vector<uint8_t>(str.begin(), str.end());
}

ChunkMetrics ChunkMetrics::deserialize(const std::vector<uint8_t>& serialized)
{
    ChunkMetrics                    metrics;
    std::string                     str(serialized.begin(), serialized.end());
    std::istringstream              iss(str);
    boost::archive::binary_iarchive ia(iss, boost::archive::no_header);
    ia >> metrics;
    return metrics;
}
