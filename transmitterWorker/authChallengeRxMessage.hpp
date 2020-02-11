#ifndef AUTH_CHALLENGE_TX_MESSAGE_HPP
#define AUTH_CHALLENGE_TX_MESSAGE_HPP

#include <exception>

#include "bluetoothMessage.hpp"

class AuthChallengeRxMessage : public BluetoothMessage
{
  private:
    static unsigned char opcode_;

    struct AuthChallengeRxMsg {
      unsigned char opcode;
      char tokenHash[8];  // NOT NULL TERMINATED!!
      char challenge[8];  // NOT NULL TERMINATED!!
    };

  public:
    AuthChallengeRxMessage(void *msg, std::string singleUseToken, std::string serial);
    unsigned char opcode();

    std::string tokenHash;
    std::string challenge;
};

class AuthChallengeException : public std::exception
{
    const char * what() const throw()
    {
      return "Transmitter failed authentication";
    }
};

#endif

