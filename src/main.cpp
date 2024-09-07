#include <chrono>
#include <iostream>
#include <string>
#include <thread>

#include "NetworkManager.hpp"
#include "TextMessage.hpp"
#include "logger.hpp"

void printUsage()
{
    std::cout << "Usage: ./QuickShare <listen_port> [connect_ip connect_port]"
              << std::endl;
}

int main(int argc, char* argv[])
{
    init_logging();
    LOG_INFO << "QuickShare started";

    if (argc != 2 && argc != 4)
    {
        printUsage();
        return 1;
    }

    uint16_t listen_port = std::stoi(argv[1]);

    auto network_manager = NetworkManager::create();

    network_manager->setMessageHandler([](const Message& msg) {
        if (msg.getType() == MessageType::TEXT)
        {
            const auto& text_msg = static_cast<const TextMessage&>(msg);
            std::cout << "Received message: " << text_msg.getText()
                      << std::endl;
            LOG_INFO << "Processed message: " << text_msg.getText();
        } else {
            LOG_WARNING << "Received message of unknown type";
        }
    });

    network_manager->start(listen_port);

    std::string peer_key;
    if (argc == 4)
    {
        std::string connect_ip = argv[2];
        uint16_t    connect_port = std::stoi(argv[3]);
        network_manager->connectToPeer(connect_ip, connect_port);
        peer_key = connect_ip + ":" + std::to_string(connect_port);
    }

    std::cout << "Listening on port " << listen_port << std::endl;
    if (!peer_key.empty())
    {
        std::cout << "Connected to " << peer_key << std::endl;
    }

    std::cout << "Type a message and press Enter to send. Type 'quit' to exit."
              << std::endl;

    std::string input;
    while (true)
    {
        std::getline(std::cin, input);
        if (input == "quit")
        {
            break;
        }
        TextMessage msg(input);
        if (!peer_key.empty())
        {
            network_manager->sendMessage(msg, peer_key);
        } else {
            network_manager->broadcastMessage(msg);
        }
        LOG_INFO << "Sent message: " << input;
    }

    network_manager->stop();
    LOG_INFO << "QuickShare stopped";
    return 0;
}
