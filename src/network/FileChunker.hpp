#ifndef FILE_CHUNKER_HPP
#define FILE_CHUNKER_HPP

#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

class FileChunker
{
  public:
    FileChunker(std::size_t chunkSize = 1024 * 1024); // 1MB default

    std::vector<std::vector<uint8_t>> splitFile(const std::string& filePath);
    bool mergeChunks(const std::vector<std::vector<uint8_t>>& chunks,
                     const std::string&                       outputPath);

    std::size_t getChunkSize() const;
    void        setChunkSize(std::size_t chunkSize);

  private:
    std::size_t m_chunkSize;
};

#endif // FILE_CHUNKER_HPP
