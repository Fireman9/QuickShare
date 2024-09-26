#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QHBoxLayout>
#include <QMainWindow>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>

#include "NetworkManager.hpp"
#include "PeerInfoWidget.hpp"
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

  private:
    void setupUi();

    PeerListWidget*                 m_peerListWidget;
    PeerInfoWidget*                 m_peerInfoWidget;
    std::shared_ptr<NetworkManager> m_networkManager;

    QPushButton*    m_settingsButton;
    SettingsWidget* m_settingsWidget;
};

#endif // MAINWINDOW_HPP
