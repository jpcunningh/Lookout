#include <tinyb.hpp>

#include <algorithm>
#include <vector>
#include <iostream>
#include <string.h>

#include "authRequestTxMessage.hpp"
#include "uuidv4.hpp"

#include "transmitterWorker.hpp"

using namespace TransmitterWorker;

unsigned char AuthRequestTxMessage::opcode_ = 0x1;

AuthRequestTxMessage::AuthRequestTxMessage(bool altBtChannel)
{
    len = sizeof(authRequestTxMessage);
    authRequestTxMessage.opcode = opcode_;
    uuidv4 uuid;
    std::vector<unsigned char> id = uuid.generate();

    singleUseToken.insert(singleUseToken.begin(), id.begin(), id.begin() + sizeof(authRequestTxMessage.singleUseToken));

    for (unsigned int i = 0; i < sizeof(authRequestTxMessage.singleUseToken); ++i) {
      authRequestTxMessage.singleUseToken[i] = id[i];
    }

    authRequestTxMessage.btChannel = (!altBtChannel) ? 0x2 : 0x1;

    auto ptr = reinterpret_cast<unsigned char *>(&authRequestTxMessage);
    buff = std::vector<unsigned char>(ptr, ptr + len);
}

unsigned char AuthRequestTxMessage::opcode()
{
    return opcode_;
}

unsigned int AuthRequestTxMessage::random_char()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 255);
    return dis(gen);
}

std::string AuthRequestTxMessage::generate_hex(const unsigned int len)
{
    std::stringstream ss;
    for (unsigned int i = 0; i < len; i++) {
        const auto rc = random_char();
        std::stringstream hexstream;
        hexstream << std::uppercase << std::hex << rc;
        auto hex = hexstream.str();
        ss << (hex.length() < 2 ? '0' + hex : hex);
    }
    return ss.str();
}

std::string AuthRequestTxMessage::generate_uuidv4()
{
    std::string s1 = generate_hex(8);
    std::string s2 = generate_hex(4);
    std::string s3 = generate_hex(4);
    std::string s4 = generate_hex(4);
    std::string s5 = generate_hex(12);

    unsigned int c7 = random_char();
    unsigned int c9 = random_char();

    // Set the most significant 4 bits to 0x4
    // clear what's there, then or in the 4.
    c7 = (c7 & 0x0F) | 0x40;
    std::stringstream hexstream;
    hexstream << std::uppercase << std::hex << c7;
    auto hex = hexstream.str();
    std::string c7_s = (hex.length() < 2 ? '0' + hex : hex);

    // Set the most significant 2 bits to 0x2
    // clear what's there, then or in the 8.
    c9 = (c9 & 0x3F) | 0x80;
    hexstream.str(std::string());
    hexstream.clear();
    hexstream << std::uppercase << std::hex << c9;
    hex = hexstream.str();
    std::string c9_s = (hex.length() < 2 ? '0' + hex : hex);

    s2.replace(0, 2, c7_s);

    s3.replace(0, 2, c9_s);

    return s1 + "-" + s2 + "-" + s3 + "-" + s4 + "-" + s5;
}
