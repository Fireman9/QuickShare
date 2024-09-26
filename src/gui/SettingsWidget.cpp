#include "SettingsWidget.hpp"

SettingsWidget::SettingsWidget(QWidget* parent) :
    QWidget(parent), m_portInput(new QLineEdit(this)),
    m_applyButton(new QPushButton("Apply", this))
{
    setupUi();
    connect(m_applyButton, &QPushButton::clicked, this,
            &SettingsWidget::onApplyClicked);
}

void SettingsWidget::setupUi()
{
    QVBoxLayout* layout = new QVBoxLayout(this);

    layout->addWidget(new QLabel("Port:"));
    layout->addWidget(m_portInput);
    layout->addWidget(m_applyButton);

    setLayout(layout);
}

void SettingsWidget::onApplyClicked()
{
    bool    ok;
    quint16 port = m_portInput->text().toUShort(&ok);
    if (ok && port > 0)
    {
        emit applySettings(port);
    } else {
        QMessageBox::warning(this, "Invalid Input",
                             "Please enter a valid port number.");
    }
}

void SettingsWidget::setCurrentPort(quint16 port)
{
    m_portInput->setText(QString::number(port));
}
