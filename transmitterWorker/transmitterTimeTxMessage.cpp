#include <tinyb.hpp>

#include <algorithm>
#include <vector>
#include <iostream>
#include <string.h>

#include "transmitterTimeTxMessage.hpp"
#include "crc.hpp"

#include "transmitterWorker.hpp"

using namespace TransmitterWorker;

unsigned char TransmitterTimeTxMessage::opcode_ = 0x24;

TransmitterTimeTxMessage::TransmitterTimeTxMessage()
{
    len = sizeof(transmitterTimeTxMessage);
    transmitterTimeTxMessage.msg.opcode = opcode_;

    auto ptr = reinterpret_cast<unsigned char *>(&transmitterTimeTxMessage.msg);
    std::vector<unsigned char>crcBuff = std::vector<unsigned char>(ptr, ptr + sizeof(transmitterTimeTxMessage.msg));

    CRC crc(crcBuff);

    unsigned int crc16 = crc.crc16(0);
    transmitterTimeTxMessage.crcMSB = crc16 >> 8;
    transmitterTimeTxMessage.crcLSB = crc16 & 0xFF;

    ptr = reinterpret_cast<unsigned char *>(&transmitterTimeTxMessage);
    buff = std::vector<unsigned char>(ptr, ptr + len);
}

unsigned char TransmitterTimeTxMessage::opcode()
{
    return opcode_;
}

