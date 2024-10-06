#include "SettingsWidget.hpp"

SettingsWidget::SettingsWidget(QWidget* parent) :
    QWidget(parent), m_portInput(new QLineEdit(this)),
    m_directoryInput(new QLineEdit(this)),
    m_chooseDirectoryButton(new QPushButton("Choose Directory", this)),
    m_applyButton(new QPushButton("Apply", this))
{
    setupUi();
    setupConnections();
}

void SettingsWidget::setupUi()
{
    QVBoxLayout* layout = new QVBoxLayout(this);

    layout->addWidget(new QLabel("Port:"));
    layout->addWidget(m_portInput);
    layout->addWidget(new QLabel("Download Directory:"));
    layout->addWidget(m_directoryInput);
    layout->addWidget(m_chooseDirectoryButton);
    layout->addWidget(m_applyButton);

    setLayout(layout);
}

void SettingsWidget::setupConnections()
{
    connect(m_applyButton, &QPushButton::clicked, this,
            &SettingsWidget::onApplyClicked);
    connect(m_portInput, &QLineEdit::returnPressed, this,
            &SettingsWidget::onPortInputReturnPressed);
    connect(m_chooseDirectoryButton, &QPushButton::clicked, this,
            &SettingsWidget::onChooseDirectoryClicked);
}

void SettingsWidget::onApplyClicked()
{
    tryApplySettings();
}

void SettingsWidget::onPortInputReturnPressed()
{
    tryApplySettings();
}

void SettingsWidget::onChooseDirectoryClicked()
{
    QString dir = QFileDialog::getExistingDirectory(
        this, tr("Choose Download Directory"), m_directoryInput->text(),
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!dir.isEmpty())
    {
        m_directoryInput->setText(dir);
    }
}

void SettingsWidget::tryApplySettings()
{
    bool    ok;
    quint16 port = m_portInput->text().toUShort(&ok);
    if (ok && port > 0 && !m_directoryInput->text().isEmpty())
    {
        emit applySettings(port, m_directoryInput->text());
    } else {
        QMessageBox::warning(this, "Invalid Input",
                             "Please enter a valid port number and directory.");
    }
}

void SettingsWidget::setCurrentPort(quint16 port)
{
    m_portInput->setText(QString::number(port));
}

void SettingsWidget::setCurrentDirectory(const QString& directory)
{
    m_directoryInput->setText(directory);
}
