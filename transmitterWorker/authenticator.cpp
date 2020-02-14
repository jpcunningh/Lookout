#include <tinyb.hpp>

#include <algorithm>
#include <vector>
#include <iostream>

#include "transmitterWorker.hpp"
#include "authenticator.hpp"

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

  authenticated = false;
  bonded = false;
}

int Authenticator::authenticate()
{
    try {
        AuthRequestTxMessage authRequestTxMsg(altBtChannel);
        auth->write_value(authRequestTxMsg.getBuff());

        std::vector<unsigned char> response = auth->read_value();
        AuthChallengeRxMessage authChallengeRxMsg(response, authRequestTxMsg.singleUseToken, serial);

        AuthChallengeTxMessage authChallengeTxMsg(authChallengeRxMsg.challenge, serial);
        auth->write_value(authChallengeTxMsg.getBuff());

        response = auth->read_value();
        AuthStatusRxMessage authStatusRxMsg(response);

        authenticated = authStatusRxMsg.authenticated;
        bonded = authStatusRxMsg.bonded;
    } catch (std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
