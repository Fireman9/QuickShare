#ifndef CLICKABLELABEL_HPP
#define CLICKABLELABEL_HPP

#include <QLabel>
#include <QMouseEvent>

class ClickableLabel : public QLabel
{
    Q_OBJECT

  public:
    explicit ClickableLabel(QWidget*        parent = nullptr,
                            Qt::WindowFlags f = Qt::WindowFlags());
    explicit ClickableLabel(const QString& text, QWidget* parent = nullptr,
                            Qt::WindowFlags f = Qt::WindowFlags());

  signals:
    void clicked();

  protected:
    void mousePressEvent(QMouseEvent* event) override;
};

#endif // CLICKABLELABEL_HPP
