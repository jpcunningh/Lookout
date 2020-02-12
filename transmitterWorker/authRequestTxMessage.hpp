#ifndef AUTH_REQUEST_TX_MESSAGE_HPP
#define AUTH_REQUEST_TX_MESSAGE_HPP

#include "bluetoothMessage.hpp"
#include <sstream>
#include <random>
#include <string>

class AuthRequestTxMessage : public BluetoothMessage
{
  private:
    static unsigned char opcode_;

    struct AuthRequestTxMsg {
      unsigned char opcode;
      char singleUseToken[8];
      unsigned char btChannel;
    };

    AuthRequestTxMsg authRequestTxMessage;

    unsigned int random_char();

    std::string generate_hex(const unsigned int len);

    std::string generate_uuidv4();

  public:
    AuthRequestTxMessage(bool altBtChannel);
    unsigned char opcode();
    std::string singleUseToken;
};

#endif

