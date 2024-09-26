#include "MainWindow.hpp"

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent), m_peerListWidget(new PeerListWidget(this)),
    m_peerInfoWidget(new PeerInfoWidget(this)),
    m_networkManager(NetworkManager::create()),
    m_settingsButton(new QPushButton("Settings", this)),
    m_settingsWidget(new SettingsWidget(this))
{
    setupUi();

    connect(m_peerListWidget, &PeerListWidget::peerSelected, this,
            &MainWindow::onPeerSelected);

    connect(m_peerListWidget, &PeerListWidget::peerAdded,
            [this](const QString& address, quint16 port) {
                m_networkManager->connectToPeer(address.toStdString(), port);
            });

    connect(m_peerInfoWidget->getFileTransferManager(),
            &FileTransferManager::fileTransferInitiated, m_networkManager.get(),
            &NetworkManager::startSendingFile);

    connect(m_networkManager.get(),
            &NetworkManager::fileTransferProgressUpdated, m_peerInfoWidget,
            &PeerInfoWidget::updateTransferProgress);

    connect(m_settingsButton, &QPushButton::clicked, this,
            &MainWindow::onSettingsClicked);
    connect(m_settingsWidget, &SettingsWidget::applySettings, this,
            &MainWindow::onApplySettings);

    quint16 initialPort = 8080;
    m_networkManager->start(initialPort);
    m_settingsWidget->setCurrentPort(initialPort);
    m_settingsWidget->hide();
}

void MainWindow::setupUi()
{
    QWidget*     centralWidget = new QWidget(this);
    QHBoxLayout* mainLayout = new QHBoxLayout(centralWidget);

    QVBoxLayout* rightLayout = new QVBoxLayout();
    rightLayout->addWidget(m_peerInfoWidget);
    rightLayout->addWidget(m_settingsButton);
    rightLayout->addWidget(m_settingsWidget);

    mainLayout->addWidget(m_peerListWidget);
    mainLayout->addLayout(rightLayout);

    setCentralWidget(centralWidget);
    setWindowTitle(tr("QuickShare"));
    resize(800, 600);
}

void MainWindow::onPeerSelected(const QString& peerKey)
{
    m_peerInfoWidget->updatePeerInfo(peerKey);
}

void MainWindow::onSettingsClicked()
{
    m_settingsWidget->setVisible(!m_settingsWidget->isVisible());
}

void MainWindow::onApplySettings(quint16 port)
{
    if (m_networkManager->changePort(port))
    {
        QMessageBox::information(this, "Success", "Port changed successfully.");
        m_settingsWidget->hide();
    } else {
        QMessageBox::warning(
            this, "Error",
            "Failed to change port. Check if the port is available.");
        m_settingsWidget->setCurrentPort(m_networkManager->getCurrentPort());
    }
}
