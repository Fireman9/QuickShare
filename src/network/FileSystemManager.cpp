#include "FileSystemManager.hpp"

namespace fs = std::filesystem;

bool FileSystemManager::fileExists(const fs::path& file_path) const
{
    return fs::exists(file_path);
}

std::uintmax_t FileSystemManager::getFileSize(const fs::path& file_path) const
{
    if (!fileExists(file_path))
    {
        LOG_ERROR(QString("File does not exist: %1").arg(file_path.c_str()));
        return 0;
    }
    return fs::file_size(file_path);
}

std::string
FileSystemManager::calculateFileHash(const fs::path& file_path) const
{
    if (!fileExists(file_path))
    {
        LOG_ERROR(QString("File does not exist: %1").arg(file_path.c_str()));
        return "";
    }

    std::ifstream file(file_path, std::ios::binary);
    if (!file)
    {
        LOG_ERROR(QString("Unable to open file: %1").arg(file_path.c_str()));
        return "";
    }

    boost::crc_32_type result;
    char               buffer[4096];
    while (file)
    {
        file.read(buffer, sizeof(buffer));
        result.process_bytes(buffer, file.gcount());
    }

    return std::to_string(result.checksum());
}

std::vector<uint8_t> FileSystemManager::readChunk(const fs::path& file_path,
                                                  std::streampos  offset,
                                                  std::streamsize size) const
{
    std::ifstream file(file_path, std::ios::binary);
    if (!file)
    {
        LOG_ERROR(QString("Unable to open file: %1").arg(file_path.c_str()));
        return {};
    }

    file.seekg(offset);
    std::vector<uint8_t> buffer(size);
    file.read(reinterpret_cast<char*>(buffer.data()), size);

    buffer.resize(file.gcount());
    return buffer;
}

void FileSystemManager::writeChunk(const fs::path&             file_path,
                                   std::streampos              offset,
                                   const std::vector<uint8_t>& data)
{
    std::fstream file(file_path,
                      std::ios::binary | std::ios::in | std::ios::out);
    if (!file)
    {
        LOG_ERROR(QString("Unable to open file: %1").arg(file_path.c_str()));
        return;
    }

    file.seekp(offset);
    file.write(reinterpret_cast<const char*>(data.data()), data.size());

    if (!file)
    {
        LOG_ERROR(QString("Error writing to file: %1").arg(file_path.c_str()));
    }
}

void FileSystemManager::createFile(const fs::path& file_path,
                                   std::uintmax_t  size)
{
    std::ofstream file(file_path, std::ios::binary);
    if (!file)
    {
        LOG_ERROR(QString("Unable to create file: %1").arg(file_path.c_str()));
        return;
    }

    file.seekp(size - 1);
    file.put(0);

    if (!file)
    {
        LOG_ERROR(QString("Error creating file: %1").arg(file_path.c_str()));
    }
}

void FileSystemManager::deleteFile(const fs::path& file_path)
{
    std::error_code ec;
    if (!fs::remove(file_path, ec))
    {
        LOG_ERROR(QString("Unable to delete file: %1. Error: %2")
                      .arg(file_path.c_str())
                      .arg(ec.message().c_str()));
    }
}

std::string FileSystemManager::getFileName(const fs::path& file_path) const
{
    return file_path.filename().string();
}

std::fstream FileSystemManager::openFile(const fs::path&         file_path,
                                         std::ios_base::openmode mode) const
{
    std::fstream file(file_path, mode);
    if (!file)
    {
        LOG_ERROR(QString("Unable to open file: %1").arg(file_path.c_str()));
    }
    return file;
}
