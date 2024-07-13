#ifndef __MESSAGE_HPP__
#define __MESSAGE_HPP__

#include <vector>

#include "MessageHeader.hpp"

struct Message
{
    MessageHeader        header;
    std::vector<uint8_t> body;
};

#endif // __MESSAGE_HPP__
