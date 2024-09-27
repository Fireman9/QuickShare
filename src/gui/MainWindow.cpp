#include "MainWindow.hpp"
#include <QFileDialog>
#include <QMessageBox>

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent), m_peerListWidget(new PeerListWidget(this)),
    m_networkManager(NetworkManager::create()), m_rightPanel(new QWidget(this)),
    m_rightPanelLayout(new QVBoxLayout(m_rightPanel)),
    m_myInfoLayout(new QHBoxLayout()), m_myPortLabel(new QLabel(this)),
    m_settingsButton(new QPushButton(this)),
    m_peerInfoLayout(new QVBoxLayout()),
    m_selectedPeerLabel(new QLabel("Selected peer", this)),
    m_peerDetailsLayout(new QHBoxLayout()),
    m_peerIpLabel(new QLabel("IP:", this)),
    m_peerPortLabel(new QLabel("port:", this)),
    m_fileSelectionLayout(new QHBoxLayout()),
    m_fileSelectionLabel(new ClickableLabel("Select file", this)),
    m_sendFileButton(new QPushButton(">", this)),
    m_progressBar(new QProgressBar(this)),
    m_settingsWidget(new SettingsWidget(this))
{
    setupUi();

    connect(m_peerListWidget, &PeerListWidget::peerSelected, this,
            &MainWindow::onPeerSelected);
    connect(m_peerListWidget, &PeerListWidget::peerAdded,
            [this](const QString& address, quint16 port) {
                m_networkManager->connectToPeer(address.toStdString(), port);
            });

    connect(m_settingsButton, &QPushButton::clicked, this,
            &MainWindow::onSettingsClicked);
    connect(m_settingsWidget, &SettingsWidget::applySettings, this,
            &MainWindow::onApplySettings);

    connect(m_fileSelectionLabel, &ClickableLabel::clicked, this,
            &MainWindow::onSelectFileClicked);
    connect(m_sendFileButton, &QPushButton::clicked, this,
            &MainWindow::onSendFileClicked);

    connect(m_networkManager.get(),
            &NetworkManager::fileTransferProgressUpdated,
            [this](const QString&, double progress) {
                m_progressBar->setValue(static_cast<int>(progress));
            });

    quint16 initialPort = 8080;
    m_networkManager->start(initialPort);
    m_settingsWidget->setCurrentPort(initialPort);
    m_myPortLabel->setText(QString("My port: %1").arg(initialPort));
    m_settingsWidget->hide();
}

void MainWindow::setupUi()
{
    QWidget*     centralWidget = new QWidget(this);
    QHBoxLayout* mainLayout = new QHBoxLayout(centralWidget);

    // Left panel - Peer list
    mainLayout->addWidget(m_peerListWidget);

    // Right panel
    mainLayout->addWidget(m_rightPanel);

    // My Info section
    m_settingsButton->setIcon(QIcon::fromTheme("configure"));
    m_settingsButton->setFixedSize(24, 24);
    m_myInfoLayout->addWidget(m_myPortLabel);
    m_myInfoLayout->addWidget(m_settingsButton);
    m_rightPanelLayout->addLayout(m_myInfoLayout);

    // Peer Info section
    m_rightPanelLayout->addWidget(m_selectedPeerLabel);
    m_peerDetailsLayout->addWidget(m_peerIpLabel);
    m_peerDetailsLayout->addWidget(m_peerPortLabel);
    m_peerInfoLayout->addLayout(m_peerDetailsLayout);
    m_rightPanelLayout->addLayout(m_peerInfoLayout);

    // File Selection section
    m_fileSelectionLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    m_fileSelectionLabel->setMinimumSize(200, 60);
    m_fileSelectionLabel->setAlignment(Qt::AlignCenter);
    m_fileSelectionLayout->addWidget(m_fileSelectionLabel);
    m_fileSelectionLayout->addWidget(m_sendFileButton);
    m_rightPanelLayout->addLayout(m_fileSelectionLayout);

    // Progress Bar
    m_rightPanelLayout->addWidget(m_progressBar);

    // Settings Widget (hidden by default)
    m_rightPanelLayout->addWidget(m_settingsWidget);

    setCentralWidget(centralWidget);
    setWindowTitle(tr("QuickShare"));
    resize(600, 400);
}

void MainWindow::onPeerSelected(const QString& peerKey)
{
    QStringList parts = peerKey.split(":");
    if (parts.size() == 2)
    {
        m_peerIpLabel->setText(QString("IP: %1").arg(parts[0]));
        m_peerPortLabel->setText(QString("port: %1").arg(parts[1]));
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
        m_myPortLabel->setText(QString("My port: %1").arg(port));
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
    m_selectedFilePath = QFileDialog::getOpenFileName(
        this, "Select File to Transfer", QDir::homePath(), "All Files (*)");
    updateFileInfo();
}

void MainWindow::updateFileInfo()
{
    if (!m_selectedFilePath.isEmpty())
    {
        QFileInfo fileInfo(m_selectedFilePath);
        m_fileSelectionLabel->setText(
            QString("Name: %1\nPath: %2\nSize: %3 bytes")
                .arg(fileInfo.fileName())
                .arg(fileInfo.filePath())
                .arg(fileInfo.size()));
    } else {
        m_fileSelectionLabel->setText("Select file");
    }
}

void MainWindow::onSendFileClicked()
{
    if (m_selectedFilePath.isEmpty())
    {
        QMessageBox::warning(this, "Error", "Please select a file first.");
        return;
    }

    QString peerKey = m_peerIpLabel->text() + ":" + m_peerPortLabel->text();
    if (peerKey == "IP::port:")
    {
        QMessageBox::warning(this, "Error", "Please select a peer first.");
        return;
    }

    m_networkManager->startSendingFile(m_selectedFilePath, peerKey);
    m_progressBar->setValue(0);
}
