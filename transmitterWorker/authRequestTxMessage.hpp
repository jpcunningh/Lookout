#ifndef AUTH_CHALLENGE_TX_MESSAGE_HPP
#define AUTH_CHALLENGE_TX_MESSAGE_HPP

#include "bluetoothMessage.hpp"
#include <sstream>
#include <random>
#include <string>

class AuthRequestTxMessage : public BluetoothMessage
{
  private:
    struct AuthRequestTxMsg {
      unsigned char opcode;
      unsigned char singleUseToken[8];
      unsigned char btChannel;
    };

    AuthRequestTxMsg authRequestTxMessage;

    unsigned int random_char();

    std::string generate_hex(const unsigned int len);

    std::string generate_uuidv4();

  public:
    AuthRequestTxMessage(bool altBtChannel);
};

#endif

