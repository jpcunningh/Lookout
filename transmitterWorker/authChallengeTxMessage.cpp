#include <tinyb.hpp>

#include <algorithm>
#include <vector>
#include <iostream>
#include <string.h>

#include "authChallengeTxMessage.hpp"
#include "encryptor.hpp"

#include "transmitterWorker.hpp"

using namespace TransmitterWorker;

AuthChallengeTxMessage::AuthChallengeTxMessage(char *challenge, char *serial)
{
    opcode = 0x4;
    len = sizeof(authChallengeTxMsg);

    Encryptor e(challenge, serial);

    authChallengeTxMsg.opcode = opcode;
    strncpy((char *)authChallengeTxMsg.challengeHash, e.calculateHash().c_str(), sizeof(authChallengeTxMsg.challengeHash));

    buff = (unsigned char*)&authChallengeTxMsg;
}

