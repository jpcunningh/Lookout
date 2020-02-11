#ifndef AUTH_CHALLENGE_TX_MESSAGE_HPP
#define AUTH_CHALLENGE_TX_MESSAGE_HPP

#include "bluetoothMessage.hpp"

class AuthChallengeTxMessage : public BluetoothMessage
{
  private:
    static unsigned char opcode_;

    struct AuthChallengTxMsg {
      unsigned char opcode;
      unsigned char challengeHash[8];  // NOT NULL TERMINATED!!
    };

    AuthChallengTxMsg authChallengeTxMsg;
  public:
    AuthChallengeTxMessage(char *challenge, char *serial);
    unsigned char opcode();
};

#endif

