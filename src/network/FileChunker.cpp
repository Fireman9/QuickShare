#include "FileChunker.hpp"

FileChunker::FileChunker(std::size_t chunkSize) : m_chunkSize(chunkSize) {}

std::vector<std::vector<uint8_t>>
FileChunker::splitFile(const std::string& filePath)
{
    std::ifstream file(filePath, std::ios::binary);
    if (!file)
    {
        std::cerr << "Failed to open file: " << filePath << std::endl;
        return {};
    }

    std::vector<std::vector<uint8_t>> chunks;
    std::vector<uint8_t>              buffer(m_chunkSize);

    while (file)
    {
        file.read(reinterpret_cast<char*>(buffer.data()), m_chunkSize);
        std::streamsize bytesRead = file.gcount();

        if (bytesRead > 0)
        {
            buffer.resize(bytesRead);
            chunks.push_back(buffer);
            buffer.resize(m_chunkSize);
        }
    }

    return chunks;
}

bool FileChunker::mergeChunks(const std::vector<std::vector<uint8_t>>& chunks,
                              const std::string& outputPath)
{
    std::ofstream outFile(outputPath, std::ios::binary);
    if (!outFile)
    {
        std::cerr << "Failed to create output file: " << outputPath
                  << std::endl;
        return false;
    }

    for (const auto& chunk : chunks)
    {
        outFile.write(reinterpret_cast<const char*>(chunk.data()),
                      chunk.size());
        if (!outFile)
        {
            std::cerr << "Failed to write chunk to file" << std::endl;
            return false;
        }
    }

    return true;
}

std::size_t FileChunker::getChunkSize() const
{
    return m_chunkSize;
}

void FileChunker::setChunkSize(std::size_t chunkSize)
{
    m_chunkSize = chunkSize;
}
