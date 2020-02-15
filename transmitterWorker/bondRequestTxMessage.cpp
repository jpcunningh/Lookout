#include <tinyb.hpp>

#include <algorithm>
#include <vector>
#include <iostream>
#include <string.h>

#include "bondRequestTxMessage.hpp"

#include "transmitterWorker.hpp"

using namespace TransmitterWorker;

unsigned char BondRequestTxMessage::opcode_ = 0x6;

BondRequestTxMessage::BondRequestTxMessage()
{
    len = sizeof(bondRequestTxMessage);
    bondRequestTxMessage.opcode = opcode_;

    auto ptr = reinterpret_cast<unsigned char *>(&bondRequestTxMessage);
    buff = std::vector<unsigned char>(ptr, ptr + len);
}

unsigned char BondRequestTxMessage::opcode()
{
    return opcode_;
}

