#ifndef FILE_SYSTEM_MANAGER_HPP
#define FILE_SYSTEM_MANAGER_HPP

#include <algorithm>
#include <boost/crc.hpp>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "logger.hpp"

class FileSystemManager
{
  public:
    FileSystemManager() = default;
    ~FileSystemManager() = default;

    bool           fileExists(const std::string& file_path) const;
    std::uintmax_t getFileSize(const std::string& file_path) const;
    std::string    calculateFileHash(const std::string& file_path) const;

    std::vector<uint8_t> readChunk(const std::string& file_path,
                                   std::streampos     offset,
                                   std::streamsize    size) const;
    void writeChunk(const std::string& file_path, std::streampos offset,
                    const std::vector<uint8_t>& data);

    void createFile(const std::string& file_path, std::uintmax_t size);
    void deleteFile(const std::string& file_path);

    std::string getFileName(const std::string& file_path) const;

  private:
    std::fstream openFile(const std::string&      file_path,
                          std::ios_base::openmode mode) const;
};

#endif // FILE_SYSTEM_MANAGER_HPP
