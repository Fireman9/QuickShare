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
    connect(m_peerList, &QListWidget::itemDoubleClicked, this,
            &PeerListWidget::onPeerItemDoubleClicked);

    m_peerList->setFont(QApplication::font());
    updateWidth();
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
        QStringList parts = peerKey.split(":");
        if (parts.size() == 2)
        {
            QString address = parts[0];
            quint16 port = parts[1].toUShort();
            emit    peerAdded(address, port);
        } else {
            QMessageBox::warning(
                this, "Invalid Input",
                "Please enter a valid peer address in the format IP:Port.");
        }
    }
}

void PeerListWidget::onPeerItemClicked(QListWidgetItem* item)
{
    if (item)
    {
        emit peerSelected(item->text());
    }
}

void PeerListWidget::onPeerItemDoubleClicked(QListWidgetItem* item)
{
    if (item)
    {
        bool    ok;
        QString oldPeerKey = item->text();
        QString newPeerKey = QInputDialog::getText(
            this, tr("Edit Peer"), tr("Enter new peer address [IP:Port]:"),
            QLineEdit::Normal, oldPeerKey, &ok);
        if (ok && !newPeerKey.isEmpty())
        {
            item->setText(newPeerKey);
            updateWidth();
            emit peerUpdated(oldPeerKey, newPeerKey);
        } else {
            QMessageBox::warning(
                this, "Invalid Input",
                "Please enter a valid peer address in the format IP:Port.");
        }
    }
}

void PeerListWidget::addPeer(const QString& peerKey)
{
    if (m_peerList->findItems(peerKey, Qt::MatchExactly).isEmpty())
    {
        m_peerList->addItem(peerKey);
        updateWidth();
    }
}

void PeerListWidget::removePeer(const QString& peerKey)
{
    QList<QListWidgetItem*> items =
        m_peerList->findItems(peerKey, Qt::MatchExactly);
    for (QListWidgetItem* item : items)
    {
        delete m_peerList->takeItem(m_peerList->row(item));
    }
    updateWidth();
}

void PeerListWidget::updateWidth()
{
    QFontMetrics fm(font());
    int          maxWidth = fm.horizontalAdvance(m_addPeerButton->text());

    for (int i = 0; i < m_peerList->count(); ++i)
    {
        int itemWidth = fm.horizontalAdvance(m_peerList->item(i)->text());
        maxWidth = qMax(maxWidth, itemWidth);
    }

    // padding
    maxWidth += 40;

    setMinimumWidth(maxWidth);
    setMaximumWidth(maxWidth);
}
