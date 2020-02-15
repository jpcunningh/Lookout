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

AuthChallengeRxMessage::AuthChallengeRxMessage(std::vector<unsigned char> &msg, std::vector<unsigned char> singleUseToken, std::string serial)
{
    len = sizeof(AuthChallengeRxMsg);

    if (msg.size() != len)
    {
        throw AuthChallengeRxException("incorrect AuthChallengeRxMessage length");
    }

    AuthChallengeRxMsg *msg_ = (AuthChallengeRxMsg *)msg.data();

    if (msg_->opcode != opcode_) {
        throw AuthChallengeRxException("incorrect AuthChallengeRxMessage opcode");
    }

    std::string tokenHash = "";
    tokenHash.append(msg_->tokenHash, sizeof(msg_->tokenHash));

    challenge.clear();
    challenge.insert(challenge.end(), msg_->challenge, msg_->challenge + sizeof(msg_->challenge));

    Encryptor e(singleUseToken, serial);

    std::string testHash = e.calculateHash().substr(0, 8);

    if (tokenHash.compare(testHash) != 0) {
        throw AuthChallengeRxException("transmitter failed authentication validation");
    }
}

unsigned char AuthChallengeRxMessage::opcode() {
    return opcode_;
}

