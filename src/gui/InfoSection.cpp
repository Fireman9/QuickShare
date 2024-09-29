#include "InfoSection.hpp"

InfoSection::InfoSection(QWidget* parent) : QWidget(parent)
{
    setupUi();
    setupConnections();
}

void InfoSection::setupUi()
{
    QHBoxLayout* layout = new QHBoxLayout(this);
    m_myPortLabel = new QLabel(this);
    m_settingsButton = new CustomButton(this);

    m_settingsButton->setIcon(QIcon(QDir::currentPath() + "/icons/gear.png"));
    m_settingsButton->setIconSize(QSize(30, 30));
    m_settingsButton->setFixedSize(35, 35);

    layout->addWidget(m_myPortLabel);
    layout->addWidget(m_settingsButton);
    layout->setAlignment(Qt::AlignTop);
}

void InfoSection::setupConnections()
{
    connect(m_settingsButton, &QPushButton::clicked, this,
            &InfoSection::settingsClicked);
}

void InfoSection::setCurrentPort(quint16 port)
{
    m_myPortLabel->setText(QString("My port: %1").arg(port));
}

QPushButton* InfoSection::getSettingsButton() const
{
    return m_settingsButton;
}
