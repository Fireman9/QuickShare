#ifndef CUSTOM_BUTTON_HPP
#define CUSTOM_BUTTON_HPP

#include <QApplication>
#include <QCursor>
#include <QEvent>
#include <QIcon>
#include <QPushButton>

class CustomButton : public QPushButton
{
    Q_OBJECT

  public:
    explicit CustomButton(QWidget* parent = nullptr);
    explicit CustomButton(const QString& text, QWidget* parent = nullptr);
    explicit CustomButton(const QIcon& icon, const QString& text,
                          QWidget* parent = nullptr);

  protected:
    bool event(QEvent* e) override;

  private:
    void init();
};

#endif // CUSTOM_BUTTON_HPP
