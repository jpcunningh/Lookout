#ifndef AUTH_CHALLENGE_RX_MESSAGE_HPP
#define AUTH_CHALLENGE_RX_MESSAGE_HPP

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
    AuthChallengeRxMessage(std::vector<unsigned char> &msg, std::vector<unsigned char> singleUseToken, std::string serial);
    unsigned char opcode();

    std::vector<unsigned char> challenge;
};

class AuthChallengeRxException : public std::exception
{
  private:
    const char* errorMsg;

  public:
    AuthChallengeRxException(const char *error)
    {
        errorMsg = error;
    }

    const char * what() const throw()
    {
        return errorMsg;
    }
};

#endif

