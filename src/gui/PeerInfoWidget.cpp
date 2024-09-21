#include "PeerInfoWidget.hpp"

PeerInfoWidget::PeerInfoWidget(QWidget* parent) :
    QWidget(parent), m_peerInfoLabel(new QLabel("No peer selected", this)),
    m_selectedFileLabel(new QLabel("No file selected", this)),
    m_selectFileButton(new QPushButton("Select File", this)),
    m_transferProgressBar(new QProgressBar(this)),
    m_layout(new QVBoxLayout(this))
{
    setupUi();

    connect(m_selectFileButton, &QPushButton::clicked, this,
            &PeerInfoWidget::onSelectFileClicked);
}

void PeerInfoWidget::setupUi()
{
    m_layout->addWidget(m_peerInfoLabel);
    m_layout->addWidget(m_selectedFileLabel);
    m_layout->addWidget(m_selectFileButton);
    m_layout->addWidget(m_transferProgressBar);
    setLayout(m_layout);

    m_transferProgressBar->setRange(0, 100);
    m_transferProgressBar->setValue(0);
}

void PeerInfoWidget::updatePeerInfo(const QString& peerKey)
{
    m_currentPeerKey = peerKey;
    m_peerInfoLabel->setText("Selected peer: " + peerKey);
    m_selectedFileLabel->setText("No file selected");
    m_transferProgressBar->setValue(0);
}

void PeerInfoWidget::onSelectFileClicked()
{
    if (m_currentPeerKey.isEmpty())
    {
        QMessageBox::warning(this, "Warning", "Please select a peer first.");
        return;
    }

    QString initialDir = QDir::currentPath();
    QString filePath = QFileDialog::getOpenFileName(
        this, "Select File to Transfer", initialDir, "All Files (*)");
    if (!filePath.isEmpty())
    {
        m_selectedFileLabel->setText("Selected file: " +
                                     QFileInfo(filePath).fileName());
        initiateFileTransfer(filePath);
    }
}

void PeerInfoWidget::updateTransferProgress(int progress)
{
    m_transferProgressBar->setValue(progress);
}

void PeerInfoWidget::initiateFileTransfer(const QString& filePath)
{
    // TODO: Implement file transfer initiation
    // This method should interact with NetworkManager class to start the actual
    // file transfer process. For now, just show a message box.
    QMessageBox::information(
        this, "File Transfer",
        "Initiating transfer of file: " + QFileInfo(filePath).fileName() +
            "\nTo peer: " + m_currentPeerKey);
}
