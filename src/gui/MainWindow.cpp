#include "MainWindow.hpp"

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent), m_peerListWidget(new PeerListWidget(this)),
    m_peerInfoWidget(new PeerInfoWidget(this)),
    m_networkManager(NetworkManager::create())
{
    setupUi();

    connect(m_peerListWidget, &PeerListWidget::peerSelected, this,
            &MainWindow::onPeerSelected);

    // Connect FileTransferManager signals to NetworkManager slots
    connect(m_peerInfoWidget->getFileTransferManager(),
            &FileTransferManager::fileTransferInitiated, m_networkManager.get(),
            &NetworkManager::startSendingFile);

    // Connect NetworkManager signals to PeerInfoWidget slots
    connect(m_networkManager.get(),
            &NetworkManager::fileTransferProgressUpdated, m_peerInfoWidget,
            &PeerInfoWidget::updateTransferProgress);

    m_networkManager->start(8080);
}

void MainWindow::setupUi()
{
    QWidget*     centralWidget = new QWidget(this);
    QHBoxLayout* mainLayout = new QHBoxLayout(centralWidget);

    mainLayout->addWidget(m_peerListWidget);
    mainLayout->addWidget(m_peerInfoWidget);

    setCentralWidget(centralWidget);
    setWindowTitle(tr("QuickShare"));
    resize(800, 600);
}

void MainWindow::onPeerSelected(const QString& peerKey)
{
    m_peerInfoWidget->updatePeerInfo(peerKey);
}
