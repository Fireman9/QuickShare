#ifndef SETTINGS_WIDGET_HPP
#define SETTINGS_WIDGET_HPP

#include <QFileDialog>
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

    void setCurrentPort(quint16 port);
    void setCurrentDirectory(const QString& directory);

  signals:
    void applySettings(quint16 port, const QString& directory);

  private slots:
    void onApplyClicked();
    void onPortInputReturnPressed();
    void onChooseDirectoryClicked();

  private:
    QLineEdit*   m_portInput;
    QLineEdit*   m_directoryInput;
    QPushButton* m_chooseDirectoryButton;
    QPushButton* m_applyButton;

    void setupUi();
    void setupConnections();
    void tryApplySettings();
};

#endif // SETTINGS_WIDGET_HPP
