#ifndef PEER_INFO_SECTION_HPP
#define PEER_INFO_SECTION_HPP

#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>

class PeerInfoSection : public QWidget
{
    Q_OBJECT

  public:
    explicit PeerInfoSection(QWidget* parent = nullptr);

    void    updatePeerInfo(const QString& ip, const QString& port);
    QString getPeerIp() const;
    QString getPeerPort() const;

  private:
    void setupUi();

    QLabel* m_selectedPeerLabel;
    QLabel* m_peerIpLabel;
    QLabel* m_peerPortLabel;
};

#endif // PEER_INFO_SECTION_HPP
