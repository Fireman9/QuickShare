#ifndef FILE_TRANSFER_MODULE_HPP
#define FILE_TRANSFER_MODULE_HPP

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

class FileTransferModule
{
  public:
    FileTransferModule(std::size_t chunkSize = 1024 * 1024); // 1MB default

    // TODO: uint8_t filesystem usage
    bool sendFile(const std::string& filePath,
                  std::function<bool(const std::vector<uint8_t>&)> sendDataCallback);
    bool receiveFile(const std::string& saveDir, const std::string& fileName,
                     std::size_t fileSize,
                     std::function<std::vector<uint8_t>()> receiveDataCallback);

    void  pauseTransfer();
    void  resumeTransfer();
    float getProgress() const;

  private:
    std::vector<uint8_t> readFileChunk(std::ifstream& file);
    bool writeFileChunk(std::ofstream& file, const std::vector<uint8_t>& chunk);

    std::size_t m_chunkSize;
    std::size_t m_totalSize;
    std::size_t m_transferredSize;
    bool        m_isPaused;
};

#endif // FILE_TRANSFER_MODULE_HPP
