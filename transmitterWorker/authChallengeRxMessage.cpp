#include <tinyb.hpp>

#include <algorithm>
#include <vector>
#include <iostream>
#include <string.h>
#include <exception>

#include "authChallengeRxMessage.hpp"
#include "encryptor.hpp"

#include "transmitterWorker.hpp"

using namespace TransmitterWorker;

unsigned char AuthChallengeRxMessage::opcode_ = 0x3;

AuthChallengeRxMessage::AuthChallengeRxMessage(void *msg, std::string singleUseToken, std::string serial)
{
    len = sizeof(AuthChallengeRxMsg);

    AuthChallengeRxMsg *msg_ = (AuthChallengeRxMsg *)msg;

    tokenHash = "";
    challenge = "";
    tokenHash.append(msg_->tokenHash, sizeof(msg_->tokenHash));
    challenge.append(msg_->challenge, sizeof(msg_->challenge));

    Encryptor e(singleUseToken, serial);

    if (tokenHash.compare(e.calculateHash()) != 0) {
        throw AuthChallengeException();
    }
}

unsigned char AuthChallengeRxMessage::opcode() {
    return opcode_;
}

