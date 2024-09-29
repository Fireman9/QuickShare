#ifndef INFO_SECTION_HPP
#define INFO_SECTION_HPP

#include <QDir>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QWidget>

#include "CustomButton.hpp"

class InfoSection : public QWidget
{
    Q_OBJECT

  public:
    explicit InfoSection(QWidget* parent = nullptr);

    void setCurrentPort(quint16 port);

    QPushButton* getSettingsButton() const;

  signals:
    void settingsClicked();

  private:
    void setupUi();
    void setupConnections();

    QLabel*       m_myPortLabel;
    CustomButton* m_settingsButton;
};

#endif // INFO_SECTION_HPP
