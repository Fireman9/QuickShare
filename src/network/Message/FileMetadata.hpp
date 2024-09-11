#ifndef FILE_METADATA_HPP
#define FILE_METADATA_HPP

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/string.hpp>
#include <sstream>
#include <vector>

#include "Message/Message.hpp"

class FileMetadata : public Message
{
  public:
    FileMetadata() = default;
    FileMetadata(const std::string& file_id, const std::string& file_name,
                 size_t file_size, const std::string& file_hash);

    MessageType getType() const override { return MessageType::FILE_METADATA; }

    const std::string& getFileId() const { return file_id_; }
    const std::string& getFileName() const { return file_name_; }
    size_t             getFileSize() const { return file_size_; }
    const std::string& getFileHash() const { return file_hash_; }

    std::vector<uint8_t> serialize() const;
    static FileMetadata  deserialize(const std::vector<uint8_t>& serialized);

  private:
    friend class boost::serialization::access;

    template <class Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
        ar & file_id_;
        ar & file_name_;
        ar & file_size_;
        ar & file_hash_;
    }

    std::string file_id_;
    std::string file_name_;
    size_t      file_size_;
    std::string file_hash_;
};

#endif // FILE_METADATA_HPP
