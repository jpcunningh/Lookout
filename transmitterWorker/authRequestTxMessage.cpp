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

    singleUseToken.clear();
    singleUseToken.insert(singleUseToken.end(), id.begin(), id.begin() + sizeof(authRequestTxMessage.singleUseToken));

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

