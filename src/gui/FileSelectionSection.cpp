#include "FileSelectionSection.hpp"

FileSelectionSection::FileSelectionSection(QWidget* parent) : QWidget(parent)
{
    setupUi();
    setupConnections();
}

void FileSelectionSection::setupUi()
{
    QHBoxLayout* layout = new QHBoxLayout(this);
    m_fileSelectionLabel = new ClickableLabel("Select file", this);
    m_sendFileButton = new CustomButton(this);

    m_fileSelectionLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    m_fileSelectionLabel->setMinimumSize(200, 80);
    m_fileSelectionLabel->setAlignment(Qt::AlignCenter);

    m_sendFileButton->setIcon(QIcon(QDir::currentPath() + "/icons/send.png"));
    m_sendFileButton->setIconSize(QSize(40, 40));
    m_sendFileButton->setFixedSize(50, 50);

    layout->addWidget(m_fileSelectionLabel);
    layout->addWidget(m_sendFileButton);
}

void FileSelectionSection::setupConnections()
{
    connect(m_fileSelectionLabel, &ClickableLabel::clicked, this,
            &FileSelectionSection::fileSelected);
    connect(m_sendFileButton, &QPushButton::clicked, this,
            &FileSelectionSection::sendClicked);
}

void FileSelectionSection::updateFileInfo(const QString& fileName,
                                          const QString& filePath,
                                          qint64         fileSize)
{
    QString formattedSize = formatFileSize(fileSize);
    m_fileSelectionLabel->setText(QString("Name: %1\nPath: %2\nSize: %3")
                                      .arg(fileName)
                                      .arg(filePath)
                                      .arg(formattedSize));

    QFont fileFont = m_fileSelectionLabel->font();
    fileFont.setPointSize(12);
    m_fileSelectionLabel->setFont(fileFont);
    m_fileSelectionLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
}

QPushButton* FileSelectionSection::getSendButton() const
{
    return m_sendFileButton;
}

QString FileSelectionSection::formatFileSize(qint64 bytes)
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
