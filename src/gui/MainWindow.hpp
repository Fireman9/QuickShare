#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QHBoxLayout>
#include <QMainWindow>

#include "PeerInfoWidget.hpp"
#include "PeerListWidget.hpp"

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

    PeerListWidget* m_peerListWidget;
    PeerInfoWidget* m_peerInfoWidget;
};

#endif // MAINWINDOW_HPP
