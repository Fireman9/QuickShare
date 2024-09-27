#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QApplication>
#include <QCursor>
#include <QDir>
#include <QFileDialog>
#include <QFont>
#include <QFontDatabase>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QMainWindow>
#include <QMessageBox>
#include <QProgressBar>
#include <QPushButton>
#include <QSpacerItem>
#include <QVBoxLayout>

#include "ClickableLabel.hpp"
#include "NetworkManager.hpp"
#include "PeerListWidget.hpp"
#include "SettingsWidget.hpp"

class MainWindow : public QMainWindow
{
    Q_OBJECT

  public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow() = default;

  private slots:
    void onPeerSelected(const QString& peerKey);
    void onSettingsClicked();
    void onApplySettings(quint16 port);
    void onSelectFileClicked();
    void onSendFileClicked();

  private:
    void    setupUi();
    void    updateFileInfo();
    QString formatFileSize(qint64 bytes);

    PeerListWidget*                 m_peerListWidget;
    std::shared_ptr<NetworkManager> m_networkManager;

    QWidget*     m_rightPanel;
    QVBoxLayout* m_rightPanelLayout;

    // My Info section
    QHBoxLayout* m_myInfoLayout;
    QLabel*      m_myPortLabel;
    QPushButton* m_settingsButton;

    // Peer Info section
    QVBoxLayout* m_peerInfoLayout;
    QLabel*      m_selectedPeerLabel;
    QHBoxLayout* m_peerDetailsLayout;
    QLabel*      m_peerIpLabel;
    QLabel*      m_peerPortLabel;

    // File Selection section
    QHBoxLayout*    m_fileSelectionLayout;
    ClickableLabel* m_fileSelectionLabel;
    QPushButton*    m_sendFileButton;

    QProgressBar* m_progressBar;

    SettingsWidget* m_settingsWidget;

    QString m_selectedFilePath;

  protected:
    bool eventFilter(QObject* watched, QEvent* event) override;
};

#endif // MAINWINDOW_HPP
