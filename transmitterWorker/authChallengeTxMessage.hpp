#ifndef AUTH_CHALLENGE_TX_MESSAGE_HPP
#define AUTH_CHALLENGE_TX_MESSAGE_HPP

#include "bluetoothMessage.hpp"

class AuthChallengeTxMessage : public BluetoothMessage
{
  private:
    struct AuthChallengTxMsg {
      unsigned char opcode;
      unsigned char challengeHash[16];  // NOT NULL TERMINATED!!
    };

    AuthChallengTxMsg authChallengeTxMsg;
  public:
    AuthChallengeTxMessage(char *challenge, char *serial);
};

#endif

