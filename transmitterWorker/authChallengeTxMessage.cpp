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

AuthChallengeTxMessage::AuthChallengeTxMessage(char *challenge, char *serial)
{
    len = sizeof(authChallengeTxMsg);
    buff = (unsigned char *)&authChallengeTxMsg;

    Encryptor e(challenge, serial);

    authChallengeTxMsg.opcode = opcode_;
    strncpy((char *)authChallengeTxMsg.challengeHash, e.calculateHash().c_str(), sizeof(authChallengeTxMsg.challengeHash));
}

unsigned char AuthChallengeTxMessage::opcode()
{
    return opcode_;
}

