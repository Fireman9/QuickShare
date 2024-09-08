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

    std::string    file_id = generateFileId(file_path, peer_id);
    std::uintmax_t file_size = fs_manager_->getFileSize(file_path);
    size_t         total_chunks = (file_size + CHUNK_SIZE - 1) / CHUNK_SIZE;

    TransferInfo info{file_path, peer_id, total_chunks, 0, true, false};
    active_transfers_[file_id] = info;

    FileMetadata metadata(file_id, fs_manager_->getFileName(file_path),
                          file_size, fs_manager_->calculateFileHash(file_path));
    if (file_metadata_callback_)
    {
        file_metadata_callback_(metadata);
    }

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

    fs_manager_->writeChunk(info.file_path, chunk_msg.getOffset(),
                            chunk_msg.getData());
    info.current_chunk++;

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
        return;
    }

    TransferInfo& info = it->second;
    if (info.current_chunk >= info.total_chunks)
    {
        checkTransferCompletion(file_id);
        return;
    }

    std::streampos       offset = info.current_chunk * CHUNK_SIZE;
    std::vector<uint8_t> data =
        fs_manager_->readChunk(info.file_path, offset, CHUNK_SIZE);

    ChunkMessage chunk(file_id, info.current_chunk, offset, data);
    if (chunk_ready_callback_)
    {
        chunk_ready_callback_(chunk);
    }

    info.current_chunk++;

    if (info.current_chunk < info.total_chunks)
    {
        processNextChunk(file_id);
    } else {
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
        TransferInfo& info = it->second;
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
