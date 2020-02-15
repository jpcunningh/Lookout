#include <tinyb.hpp>

#include <algorithm>
#include <vector>
#include <iostream>
#include <string.h>

#include "uuidv4.hpp"

#include "transmitterWorker.hpp"

using namespace TransmitterWorker;

uuidv4::uuidv4()
{
}

unsigned int uuidv4::random_char()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 255);
    return dis(gen);
}

std::string uuidv4::generate_hex(std::vector<unsigned char> chars)
{
    std::stringstream ss;
    for (auto x: chars) {
        std::stringstream hexstream;
        hexstream << std::uppercase << std::hex << x;
        auto hex = hexstream.str();
        ss << (hex.length() < 2 ? '0' + hex : hex);
    }
    return ss.str();
}

std::vector<unsigned char> uuidv4::generate()
{
    uuid.clear();

    for (auto i = 0; i < 32; i++)
    {
       auto rc = random_char();

       if (i == 6) {
           rc = (rc & 0x0F) | 0x40;
       } else if (i == 8) {
           rc = (rc & 0x3F) | 0x80;
       }
       uuid.push_back(rc);
    }

    return uuid;
}

std::string uuidv4::get_hex()
{
    std::string s = generate_hex(uuid);

    s.insert(8, "-");
    s.insert(13, "-");
    s.insert(18, "-");
    s.insert(27, "-");

    return s;
}
