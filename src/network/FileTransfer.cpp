#include "FileTransfer.hpp"

FileTransfer::FileTransfer(std::shared_ptr<FileSystemManager> fs_manager) :
    fs_manager_(std::move(fs_manager))
{}

void FileTransfer::startSending(const std::string& file_path,
                                const std::string& peer_id)
{
    if (!fs_manager_->fileExists(file_path))
    {
        LOG_ERROR << "File does not exist: " << file_path;
    }

    LOG_INFO << "file exists";

    std::string    file_id = generateFileId(file_path, peer_id);
    std::uintmax_t file_size = fs_manager_->getFileSize(file_path);
    size_t         total_chunks = (file_size + CHUNK_SIZE - 1) / CHUNK_SIZE;

    LOG_INFO << "fileID: " << file_id;
    LOG_INFO << "file size: " << file_size;
    LOG_INFO << "total chunks: " << total_chunks;

    TransferInfo info{file_path, peer_id, total_chunks, 0, true, false};
    active_transfers_[file_id] = info;

    FileMetadata metadata(file_id, fs_manager_->getFileName(file_path),
                          file_size, fs_manager_->calculateFileHash(file_path));
    if (file_metadata_callback_)
    {
        file_metadata_callback_(metadata);
    }

    LOG_INFO << "calling processNextChunk";
    processNextChunk(file_id);
}

void FileTransfer::startReceiving(const FileMetadata& metadata)
{
    std::filesystem::path current_path = std::filesystem::current_path();
    std::string file_path = (current_path / metadata.getFileName()).string();
    fs_manager_->createFile(file_path, metadata.getFileSize());

    TransferInfo info{file_path,
                      "",
                      (metadata.getFileSize() + CHUNK_SIZE - 1) / CHUNK_SIZE,
                      0,
                      false,
                      false};
    active_transfers_[metadata.getFileId()] = info;
}

void FileTransfer::handleIncomingChunk(const ChunkMessage& chunk_msg)
{
    auto it = active_transfers_.find(chunk_msg.getFileId());
    if (it == active_transfers_.end())
    {
        LOG_ERROR << "No active transfer for file ID: "
                  << chunk_msg.getFileId();
        return;
    }

    TransferInfo& info = it->second;
    if (info.is_sending)
    {
        LOG_ERROR << "Received chunk for a file being sent: "
                  << chunk_msg.getFileId();
        return;
    }

    LOG_INFO << "calling writeChunk, filepath: " << info.file_path
             << ", offset: " << chunk_msg.getOffset();

    fs_manager_->writeChunk(info.file_path, chunk_msg.getOffset(),
                            chunk_msg.getData());
    LOG_INFO << "writeChunk called";
    info.current_chunk++;

    LOG_INFO << "calling checkTransferCompletion";
    checkTransferCompletion(chunk_msg.getFileId());
}

void FileTransfer::pauseTransfer(const std::string& file_id)
{
    auto it = active_transfers_.find(file_id);
    if (it != active_transfers_.end())
    {
        it->second.is_paused = true;
    }
}

void FileTransfer::resumeTransfer(const std::string& file_id)
{
    auto it = active_transfers_.find(file_id);
    if (it != active_transfers_.end())
    {
        it->second.is_paused = false;
        if (it->second.is_sending)
        {
            processNextChunk(file_id);
        }
    }
}

void FileTransfer::cancelTransfer(const std::string& file_id)
{
    auto it = active_transfers_.find(file_id);
    if (it != active_transfers_.end())
    {
        if (!it->second.is_sending)
        {
            fs_manager_->deleteFile(it->second.file_path);
        }
        active_transfers_.erase(it);
    }
}

void FileTransfer::setChunkReadyCallback(ChunkReadyCallback callback)
{
    chunk_ready_callback_ = std::move(callback);
}

void FileTransfer::setFileMetadataCallback(FileMetadataCallback callback)
{
    file_metadata_callback_ = std::move(callback);
}

void FileTransfer::setTransferCompleteCallback(
    TransferCompleteCallback callback)
{
    transfer_complete_callback_ = std::move(callback);
}

void FileTransfer::processNextChunk(const std::string& file_id)
{
    auto it = active_transfers_.find(file_id);
    if (it == active_transfers_.end() || it->second.is_paused)
    {
        LOG_INFO << "ret 1";
        return;
    }

    LOG_INFO << "active_transfers_ found";

    TransferInfo& info = it->second;

    LOG_INFO << "current_chunk: " << info.current_chunk;
    LOG_INFO << "total_chunks: " << info.total_chunks;

    if (info.current_chunk >= info.total_chunks)
    {
        LOG_INFO << "calling checkTransferCompletion";
        checkTransferCompletion(file_id);
        return;
    }

    LOG_INFO << "checkTransferCompletion not called";

    std::streampos offset = info.current_chunk * CHUNK_SIZE;

    LOG_INFO << "offset: " << offset;
    LOG_INFO << "calling fs_manager_->readChunk";

    std::vector<uint8_t> data =
        fs_manager_->readChunk(info.file_path, offset, CHUNK_SIZE);

    LOG_INFO << "chunk read";
    LOG_INFO << "data size: " << data.size();

    ChunkMessage chunk(file_id, info.current_chunk, offset, data);
    LOG_INFO << "chunk created";
    if (chunk_ready_callback_)
    {
        LOG_INFO << "chunk callback calling";
        chunk_ready_callback_(chunk);
    }
    LOG_INFO << "chunk callback called";

    info.current_chunk++;

    LOG_INFO << "current_chunk: " << info.current_chunk;

    if (info.current_chunk < info.total_chunks)
    {
        LOG_INFO << "calling processNextChunk";
        processNextChunk(file_id);
    } else {
        LOG_INFO << "calling checkTransferCompletion";
        checkTransferCompletion(file_id);
    }
}

std::string FileTransfer::generateFileId(const std::string& file_path,
                                         const std::string& peer_id)
{
    return fs_manager_->calculateFileHash(file_path) + "_" + peer_id;
}

void FileTransfer::checkTransferCompletion(const std::string& file_id)
{
    auto it = active_transfers_.find(file_id);
    if (it != active_transfers_.end())
    {
        const TransferInfo& info = it->second;
        if (info.current_chunk >= info.total_chunks)
        {
            if (transfer_complete_callback_)
            {
                bool success = true;
                if (!info.is_sending)
                {
                    std::string calculated_hash =
                        fs_manager_->calculateFileHash(info.file_path);
                    // TODO: compare calculated_hash with the expected hash from
                    // FileMetadata
                    success = true;
                }
                transfer_complete_callback_(file_id, success);
            }
            active_transfers_.erase(it);
        }
    }
}
