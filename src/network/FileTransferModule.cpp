#include "FileTransferModule.hpp"

FileTransferModule::FileTransferModule(std::size_t chunkSize) :
    m_chunkSize(chunkSize), m_totalSize(0), m_transferredSize(0),
    m_isPaused(false)
{}

// TODO: formatting
bool FileTransferModule::sendFile(
    const std::string&                               filePath,
    std::function<bool(const std::vector<uint8_t>&)> sendDataCallback)
{
    auto chunks = m_fileChunker.splitFile(filePath);
    if (chunks.empty())
    {
        std::cerr << "Failed to split file into chunks: " << filePath
                  << std::endl;
        return false;
    }

    m_totalSize = std::filesystem::file_size(filePath);
    m_transferredSize = 0;

    // Send file size
    auto sizeData =
        std::bit_cast<std::array<uint8_t, sizeof(m_totalSize)>>(m_totalSize);
    if (!sendDataCallback(
            std::vector<uint8_t>(sizeData.begin(), sizeData.end())))
    {
        std::cerr << "Failed to send file size" << std::endl;
        return false;
    }

    // Send number of chunks
    uint32_t numChunks = static_cast<uint32_t>(chunks.size());
    auto     numChunksData =
        std::bit_cast<std::array<uint8_t, sizeof(numChunks)>>(numChunks);
    if (!sendDataCallback(
            std::vector<uint8_t>(numChunksData.begin(), numChunksData.end())))
    {
        std::cerr << "Failed to send number of chunks" << std::endl;
        return false;
    }

    for (const auto& chunk : chunks)
    {
        if (m_isPaused)
        {
            std::cout << "Transfer paused" << std::endl;
            return false;
        }

        if (!sendDataCallback(chunk))
        {
            std::cerr << "Failed to send chunk" << std::endl;
            return false;
        }

        m_transferredSize += chunk.size();
    }

    return m_transferredSize == m_totalSize;
}

bool FileTransferModule::receiveFile(
    const std::string& saveDir, const std::string& fileName,
    std::function<std::vector<uint8_t>()> receiveDataCallback)
{
    // Receive file size
    auto sizeData = receiveDataCallback();
    if (sizeData.size() != sizeof(m_totalSize))
    {
        std::cerr << "Failed to receive file size" << std::endl;
        return false;
    }
    std::array<uint8_t, sizeof(m_totalSize)> sizeArray;
    std::copy_n(sizeData.begin(), sizeof(m_totalSize), sizeArray.begin());
    m_totalSize = std::bit_cast<std::size_t>(sizeArray);

    // Receive number of chunks
    auto numChunksData = receiveDataCallback();
    if (numChunksData.size() != sizeof(uint32_t))
    {
        std::cerr << "Failed to receive number of chunks" << std::endl;
        return false;
    }
    std::array<uint8_t, sizeof(uint32_t)> numChunksArray;
    std::copy_n(numChunksData.begin(), sizeof(uint32_t),
                numChunksArray.begin());
    uint32_t numChunks = std::bit_cast<uint32_t>(numChunksArray);

    m_transferredSize = 0;
    std::vector<std::vector<uint8_t>> chunks;

    for (uint32_t i = 0; i < numChunks; ++i)
    {
        if (m_isPaused)
        {
            std::cout << "Transfer paused" << std::endl;
            return false;
        }

        auto chunk = receiveDataCallback();
        if (chunk.empty())
        {
            std::cerr << "Failed to receive chunk " << i << std::endl;
            return false;
        }

        chunks.push_back(chunk);
        m_transferredSize += chunk.size();
    }

    std::filesystem::path filePath = std::filesystem::path(saveDir) / fileName;
    if (!m_fileChunker.mergeChunks(chunks, filePath.string()))
    {
        std::cerr << "Failed to merge chunks" << std::endl;
        return false;
    }

    return m_transferredSize == m_totalSize;
}

void FileTransferModule::pauseTransfer()
{
    m_isPaused = true;
}

void FileTransferModule::resumeTransfer()
{
    m_isPaused = false;
}

float FileTransferModule::getProgress() const
{
    return m_totalSize > 0 ?
               static_cast<float>(m_transferredSize) / m_totalSize :
               0.0f;
}

std::vector<uint8_t> FileTransferModule::readFileChunk(std::ifstream& file)
{
    std::vector<uint8_t> chunk(m_chunkSize);
    file.read(reinterpret_cast<char*>(chunk.data()), m_chunkSize);
    chunk.resize(file.gcount());
    return chunk;
}

bool FileTransferModule::writeFileChunk(std::ofstream&              file,
                                        const std::vector<uint8_t>& chunk)
{
    file.write(reinterpret_cast<const char*>(chunk.data()), chunk.size());
    return file.good();
}
