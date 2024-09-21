#include "MainWindow.hpp"

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent), m_peerListWidget(new PeerListWidget(this)),
    m_peerInfoWidget(new PeerInfoWidget(this))
{
    setupUi();

    connect(m_peerListWidget, &PeerListWidget::peerSelected, this,
            &MainWindow::onPeerSelected);
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
