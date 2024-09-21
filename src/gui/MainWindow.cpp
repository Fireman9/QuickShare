#include "MainWindow.hpp"

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent), m_button(new QPushButton("Click me!", this))
{
    QWidget*     centralWidget = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(centralWidget);

    layout->addWidget(m_button);

    setCentralWidget(centralWidget);

    connect(m_button, &QPushButton::clicked, this,
            &MainWindow::onButtonClicked);

    setWindowTitle("QuickShare");
    resize(300, 200);
}

void MainWindow::onButtonClicked()
{
    QMessageBox::information(this, "Hello", "Qt is working!");
}
