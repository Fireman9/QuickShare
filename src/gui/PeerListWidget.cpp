#include "PeerListWidget.hpp"

PeerListWidget::PeerListWidget(QWidget* parent) :
    QWidget(parent), m_peerList(new QListWidget(this)),
    m_addPeerButton(new QPushButton("Add Peer", this)),
    m_layout(new QVBoxLayout(this))
{
    setupUi();

    connect(m_addPeerButton, &QPushButton::clicked, this,
            &PeerListWidget::onAddPeerClicked);
    connect(m_peerList, &QListWidget::itemClicked, this,
            &PeerListWidget::onPeerItemClicked);
}

void PeerListWidget::setupUi()
{
    m_layout->addWidget(m_peerList);
    m_layout->addWidget(m_addPeerButton);
    setLayout(m_layout);
}

void PeerListWidget::onAddPeerClicked()
{
    bool    ok;
    QString peerKey = QInputDialog::getText(this, tr("Add Peer"),
                                            tr("Enter peer address [IP:Port]:"),
                                            QLineEdit::Normal, QString(), &ok);
    if (ok && !peerKey.isEmpty())
    {
        addPeer(peerKey);
    }
}

void PeerListWidget::onPeerItemClicked(QListWidgetItem* item)
{
    if (item)
    {
        emit peerSelected(item->text());
    }
}

void PeerListWidget::addPeer(const QString& peerKey)
{
    if (!m_peerList->findItems(peerKey, Qt::MatchExactly).isEmpty())
    {
        // TODO: Peer already exists, show a warning
        return;
    }

    m_peerList->addItem(peerKey);
}
