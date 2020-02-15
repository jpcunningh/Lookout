#include <tinyb.hpp>

#include <algorithm>
#include <vector>
#include <string.h>
#include <exception>

#include "authStatusRxMessage.hpp"

#include "transmitterWorker.hpp"

using namespace TransmitterWorker;

unsigned char AuthStatusRxMessage::opcode_ = 0x5;

AuthStatusRxMessage::AuthStatusRxMessage(std::vector<unsigned char> &msg)
{
    len = sizeof(AuthStatusRxMsg);

    if (msg.size() != len)
    {
        throw AuthStatusRxException("incorrect AuthStatusRxMessage length");
    }

    AuthStatusRxMsg *msg_ = (AuthStatusRxMsg *)msg.data();

    if (msg_->opcode != opcode_) {
        throw AuthStatusRxException("incorrect AuthStatusRxMessage opcode");
    }

    authenticated = msg_->authenticated != 0;
    bonded = msg_->bonded != 0;
}

unsigned char AuthStatusRxMessage::opcode() {
    return opcode_;
}

