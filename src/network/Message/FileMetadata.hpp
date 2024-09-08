#ifndef FILE_METADATA_HPP
#define FILE_METADATA_HPP

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/string.hpp>
#include <sstream>

#include "Message/Message.hpp"

class FileMetadata : public Message
{
  public:
    FileMetadata() = default;
    FileMetadata(const std::string& file_id, const std::string& file_name,
                 size_t file_size, const std::string& file_hash) :
        file_id_(file_id),
        file_name_(file_name), file_size_(file_size), file_hash_(file_hash)
    {}

    MessageType getType() const override { return MessageType::FILE_METADATA; }

    const std::string& getFileId() const { return file_id_; }
    const std::string& getFileName() const { return file_name_; }
    size_t             getFileSize() const { return file_size_; }
    const std::string& getFileHash() const { return file_hash_; }

    std::string         serialize() const;
    static FileMetadata deserialize(const std::string& serialized);

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
