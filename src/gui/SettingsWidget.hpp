#ifndef SETTINGS_WIDGET_HPP
#define SETTINGS_WIDGET_HPP

#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

class SettingsWidget : public QWidget
{
    Q_OBJECT

  public:
    explicit SettingsWidget(QWidget* parent = nullptr);

  signals:
    void applySettings(quint16 port);

  private slots:
    void onApplyClicked();

  private:
    QLineEdit*   m_portInput;
    QPushButton* m_applyButton;

    void setupUi();
};

#endif // SETTINGS_WIDGET_HPP
