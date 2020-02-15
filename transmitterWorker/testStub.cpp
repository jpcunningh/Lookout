#include <tinyb.hpp>

#include <algorithm>
#include <vector>
#include <iostream>
#include <thread>
#include <atomic>
#include <csignal>
#include <ctype.h>
#include <stdio.h>

#include "bluetoothServices.hpp"
#include "transmitterWorker.hpp"
#include "authRequestTxMessage.hpp"
#include "authChallengeRxMessage.hpp"
#include "encryptor.hpp"
#include "transmitterTimeTxMessage.hpp"
#include "transmitterTimeRxMessage.hpp"
#include "hexdump.hpp"

using namespace TransmitterWorker;

int main()
{
    AuthRequestTxMessage msg(true);

    struct AuthChallengeRxMsg {
      unsigned char opcode;
      char tokenHash[8];  // NOT NULL TERMINATED!!
      char challenge[8];  // NOT NULL TERMINATED!!
    };

    Encryptor e(msg.singleUseToken, "8GNGKM");

    std::string hash = e.calculateHash();
    const char *hash_ptr = hash.c_str();

    std::cerr << "singleUseToken: \n";
    hexdump(&(msg.singleUseToken[0]), 8);

    AuthChallengeRxMsg authChallengeRxMsg;

    authChallengeRxMsg.opcode = 0x3;

    for (int i = 0; i < 8; ++i)
    {
        authChallengeRxMsg.tokenHash[i] = hash_ptr[i];
        authChallengeRxMsg.challenge[i] = 0x1;
    }
    std::cerr << "hash in Msg: \n";
    hexdump(authChallengeRxMsg.tokenHash, 8);

    auto ptr = reinterpret_cast<unsigned char *>(&authChallengeRxMsg);
    std::vector<unsigned char>buff = std::vector<unsigned char>(ptr, ptr + sizeof(authChallengeRxMsg));

    AuthChallengeRxMessage authChallengeRxMsg2(buff, msg.singleUseToken, "8GNGKM");

    TransmitterTimeTxMessage timeMsg;
    std::cerr << "transmitterTimeTxMsg: \n";
    hexdump(timeMsg.getBuff().data(), timeMsg.length());

    struct TransmitterTimeRxMsg {
        struct {
            unsigned char opcode;
            unsigned char status;
            unsigned short currentTimeMSW;      // Ugh - not word aligned
            unsigned short currentTimeLSW;      // Ugh - not word aligned
            unsigned short sessionStartTimeMSW; // Ugh - not word aligned
            unsigned short sessionStartTimeLSW; // Ugh - not word aligned
            unsigned char unknown[4];
        } msg;

        unsigned short crc;
    } timeRxMsg;

    timeRxMsg.msg.opcode = 0x25;
    timeRxMsg.msg.status = 0;
    timeRxMsg.msg.currentTimeMSW = 0x0a0e;
    timeRxMsg.msg.currentTimeLSW = 0x0051;
    timeRxMsg.msg.sessionStartTimeMSW = 0xd2e0;
    timeRxMsg.msg.sessionStartTimeLSW = 0x0049;
    timeRxMsg.msg.unknown[0] = 0x1;
    timeRxMsg.msg.unknown[1] = 0x0;
    timeRxMsg.msg.unknown[2] = 0x0;
    timeRxMsg.msg.unknown[3] = 0x0;

    timeRxMsg.crc = 0x859b;

    ptr = reinterpret_cast<unsigned char *>(&timeRxMsg);
    buff = std::vector<unsigned char>(ptr, ptr + sizeof(timeRxMsg));

    std::cerr << "transmitterTimeRxMsg: \n";
    hexdump(buff.data(), sizeof(timeRxMsg));

    TransmitterTimeRxMessage txmitterTimeRxMsg(buff);

    std::cerr << "status: " << txmitterTimeRxMsg.status << "\n";
    std::cerr << "currentTime: " << txmitterTimeRxMsg.currentTime << "\n";
    std::cerr << "sessionStartTime: " << txmitterTimeRxMsg.sessionStartTime << "\n";
    std::cerr << std::endl;

    return 0;
}
