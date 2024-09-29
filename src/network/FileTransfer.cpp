#include "FileTransfer.hpp"

FileTransfer::FileTransfer(std::shared_ptr<FileSystemManager> fs_manager) :
    fs_manager_(std::move(fs_manager))
{}

void FileTransfer::startSending(const std::string& file_path,
                                const std::string& peer_id)
{
    if (!fs_manager_->fileExists(file_path))
    {
        LOG_ERROR(QString("File does not exist: %1").arg(file_path.c_str()));
        return;
    }

    std::string file_id = generateFileId(file_path, peer_id);
    size_t      file_size = fs_manager_->getFileSize(file_path);
    std::string file_hash = fs_manager_->calculateFileHash(file_path);

    TransferInfo info{
        file_path,
        peer_id,
        0,
        file_size,
        true,
        false,
        file_hash,
        std::make_unique<ChunkSizeOptimizer>(generatePossibleChunkSizes())};
    active_transfers_[file_id] = std::move(info);

    FileMetadata metadata(file_id, fs_manager_->getFileName(file_path),
                          file_size, file_hash);
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

    TransferInfo info{
        file_path,
        "",
        0,
        metadata.getFileSize(),
        false,
        false,
        metadata.getFileHash(),
        std::make_unique<ChunkSizeOptimizer>(generatePossibleChunkSizes())};
    active_transfers_[metadata.getFileId()] = std::move(info);
}

void FileTransfer::handleIncomingChunk(const ChunkMessage& chunk_msg)
{
    auto it = active_transfers_.find(chunk_msg.getFileId());
    if (it == active_transfers_.end())
    {
        LOG_ERROR(QString("No active transfer for file ID: %1")
                      .arg(chunk_msg.getFileId().c_str()));
        return;
    }

    TransferInfo& info = it->second;
    if (info.is_sending)
    {
        LOG_ERROR(QString("Received chunk for a file being sent: %1")
                      .arg(chunk_msg.getFileId().c_str()));
        return;
    }

    fs_manager_->writeChunk(info.file_path, chunk_msg.getOffset(),
                            chunk_msg.getData());
    info.current_offset = chunk_msg.getOffset() + chunk_msg.getData().size();

    if (info.current_offset >= info.file_size)
    {
        checkTransferCompletion(chunk_msg.getFileId());
    }
}

void FileTransfer::handleChunkMetrics(const std::string& file_id,
                                      size_t chunk_number, size_t chunk_size,
                                      std::chrono::microseconds latency)
{
    auto it = active_transfers_.find(file_id);
    if (it != active_transfers_.end() && it->second.is_sending)
    {
        it->second.chunk_size_optimizer->recordPerformance(chunk_size, latency);
        processNextChunk(file_id);
    }
}

void FileTransfer::pauseTransfer(const std::string& file_id)
{
    auto it = active_transfers_.find(file_id);
    if (it != active_transfers_.end())
    {
        it->second.is_paused = true;
        LOG_INFO(
            QString("Transfer paused for file ID: %1").arg(file_id.c_str()));
    } else {
        LOG_ERROR(QString("Non-existent pause to transfer for file ID: %1")
                      .arg(file_id.c_str()));
    }
}

void FileTransfer::resumeTransfer(const std::string& file_id)
{
    auto it = active_transfers_.find(file_id);
    if (it != active_transfers_.end())
    {
        it->second.is_paused = false;
        LOG_INFO(
            QString("Transfer resumed for file ID: %1").arg(file_id.c_str()));
        if (it->second.is_sending)
        {
            processNextChunk(file_id);
        }
    } else {
        LOG_ERROR(QString("Non-existent resume to transfer for file ID: %1")
                      .arg(file_id.c_str()));
    }
}

void FileTransfer::cancelTransfer(const std::string& file_id)
{
    auto it = active_transfers_.find(file_id);
    if (it != active_transfers_.end())
    {
        active_transfers_.erase(it);
        if (transfer_complete_callback_)
        {
            transfer_complete_callback_(file_id, false);
        }
    }
}

std::vector<std::string> FileTransfer::getActiveTransfers() const
{
    std::vector<std::string> active_transfers;
    active_transfers.reserve(active_transfers_.size());

    std::transform(active_transfers_.begin(), active_transfers_.end(),
                   std::back_inserter(active_transfers),
                   [](const auto& pair) { return pair.first; });

    return active_transfers;
}

double FileTransfer::getTransferProgress(const std::string& file_id) const
{
    auto it = active_transfers_.find(file_id);
    if (it == active_transfers_.end())
    {
        LOG_WARNING(
            QString("Attempted to get progress for non-existent transfer: %1")
                .arg(file_id.c_str()));
        return 0.0;
    }

    const TransferInfo& info = it->second;
    if (info.file_size == 0)
    {
        LOG_WARNING(
            QString("File size is 0 for transfer: %1").arg(file_id.c_str()));
        return 0.0;
    }

    double progress =
        static_cast<double>(info.current_offset) / info.file_size * 100.0;
    return std::min(progress, 100.0);
}

size_t FileTransfer::getOptimalChunkSize(const std::string& file_id) const
{
    auto it = active_transfers_.find(file_id);
    if (it != active_transfers_.end() && it->second.chunk_size_optimizer)
    {
        return it->second.chunk_size_optimizer->getOptimalChunkSize();
    }
    return MAX_CHUNK_SIZE;
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
    if (info.current_offset >= info.file_size)
    {
        checkTransferCompletion(file_id);
        return;
    }

    size_t optimal_chunk_size =
        info.chunk_size_optimizer->getOptimalChunkSize();
    size_t remaining_size = info.file_size - info.current_offset;
    size_t chunk_size = std::min(optimal_chunk_size, remaining_size);

    std::vector<uint8_t> data =
        fs_manager_->readChunk(info.file_path, info.current_offset, chunk_size);

    ChunkMessage chunk(file_id, info.current_offset, data);
    if (chunk_ready_callback_)
    {
        chunk_ready_callback_(chunk);
    }

    info.current_offset += chunk_size;
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
        if (info.current_offset >= info.file_size)
        {
            bool success = true;
            if (!info.is_sending)
            {
                std::string calculated_hash =
                    fs_manager_->calculateFileHash(info.file_path);
                success = (calculated_hash == info.expected_hash);

                if (!success)
                {
                    LOG_ERROR(QString("Hash mismatch for file %1. Expected: "
                                      "%2, Calculated: %3")
                                  .arg(info.file_path.c_str())
                                  .arg(info.expected_hash.c_str())
                                  .arg(calculated_hash.c_str()));
                } else {
                    LOG_INFO(QString("Hash verification successful for file %1")
                                 .arg(info.file_path.c_str()));
                }
            }

            if (transfer_complete_callback_)
            {
                transfer_complete_callback_(file_id, success);
            }

            if (!success && !info.is_sending)
            {
                fs_manager_->deleteFile(info.file_path);
                LOG_INFO(QString("Deleted file %1 due to hash mismatch")
                             .arg(info.file_path.c_str()));
            }

            active_transfers_.erase(it);
        }
    }
}

std::vector<size_t> FileTransfer::generatePossibleChunkSizes()
{
    std::vector<size_t> sizes;
    for (size_t size = MIN_CHUNK_SIZE; size <= MAX_CHUNK_SIZE; size *= 2)
    {
        sizes.push_back(size);
    }
    return sizes;
}
