#include "FileTransferManager.hpp"

FileTransferManager::FileTransferManager(QObject* parent) : QObject(parent) {}

void FileTransferManager::initiateFileTransfer(const QString& filePath,
                                               const QString& peerKey)
{
    emit fileTransferInitiated(filePath, peerKey);
}
