#include "Message/FileMetadata.hpp"

std::string FileMetadata::serialize() const
{
    std::ostringstream            oss;
    boost::archive::text_oarchive oa(oss);
    oa << *this;
    return oss.str();
}

FileMetadata FileMetadata::deserialize(const std::string& serialized)
{
    FileMetadata                  metadata;
    std::istringstream            iss(serialized);
    boost::archive::text_iarchive ia(iss);
    ia >> metadata;
    return metadata;
}
