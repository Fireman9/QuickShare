#include <iostream>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/asio.hpp>
#include <boost/serialization/string.hpp>

#include <QApplication>
#include <QLabel>

#include <openssl/crypto.h>
#include <openssl/opensslv.h>

#include "network/NetworkManager.hpp"

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

void check_openssl()
{
    std::cout << "OpenSSL version: " << OpenSSL_version(OPENSSL_VERSION)
              << std::endl;
    std::cout << "OpenSSL library version: " << OpenSSL_version(OPENSSL_CFLAGS)
              << std::endl;
}

struct Person
{
    std::string name;
    int         age;

    template <class Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
        ar & name;
        ar & age;
    }
};

void check_serialization()
{
    std::stringstream ss;
    {
        Person                        p{"John Doe", 30};
        boost::archive::text_oarchive oa(ss);
        oa << p;
    }
    std::cout << "Serialized content:\n" << ss.str() << std::endl;

    Person restored;
    {
        boost::archive::text_iarchive ia(ss);
        ia >> restored;
    }
    std::cout << "Name: " << restored.name << ", Age: " << restored.age
              << std::endl;
}

int main(int argc, char* argv[])
{
    std::cout << "Boost.Asio:" << std::endl;
    check_boost_asio();

    std::cout << "\nQt6:" << std::endl;
    check_qt6(argc, argv);

    std::cout << "\nOpenSSL:" << std::endl;
    check_openssl();

    std::cout << "\nSerialization:" << std::endl;
    check_serialization();

    return 0;
}
