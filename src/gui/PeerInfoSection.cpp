#include "PeerInfoSection.hpp"

PeerInfoSection::PeerInfoSection(QWidget* parent) : QWidget(parent)
{
    setupUi();
}

void PeerInfoSection::setupUi()
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    m_selectedPeerLabel = new QLabel("Selected peer", this);
    m_peerIpLabel = new QLabel("IP:", this);
    m_peerPortLabel = new QLabel("port:", this);

    QFont peerFont = m_selectedPeerLabel->font();
    peerFont.setPointSize(16);
    m_selectedPeerLabel->setFont(peerFont);
    m_selectedPeerLabel->setAlignment(Qt::AlignLeft);

    QHBoxLayout* detailsLayout = new QHBoxLayout();
    detailsLayout->addWidget(m_peerIpLabel);
    detailsLayout->addWidget(m_peerPortLabel);

    layout->addWidget(m_selectedPeerLabel);
    layout->addLayout(detailsLayout);
}

void PeerInfoSection::updatePeerInfo(const QString& ip, const QString& port)
{
    m_peerIpLabel->setText(QString("IP: %1").arg(ip));
    m_peerPortLabel->setText(QString("port: %1").arg(port));
}

QString PeerInfoSection::getPeerIp() const
{
    return m_peerIpLabel->text().remove("IP: ");
}

QString PeerInfoSection::getPeerPort() const
{
    return m_peerPortLabel->text().remove("port: ");
}
