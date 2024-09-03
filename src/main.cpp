#include <iostream>
#include <thread>

#include "Peer.hpp"
#include "logger.hpp"

int main(int argc, char* argv[])
{
    init_logging();
    LOG_INFO << "QuickShare started";

    if (argc != 4)
    {
        LOG_ERROR << "Usage: exe <listen_port> <connect_host> <connect_port>";
        return 1;
    }

    io_context io_context;
    uint16_t   listen_port = std::atoi(argv[1]);

    auto peer = Peer::create(io_context, listen_port);

    peer->set_message_handler([](const std::string& message) {
        std::cout << "Received: " << message << std::endl;
    });

    peer->start();
    peer->connect(argv[2], argv[3]);

    std::thread t([&io_context]() { io_context.run(); });

    std::string input;
    while (std::getline(std::cin, input))
    {
        if (input == "quit") break;
        peer->write(input);
    }

    io_context.stop();
    t.join();

    return 0;
}
