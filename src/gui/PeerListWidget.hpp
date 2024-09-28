#ifndef PEER_LIST_WIDGET_HPP
#define PEER_LIST_WIDGET_HPP

#include <QApplication>
#include <QFont>
#include <QInputDialog>
#include <QListWidget>
#include <QMessageBox>
#include <QPushButton>
#include <QStringList>
#include <QVBoxLayout>
#include <QWidget>

class PeerListWidget : public QWidget
{
    Q_OBJECT

  public:
    explicit PeerListWidget(QWidget* parent = nullptr);

  signals:
    void peerSelected(const QString& peerKey);
    void peerAdded(const QString& address, quint16 port);
    // TODO: process peerUpdated in network manager
    void peerUpdated(const QString& oldPeerKey, const QString& newPeerKey);

  private slots:
    void onAddPeerClicked();
    void onPeerItemClicked(QListWidgetItem* item);
    void onPeerItemDoubleClicked(QListWidgetItem* item);

  private:
    QListWidget* m_peerList;
    QPushButton* m_addPeerButton;
    QVBoxLayout* m_layout;

    void setupUi();
    void addPeer(const QString& peerKey);
    void updateWidth();
};

#endif // PEER_LIST_WIDGET_HPP
