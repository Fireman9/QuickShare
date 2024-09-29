#include "CustomButton.hpp"

CustomButton::CustomButton(QWidget* parent) : QPushButton(parent)
{
    init();
}

CustomButton::CustomButton(const QString& text, QWidget* parent) :
    QPushButton(text, parent)
{
    init();
}

CustomButton::CustomButton(const QIcon& icon, const QString& text,
                           QWidget* parent) :
    QPushButton(icon, text, parent)
{
    init();
}

void CustomButton::init()
{
    setStyleSheet("CustomButton {"
                  "   background-color: transparent;"
                  "   border: 1px solid black;"
                  "   border-radius: 5px;"
                  "   padding: 5px;"
                  "}"
                  "CustomButton:hover {"
                  "   border: 1px solid gray;"
                  "}");
}

bool CustomButton::event(QEvent* e)
{
    if (e->type() == QEvent::Enter)
    {
        QApplication::setOverrideCursor(Qt::PointingHandCursor);
    } else if (e->type() == QEvent::Leave) {
        QApplication::restoreOverrideCursor();
    }
    return QPushButton::event(e);
}