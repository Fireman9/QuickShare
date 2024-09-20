#ifndef FILE_SYSTEM_MANAGER_HPP
#define FILE_SYSTEM_MANAGER_HPP

#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#include <boost/crc.hpp>

#include "logger.hpp"

class FileSystemManager
{
  public:
    FileSystemManager() = default;
    ~FileSystemManager() = default;

    FileSystemManager(const FileSystemManager&) = delete;
    FileSystemManager& operator=(const FileSystemManager&) = delete;
    FileSystemManager(FileSystemManager&&) = delete;
    FileSystemManager& operator=(FileSystemManager&&) = delete;

    bool           fileExists(const std::filesystem::path& file_path) const;
    std::uintmax_t getFileSize(const std::filesystem::path& file_path) const;
    std::string calculateFileHash(const std::filesystem::path& file_path) const;

    std::vector<uint8_t> readChunk(const std::filesystem::path& file_path,
                                   std::streampos               offset,
                                   std::streamsize              size) const;
    void                 writeChunk(const std::filesystem::path& file_path,
                                    std::streampos offset, const std::vector<uint8_t>& data);

    void createFile(const std::filesystem::path& file_path,
                    std::uintmax_t               size);
    void deleteFile(const std::filesystem::path& file_path);

    std::string getFileName(const std::filesystem::path& file_path) const;

  private:
    std::fstream openFile(const std::filesystem::path& file_path,
                          std::ios_base::openmode      mode) const;
};

#endif // FILE_SYSTEM_MANAGER_HPP
