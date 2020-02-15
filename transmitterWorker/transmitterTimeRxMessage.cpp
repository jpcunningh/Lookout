#include <tinyb.hpp>

#include <algorithm>
#include <vector>
#include <string.h>
#include <exception>

#include "transmitterTimeRxMessage.hpp"
#include "crc.hpp"

#include "transmitterWorker.hpp"

using namespace TransmitterWorker;

unsigned char TransmitterTimeRxMessage::opcode_ = 0x25;

TransmitterTimeRxMessage::TransmitterTimeRxMessage(std::vector<unsigned char> &msg)
{
    len = sizeof(TransmitterTimeRxMsg);

    if (msg.size() != len)
    {
        throw TransmitterTimeRxException("incorrect TransmitterTimeRxMessage length");
    }

    TransmitterTimeRxMsg *msg_ = (TransmitterTimeRxMsg *)msg.data();

    if (msg_->msg.opcode != opcode_) {
        throw TransmitterTimeRxException("incorrect TransmitterTimeRxMessage opcode");
    }

    auto ptr = reinterpret_cast<unsigned char *>(&msg_->msg);
    std::vector<unsigned char>crcBuff = std::vector<unsigned char>(ptr, ptr + sizeof(msg_->msg));

    CRC crc(crcBuff);

    unsigned int crc16 = crc.crc16(0);

    if (msg_->crc != crc16) {
        throw TransmitterTimeRxException("TransmitterTimeRxMessage incorrect crc");
    }

    status = msg_->msg.status;
    currentTime =
        msg_->msg.currentTime[0] |
        msg_->msg.currentTime[1] << 8 |
        msg_->msg.currentTime[2] << 16 |
        msg_->msg.currentTime[3] << 24;

    sessionStartTime =
        msg_->msg.sessionStartTime[0] |
        msg_->msg.sessionStartTime[1] << 8 |
        msg_->msg.sessionStartTime[2] << 16 |
        msg_->msg.sessionStartTime[3] << 24;
}

unsigned char TransmitterTimeRxMessage::opcode() {
    return opcode_;
}

