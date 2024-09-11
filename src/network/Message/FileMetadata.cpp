#include "FileMetadata.hpp"

FileMetadata::FileMetadata(const std::string& file_id,
                           const std::string& file_name, size_t file_size,
                           const std::string& file_hash) :
    file_id_(file_id),
    file_name_(file_name), file_size_(file_size), file_hash_(file_hash)
{}

std::vector<uint8_t> FileMetadata::serialize() const
{
    std::ostringstream              oss;
    boost::archive::binary_oarchive oa(oss, boost::archive::no_header);
    oa << *this;
    const std::string& str = oss.str();
    return std::vector<uint8_t>(str.begin(), str.end());
}

FileMetadata FileMetadata::deserialize(const std::vector<uint8_t>& serialized)
{
    FileMetadata                    metadata;
    std::string                     str(serialized.begin(), serialized.end());
    std::istringstream              iss(str);
    boost::archive::binary_iarchive ia(iss, boost::archive::no_header);
    ia >> metadata;
    return metadata;
}
