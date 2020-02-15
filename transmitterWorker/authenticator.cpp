#include <tinyb.hpp>

#include <algorithm>
#include <vector>
#include <iostream>

#include "transmitterWorker.hpp"
#include "authenticator.hpp"
#include "hexdump.hpp"

#include "authRequestTxMessage.hpp"
#include "authChallengeRxMessage.hpp"
#include "authChallengeTxMessage.hpp"
#include "authStatusRxMessage.hpp"

using namespace TransmitterWorker;

Authenticator::Authenticator(BluetoothGattCharacteristic *authCharacteristic, std::string txSerial, bool altBtChan)
{
  auth = authCharacteristic;
  serial = txSerial;
  altBtChannel = altBtChan;

  authenticated = 0;
  bonded = 0;
}

int Authenticator::authenticate()
{
    try {
        AuthRequestTxMessage authRequestTxMsg(altBtChannel);
        std::cerr << "Sending AuthRequestTxMessage:\n";
        hexdump(authRequestTxMsg.getBuff().data(), authRequestTxMsg.length());
        auth->write_value(authRequestTxMsg.getBuff());

        std::vector<unsigned char> response = auth->read_value();
        std::cerr << "Received AuthChallengeRxMessage:\n";
        hexdump(response.data(), response.size());
        AuthChallengeRxMessage authChallengeRxMsg(response, authRequestTxMsg.singleUseToken, serial);

        AuthChallengeTxMessage authChallengeTxMsg(authChallengeRxMsg.challenge, serial);
        std::cerr << "Sending AuthChallengeTxMessage:\n";
        hexdump(authChallengeTxMsg.getBuff().data(), authChallengeTxMsg.length());
        auth->write_value(authChallengeTxMsg.getBuff());

        response = auth->read_value();
        std::cerr << "Received AuthStatusRxMessage:\n";
        hexdump(response.data(), response.size());
        AuthStatusRxMessage authStatusRxMsg(response);

        authenticated = authStatusRxMsg.authenticated;
        bonded = authStatusRxMsg.bonded;
    } catch (std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
