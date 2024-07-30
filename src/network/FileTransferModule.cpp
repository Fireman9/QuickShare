#include "FileTransferModule.hpp"

FileTransferModule::FileTransferModule(std::size_t chunkSize) :
    m_chunkSize(chunkSize), m_totalSize(0), m_transferredSize(0),
    m_isPaused(false)
{}

bool FileTransferModule::sendFile(
    const std::string&                               filePath,
    std::function<bool(const std::vector<uint8_t>&)> sendDataCallback)
{
    std::ifstream file(filePath, std::ios::binary);
    if (!file)
    {
        std::cerr << "Failed to open file: " << filePath << std::endl;
        return false;
    }

    file.seekg(0, std::ios::end);
    m_totalSize = file.tellg();
    file.seekg(0, std::ios::beg);

    m_transferredSize = 0;

    while (file && !m_isPaused)
    {
        std::vector<uint8_t> chunk = readFileChunk(file);
        if (chunk.empty()) break;

        if (!sendDataCallback(chunk))
        {
            std::cerr << "Failed to send data chunk" << std::endl;
            return false;
        }

        m_transferredSize += chunk.size();
    }

    return m_transferredSize == m_totalSize;
}

bool FileTransferModule::receiveFile(
    const std::string& saveDir, const std::string& fileName,
    std::size_t                           fileSize,
    std::function<std::vector<uint8_t>()> receiveDataCallback)
{
    std::filesystem::path filePath = std::filesystem::path(saveDir) / fileName;
    std::ofstream         file(filePath, std::ios::binary);
    if (!file)
    {
        std::cerr << "Failed to create file: " << filePath << std::endl;
        return false;
    }

    m_totalSize = fileSize;
    m_transferredSize = 0;

    while (m_transferredSize < m_totalSize && !m_isPaused)
    {
        std::vector<uint8_t> chunk = receiveDataCallback();
        if (chunk.empty()) break;

        if (!writeFileChunk(file, chunk))
        {
            std::cerr << "Failed to write data chunk" << std::endl;
            return false;
        }

        m_transferredSize += chunk.size();
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
