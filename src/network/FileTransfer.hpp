#ifndef FILE_TRANSFER_HPP
#define FILE_TRANSFER_HPP

#include <filesystem>
#include <functional>
#include <memory>
#include <queue>
#include <string>
#include <unordered_map>

#include "FileSystemManager.hpp"
#include "Message/ChunkMessage.hpp"
#include "Message/FileMetadata.hpp"

class FileTransfer
{
  public:
    static constexpr size_t CHUNK_SIZE = 1024 * 1024; // 1 MB chunks

    explicit FileTransfer(std::shared_ptr<FileSystemManager> fs_manager);

    void startSending(const std::string& file_path, const std::string& peer_id);
    void startReceiving(const FileMetadata& metadata);
    void handleIncomingChunk(const ChunkMessage& chunk_msg);
    void pauseTransfer(const std::string& file_id);
    void resumeTransfer(const std::string& file_id);
    void cancelTransfer(const std::string& file_id);

    using ChunkReadyCallback = std::function<void(const ChunkMessage&)>;
    void setChunkReadyCallback(ChunkReadyCallback callback);

    using FileMetadataCallback = std::function<void(const FileMetadata&)>;
    void setFileMetadataCallback(FileMetadataCallback callback);

    using TransferCompleteCallback =
        std::function<void(const std::string& file_id, bool success)>;
    void setTransferCompleteCallback(TransferCompleteCallback callback);

  private:
    struct TransferInfo
    {
        std::string file_path;
        std::string peer_id;
        size_t      total_chunks;
        size_t      current_chunk;
        bool        is_sending;
        bool        is_paused;
    };

    std::shared_ptr<FileSystemManager>            fs_manager_;
    std::unordered_map<std::string, TransferInfo> active_transfers_;
    std::queue<std::string>                       transfer_queue_;
    ChunkReadyCallback                            chunk_ready_callback_;
    FileMetadataCallback                          file_metadata_callback_;
    TransferCompleteCallback                      transfer_complete_callback_;

    void        processNextChunk(const std::string& file_id);
    std::string generateFileId(const std::string& file_path,
                               const std::string& peer_id);
    void        checkTransferCompletion(const std::string& file_id);
};

#endif // FILE_TRANSFER_HPP
