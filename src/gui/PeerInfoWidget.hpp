#ifndef PEER_INFO_WIDGET_HPP
#define PEER_INFO_WIDGET_HPP

#include <QFileDialog>
#include <QLabel>
#include <QMessageBox>
#include <QProgressBar>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

#include "FileTransferManager.hpp"

class PeerInfoWidget : public QWidget
{
    Q_OBJECT

  public:
    explicit PeerInfoWidget(QWidget* parent = nullptr);

    FileTransferManager* getFileTransferManager() const;

  public slots:
    void updatePeerInfo(const QString& peerKey);
    void updateTransferProgress(const QString& fileId, double progress);

  private slots:
    void onSelectFileClicked();

  private:
    QLabel*       m_peerInfoLabel;
    QLabel*       m_selectedFileLabel;
    QPushButton*  m_selectFileButton;
    QProgressBar* m_transferProgressBar;
    QVBoxLayout*  m_layout;

    QString m_currentPeerKey;
    QString m_currentFileId;

    FileTransferManager* m_fileTransferManager;

    void setupUi();
    void initiateFileTransfer(const QString& filePath);
};

#endif // PEER_INFO_WIDGET_HPP
