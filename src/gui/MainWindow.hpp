#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QHBoxLayout>
#include <QMainWindow>

#include "PeerInfoWidget.hpp"
#include "PeerListWidget.hpp"
#include "NetworkManager.hpp"

class MainWindow : public QMainWindow
{
    Q_OBJECT

  public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow() = default;

  private slots:
    void onPeerSelected(const QString& peerKey);

  private:
    void setupUi();

    PeerListWidget*                 m_peerListWidget;
    PeerInfoWidget*                 m_peerInfoWidget;
    std::shared_ptr<NetworkManager> m_networkManager;
};

#endif // MAINWINDOW_HPP
