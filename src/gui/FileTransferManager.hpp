#ifndef FILE_TRANSFER_MANAGER_HPP
#define FILE_TRANSFER_MANAGER_HPP

#include <QObject>
#include <QString>

class FileTransferManager : public QObject
{
    Q_OBJECT

  public:
    explicit FileTransferManager(QObject* parent = nullptr);

  public slots:
    void initiateFileTransfer(const QString& filePath, const QString& peerKey);

  signals:
    void fileTransferInitiated(const QString& filePath, const QString& peerKey);
};

#endif // FILE_TRANSFER_MANAGER_HPP
