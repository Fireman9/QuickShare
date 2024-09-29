#include "MainWindow.hpp"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent)
{

    m_networkManager = NetworkManager::create();

    setupUi();
    setupConnections();

    quint16 initialPort = 8080;
    m_networkManager->start(initialPort);
    m_infoSection->setCurrentPort(initialPort);
    m_settingsWidget->setCurrentPort(initialPort);
    m_settingsWidget->hide();

    m_progressBar->setValue(0);
}

void MainWindow::setupUi()
{
    QWidget*     centralWidget = new QWidget(this);
    QHBoxLayout* mainLayout = new QHBoxLayout(centralWidget);

    m_peerListWidget = new PeerListWidget(this);
    mainLayout->addWidget(m_peerListWidget);

    m_rightPanel = new QWidget(this);
    m_rightPanelLayout = new QVBoxLayout(m_rightPanel);
    mainLayout->addWidget(m_rightPanel);

    m_infoSection = new InfoSection(this);
    m_rightPanelLayout->addWidget(m_infoSection);

    m_rightPanelLayout->addItem(
        new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));

    m_peerInfoSection = new PeerInfoSection(this);
    m_rightPanelLayout->addWidget(m_peerInfoSection);

    m_rightPanelLayout->addItem(
        new QSpacerItem(20, 10, QSizePolicy::Minimum, QSizePolicy::Fixed));

    m_fileSelectionSection = new FileSelectionSection(this);
    m_rightPanelLayout->addWidget(m_fileSelectionSection);

    m_progressBar = new QProgressBar(this);
    m_rightPanelLayout->addWidget(m_progressBar);

    m_settingsWidget = new SettingsWidget(this);
    m_rightPanelLayout->addWidget(m_settingsWidget);

    setCentralWidget(centralWidget);
    setWindowTitle(tr("QuickShare"));
    resize(500, 350);
}

void MainWindow::setupConnections()
{
    connect(m_peerListWidget, &PeerListWidget::peerSelected, this,
            &MainWindow::onPeerSelected);
    connect(m_peerListWidget, &PeerListWidget::peerAdded,
            [this](const QString& address, quint16 port) {
                m_networkManager->connectToPeer(address.toStdString(), port);
            });

    connect(m_infoSection, &InfoSection::settingsClicked, this,
            &MainWindow::onSettingsClicked);
    connect(m_settingsWidget, &SettingsWidget::applySettings, this,
            &MainWindow::onApplySettings);

    connect(m_fileSelectionSection, &FileSelectionSection::fileSelected, this,
            &MainWindow::onSelectFileClicked);
    connect(m_fileSelectionSection, &FileSelectionSection::sendClicked, this,
            &MainWindow::onSendFileClicked);

    connect(m_networkManager.get(),
            &NetworkManager::fileTransferProgressUpdated,
            [this](int progress) { m_progressBar->setValue(progress); });
}

void MainWindow::onPeerSelected(const QString& peerKey)
{
    QStringList parts = peerKey.split(":");
    if (parts.size() == 2)
    {
        m_peerInfoSection->updatePeerInfo(parts[0], parts[1]);
    } else {
        m_peerInfoSection->updatePeerInfo("", "");
    }
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
        m_infoSection->setCurrentPort(port);
        m_settingsWidget->hide();
    } else {
        QMessageBox::warning(
            this, "Error",
            "Failed to change port. Check if the port is available.");
        m_settingsWidget->setCurrentPort(m_networkManager->getCurrentPort());
    }
}

void MainWindow::onSelectFileClicked()
{
    QString newFilePath = QFileDialog::getOpenFileName(
        this, "Select File to Transfer", QDir::homePath(), "All Files (*)");

    m_progressBar->setValue(0);

    if (!newFilePath.isEmpty() && newFilePath != m_selectedFilePath)
    {
        m_selectedFilePath = newFilePath;
        updateFileInfo();
    }
}

void MainWindow::onSendFileClicked()
{
    if (m_selectedFilePath.isEmpty())
    {
        QMessageBox::warning(this, "Error", "Please select a file first.");
        return;
    }

    QString ip = m_peerInfoSection->getPeerIp();
    QString port = m_peerInfoSection->getPeerPort();

    if (ip.isEmpty() || port.isEmpty() || ip == "IP:" || port == "port:")
    {
        QMessageBox::warning(this, "Error", "Please select a peer first.");
        return;
    }

    QString peerKey = ip + ":" + port;
    m_networkManager->startSendingFile(m_selectedFilePath, peerKey);
    m_progressBar->setValue(0);
}

void MainWindow::updateFileInfo()
{
    if (!m_selectedFilePath.isEmpty())
    {
        QFileInfo fileInfo(m_selectedFilePath);
        QString   dirPath = fileInfo.dir().path();
        qint64    fileSize = fileInfo.size();
        m_fileSelectionSection->updateFileInfo(fileInfo.fileName(), dirPath,
                                               fileSize);
    } else {
        m_fileSelectionSection->updateFileInfo("", "", 0);
    }
}
