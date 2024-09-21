#ifndef PEER_LIST_WIDGET_HPP
#define PEER_LIST_WIDGET_HPP

#include <QInputDialog>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

class PeerListWidget : public QWidget
{
    Q_OBJECT

  public:
    explicit PeerListWidget(QWidget* parent = nullptr);

  signals:
    void peerSelected(const QString& peerKey);

  private slots:
    void onAddPeerClicked();
    void onPeerItemClicked(QListWidgetItem* item);

  private:
    QListWidget* m_peerList;
    QPushButton* m_addPeerButton;
    QVBoxLayout* m_layout;

    void setupUi();
    void addPeer(const QString& peerKey);
};

#endif // PEER_LIST_WIDGET_HPP
