#include <iostream>

#include <boost/asio.hpp>

#include <QApplication>
#include <QLabel>

// #include "message.pb.h"
// #include <openssl/sha.h>

void check_boost_asio()
{
    boost::asio::io_context   io_context;
    boost::asio::steady_timer timer(io_context,
                                    boost::asio::chrono::seconds(1));
    timer.async_wait([](const boost::system::error_code& error) {
        std::cout << "Boost.Asio timer expired!" << std::endl;
    });
    io_context.run();
}

void check_qt6(int argc, char* argv[])
{
    QApplication app(argc, argv);
    QLabel       label("Qt6 works!");
    label.show();
    app.exec();
}

// void check_openssl()
// {
//     unsigned char hash[SHA256_DIGEST_LENGTH];
//     const char*   data = "OpenSSL test";
//     SHA256_CTX    sha256;
//     SHA256_Init(&sha256);
//     SHA256_Update(&sha256, data, strlen(data));
//     SHA256_Final(hash, &sha256);

//     std::cout << "OpenSSL SHA256: ";
//     for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
//     {
//         printf("%02x", hash[i]);
//     }
//     std::cout << std::endl;
// }

// void check_protobuf()
// {
//     GOOGLE_PROTOBUF_VERIFY_VERSION;
//     tutorial::Person person;
//     person.set_name("John Doe");
//     person.set_id(1234);
//     person.set_email("johndoe@example.com");
//     std::cout << "Protocol Buffers: " << person.DebugString() << std::endl;
// }

int main(int argc, char* argv[])
{
    std::cout << "Boost.Asio:" << std::endl;
    check_boost_asio();

    std::cout << "\nQt6:" << std::endl;
    check_qt6(argc, argv);

    // std::cout << "\nOpenSSL:" << std::endl;
    // check_openssl();

    // std::cout << "\nProtoBuf:" << std::endl;
    // check_protobuf();

    return 0;
}
