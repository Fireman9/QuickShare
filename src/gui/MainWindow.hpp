#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QFileDialog>
#include <QFontDatabase>
#include <QMainWindow>
#include <QProgressBar>
#include <QSpacerItem>

#include "FileSelectionSection.hpp"
#include "InfoSection.hpp"
#include "NetworkManager.hpp"
#include "PeerInfoSection.hpp"
#include "PeerListWidget.hpp"
#include "SettingsWidget.hpp"

class MainWindow : public QMainWindow
{
    Q_OBJECT

  public:
    explicit MainWindow(QWidget* parent = nullptr);

  private slots:
    void onPeerSelected(const QString& peerKey);
    void onPeerConnectionResult(const QString& peerKey, bool success);
    void onSettingsClicked();
    void onApplySettings(quint16 port);
    void onSelectFileClicked();
    void onSendFileClicked();

    void onFileReceiveStarted(const QString& fileName, const QString& filePath,
                              qint64 fileSize);
    void onFileReceiveProgressUpdated(int progress);
    void onFileSendStarted(const QString& fileName, const QString& filePath,
                           qint64 fileSize);
    void onFileSendProgressUpdated(int progress);

  private:
    void setupUi();
    void setupConnections();

    void updateFileInfo();

    PeerListWidget* m_peerListWidget;
    QWidget*        m_rightPanel;
    QVBoxLayout*    m_rightPanelLayout;

    InfoSection*          m_infoSection;
    PeerInfoSection*      m_peerInfoSection;
    FileSelectionSection* m_fileSelectionSection;

    QProgressBar*   m_progressBar;
    SettingsWidget* m_settingsWidget;

    std::shared_ptr<NetworkManager> m_networkManager;
    QString                         m_selectedFilePath;
};

#endif // MAINWINDOW_HPP
