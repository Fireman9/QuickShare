#include "MainWindow.hpp"

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
    m_sendFileButton(new QPushButton(this)),
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
            [this](int progress) { m_progressBar->setValue(progress); });

    quint16 initialPort = 8080;
    m_networkManager->start(initialPort);
    m_settingsWidget->setCurrentPort(initialPort);
    m_myPortLabel->setText(QString("My port: %1").arg(initialPort));
    m_settingsWidget->hide();

    m_progressBar->setValue(0);
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
    QString settingsIconPath = QDir::currentPath() + "/icons/gear.png";
    m_settingsButton->setIcon(QIcon(settingsIconPath));
    m_settingsButton->setIconSize(QSize(30, 30));
    m_settingsButton->setFixedSize(35, 35);
    m_settingsButton->setStyleSheet("QPushButton {"
                                    "   background-color: transparent;"
                                    "   border: 1px solid black;"
                                    "   border-radius: 3px;"
                                    "}"
                                    "QPushButton:hover {"
                                    "   border: 1px solid gray;"
                                    "}");
    m_settingsButton->installEventFilter(this);

    m_myInfoLayout->addWidget(m_myPortLabel);
    m_myInfoLayout->addWidget(m_settingsButton);
    m_myInfoLayout->setAlignment(Qt::AlignTop);
    m_rightPanelLayout->addLayout(m_myInfoLayout);

    // Add spacer
    m_rightPanelLayout->addItem(
        new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));

    // Peer Info section
    m_selectedPeerLabel->setAlignment(Qt::AlignLeft); // Left align the text
    QFont peerFont = m_selectedPeerLabel->font();
    peerFont.setPointSize(16);
    m_selectedPeerLabel->setFont(peerFont);
    m_rightPanelLayout->addWidget(m_selectedPeerLabel);

    m_peerDetailsLayout->addWidget(m_peerIpLabel);
    m_peerDetailsLayout->addWidget(m_peerPortLabel);
    m_peerInfoLayout->addLayout(m_peerDetailsLayout);
    m_rightPanelLayout->addLayout(m_peerInfoLayout);

    // Add some space after peer IP and port
    m_rightPanelLayout->addItem(
        new QSpacerItem(20, 10, QSizePolicy::Minimum, QSizePolicy::Fixed));

    // File Selection section
    m_fileSelectionLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    m_fileSelectionLabel->setMinimumSize(200, 80);
    m_fileSelectionLabel->setAlignment(Qt::AlignCenter);

    QString sendIconPath = QDir::currentPath() + "/icons/send.png";
    m_sendFileButton->setIcon(QIcon(sendIconPath));
    m_sendFileButton->setIconSize(QSize(40, 40));
    m_sendFileButton->setFixedSize(50, 50);
    m_sendFileButton->setStyleSheet("QPushButton {"
                                    "   background-color: transparent;"
                                    "   border: 1px solid black;"
                                    "   border-radius: 5px;"
                                    "}"
                                    "QPushButton:hover {"
                                    "   border: 1px solid gray;"
                                    "}");
    m_sendFileButton->installEventFilter(this);

    m_fileSelectionLayout->addWidget(m_fileSelectionLabel);
    m_fileSelectionLayout->addWidget(m_sendFileButton);
    m_rightPanelLayout->addLayout(m_fileSelectionLayout);

    // Progress Bar
    m_rightPanelLayout->addWidget(m_progressBar);
    m_progressBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    // Settings Widget (hidden by default)
    m_rightPanelLayout->addWidget(m_settingsWidget);

    setCentralWidget(centralWidget);
    setWindowTitle(tr("QuickShare"));
    resize(500, 350);
}

void MainWindow::onPeerSelected(const QString& peerKey)
{
    QStringList parts = peerKey.split(":");
    if (parts.size() == 2)
    {
        m_peerIpLabel->setText(QString("IP: %1").arg(parts[0]));
        m_peerPortLabel->setText(QString("port: %1").arg(parts[1]));
    } else {
        m_peerIpLabel->setText("IP:");
        m_peerPortLabel->setText("port:");
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
    QString newFilePath = QFileDialog::getOpenFileName(
        this, "Select File to Transfer", QDir::homePath(), "All Files (*)");

    m_progressBar->setValue(0);

    if (!newFilePath.isEmpty() && newFilePath != m_selectedFilePath)
    {
        m_selectedFilePath = newFilePath;
        updateFileInfo();
    }
}

void MainWindow::updateFileInfo()
{
    if (!m_selectedFilePath.isEmpty())
    {
        QFileInfo fileInfo(m_selectedFilePath);
        QString   dirPath = fileInfo.dir().path();
        QString   formattedSize = formatFileSize(fileInfo.size());
        m_fileSelectionLabel->setText(QString("Name: %1\nPath: %2\nSize: %3")
                                          .arg(fileInfo.fileName())
                                          .arg(dirPath)
                                          .arg(formattedSize));

        QFont fileFont = m_fileSelectionLabel->font();
        fileFont.setPointSize(12);
        m_fileSelectionLabel->setFont(fileFont);
        m_fileSelectionLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    } else {
        m_fileSelectionLabel->setText("Select file");
        m_fileSelectionLabel->setAlignment(Qt::AlignCenter);
        m_fileSelectionLabel->setFont(QApplication::font());
    }
}

void MainWindow::onSendFileClicked()
{
    if (m_selectedFilePath.isEmpty())
    {
        QMessageBox::warning(this, "Error", "Please select a file first.");
        return;
    }

    QString ip = m_peerIpLabel->text().remove("IP: ");
    QString port = m_peerPortLabel->text().remove("port: ");

    if (ip.isEmpty() || port.isEmpty() || ip == "IP:" || port == "port:")
    {
        QMessageBox::warning(this, "Error", "Please select a peer first.");
        return;
    }

    QString peerKey = ip + ":" + port;
    m_networkManager->startSendingFile(m_selectedFilePath, peerKey);
    m_progressBar->setValue(0);
}

QString MainWindow::formatFileSize(qint64 bytes)
{
    const char* units[] = {"b", "Kb", "Mb", "Gb", "Tb"};
    int         unitIndex = 0;
    double      size = static_cast<double>(bytes);

    while (size >= 1024.0 && unitIndex < 4)
    {
        size /= 1024.0;
        unitIndex++;
    }

    return QString("%1 %2").arg(size, 0, 'f', 2).arg(units[unitIndex]);
}

bool MainWindow::eventFilter(QObject* watched, QEvent* event)
{
    QPushButton* button = qobject_cast<QPushButton*>(watched);
    if (button && (button == m_settingsButton || button == m_sendFileButton))
    {
        if (event->type() == QEvent::Enter)
        {
            // Change cursor to pointing hand
            button->setCursor(Qt::PointingHandCursor);
        } else if (event->type() == QEvent::Leave) {
            // Reset cursor
            button->unsetCursor();
        }
    }
    return QMainWindow::eventFilter(watched, event);
}
