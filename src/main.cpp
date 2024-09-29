#include <chrono>
#include <iostream>
#include <string>
#include <thread>

#include "Logger.hpp"
#include "MainWindow.hpp"
#include "Message/TextMessage.hpp"
#include "NetworkManager.hpp"

int main(int argc, char* argv[])
{
    Logger::instance().setLogFile("QuickShare.log");
    Logger::instance().setConsoleOutput(true);
    Logger::instance().setLogLevel(Logger::LogLevel::Debug);

    LOG_INFO("QuickShare started");

    QFont robotoFont("Roboto", 14);
    QApplication::setFont(robotoFont);

    QApplication a(argc, argv);

    QIcon appIcon(QDir::currentPath() + "/icons/quickshare.png");
    a.setWindowIcon(appIcon);

    MainWindow w;
    w.show();

    return a.exec();
}
