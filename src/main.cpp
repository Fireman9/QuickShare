#include "logger.hpp"

int main(int argc, char* argv[])
{
    init_logging();
    LOG_INFO << "QuickShare started";

    return 0;
}
