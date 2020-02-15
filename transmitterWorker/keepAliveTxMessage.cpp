#include <tinyb.hpp>

#include <algorithm>
#include <vector>
#include <iostream>
#include <string.h>

#include "keepAliveTxMessage.hpp"

#include "transmitterWorker.hpp"

using namespace TransmitterWorker;

unsigned char KeepAliveTxMessage::opcode_ = 0x6;

KeepAliveTxMessage::KeepAliveTxMessage(unsigned char time)
{
    len = sizeof(keepAliveTxMessage);
    keepAliveTxMessage.opcode = opcode_;
    keepAliveTxMessage.time = time;

    auto ptr = reinterpret_cast<unsigned char *>(&keepAliveTxMessage);
    buff = std::vector<unsigned char>(ptr, ptr + len);
}

unsigned char KeepAliveTxMessage::opcode()
{
    return opcode_;
}

