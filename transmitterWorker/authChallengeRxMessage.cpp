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

AuthChallengeRxMessage::AuthChallengeRxMessage(std::vector<unsigned char> &msg, std::string singleUseToken, std::string serial)
{
    len = sizeof(AuthChallengeRxMsg);

    if (msg.size() != len)
    {
        throw AuthChallengeRxException("incorrect message length");
    }

    AuthChallengeRxMsg *msg_ = (AuthChallengeRxMsg *)msg.data();

    std::string tokenHash = "";
    challenge = "";
    tokenHash.append(msg_->tokenHash, sizeof(msg_->tokenHash));
    challenge.append(msg_->challenge, sizeof(msg_->challenge));

    Encryptor e(singleUseToken, serial);

    if (tokenHash.compare(e.calculateHash()) != 0) {
        throw AuthChallengeRxException("transmitter failed authentication validation");
    }
}

unsigned char AuthChallengeRxMessage::opcode() {
    return opcode_;
}

