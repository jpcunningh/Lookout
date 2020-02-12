#include <tinyb.hpp>

#include <algorithm>
#include <vector>
#include <iostream>
#include <string.h>

#include "authChallengeTxMessage.hpp"
#include "encryptor.hpp"

#include "transmitterWorker.hpp"

using namespace TransmitterWorker;

unsigned char AuthChallengeTxMessage::opcode_ = 0x4;

AuthChallengeTxMessage::AuthChallengeTxMessage(std::string challenge, std::string serial)
{
    len = sizeof(authChallengeTxMsg);

    auto ptr = reinterpret_cast<unsigned char *>(&authChallengeTxMsg);
    buff = std::vector<unsigned char>(ptr, ptr + len);

    Encryptor e(challenge, serial);

    authChallengeTxMsg.opcode = opcode_;
    strncpy((char *)authChallengeTxMsg.challengeHash, e.calculateHash().c_str(), sizeof(authChallengeTxMsg.challengeHash));
}

unsigned char AuthChallengeTxMessage::opcode()
{
    return opcode_;
}

