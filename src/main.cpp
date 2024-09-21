#include <chrono>
#include <iostream>
#include <string>
#include <thread>

#include <QApplication>

#include "Message/TextMessage.hpp"
#include "NetworkManager.hpp"
#include "gui/MainWindow.hpp"
#include "logger.hpp"

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    MainWindow   w;
    w.show();
    return a.exec();
}

// void printUsage()
// {
//     std::cout << "Usage: ./QuickShare <listen_port> [connect_ip
//     connect_port]"
//               << std::endl;
//     std::cout << "Commands:" << std::endl;
//     std::cout << "  send <message>      - Send a text message" << std::endl;
//     std::cout << "  sendfile <filepath> - Send a file" << std::endl;
//     std::cout << "  quit                - Exit the program" << std::endl;
// }

// int main(int argc, char* argv[])
// {
//     init_logging();
//     LOG_INFO << "QuickShare started";

//     if (argc != 2 && argc != 4)
//     {
//         printUsage();
//         return 1;
//     }

//     uint16_t listen_port = std::stoi(argv[1]);

//     auto network_manager = NetworkManager::create();

//     network_manager->setMessageHandler([](const Message& msg) {
//         if (msg.getType() == MessageType::TEXT)
//         {
//             const auto& text_msg = static_cast<const TextMessage&>(msg);
//             std::cout << "Received message: " << text_msg.getText()
//                       << std::endl;
//             LOG_INFO << "Processed message: " << text_msg.getText();
//         } else {
//             LOG_WARNING << "Received message of unknown type";
//         }
//     });

//     network_manager->start(listen_port);

//     std::string peer_key;
//     if (argc == 4)
//     {
//         std::string connect_ip = argv[2];
//         uint16_t    connect_port = std::stoi(argv[3]);
//         network_manager->connectToPeer(connect_ip, connect_port);
//         peer_key = connect_ip + ":" + std::to_string(connect_port);
//     }

//     std::cout << "Listening on port " << listen_port << std::endl;
//     if (!peer_key.empty())
//     {
//         std::cout << "Connected to " << peer_key << std::endl;
//     }

//     std::cout << "Type 'send <message>' to send a text message, 'sendfile "
//                  "<filepath>' to send a file, or 'quit' to exit."
//               << std::endl;

//     std::string input;
//     while (true)
//     {
//         std::getline(std::cin, input);
//         std::istringstream iss(input);
//         std::string        command;
//         iss >> command;

//         if (command == "quit")
//         {
//             break;
//         } else if (command == "send") {
//             std::string message;
//             std::getline(iss >> std::ws, message);
//             if (!message.empty())
//             {
//                 TextMessage msg(message);
//                 if (!peer_key.empty())
//                 {
//                     network_manager->sendMessage(msg, peer_key);
//                     LOG_INFO << "Sent message: " << message;
//                 } else {
//                     network_manager->broadcastMessage(msg);
//                     LOG_INFO << "Broadcast message: " << message;
//                 }
//             } else {
//                 std::cout << "Error: Empty message. Usage: send <message>"
//                           << std::endl;
//             }
//         } else if (command == "sendfile") {
//             std::string filepath;
//             iss >> filepath;
//             if (!filepath.empty())
//             {
//                 if (!peer_key.empty())
//                 {
//                     network_manager->startSendingFile(filepath, peer_key);
//                     LOG_INFO << "Started sending file: " << filepath;
//                 } else {
//                     std::cout << "Error: No peer connected. Cannot send
//                     file."
//                               << std::endl;
//                     LOG_WARNING
//                         << "Attempted to send file without connected peer";
//                 }
//             } else {
//                 std::cout
//                     << "Error: No file specified. Usage: sendfile <filepath>"
//                     << std::endl;
//             }
//         } else {
//             std::cout << "Unknown command. ";
//             printUsage();
//         }
//     }

//     network_manager->stop();
//     LOG_INFO << "QuickShare stopped";
//     return 0;
// }
