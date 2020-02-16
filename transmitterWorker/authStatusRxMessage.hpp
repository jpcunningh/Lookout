#ifndef AUTH_STATUS_RX_MESSAGE_HPP
#define AUTH_STATUS_RX_MESSAGE_HPP

#include <exception>

#include "bluetoothMessage.hpp"

class AuthStatusRxMessage : public BluetoothMessage
{
  private:
    static unsigned char opcode_;

    struct AuthStatusRxMsg {
      unsigned char opcode;
      unsigned char authenticated;
      unsigned char bonded;
    };

  public:
    AuthStatusRxMessage(std::vector<unsigned char> &msg);
    unsigned char opcode();

    unsigned char authenticated;
    unsigned char bonded;
};

class AuthStatusRxException : public std::exception
{
  private:
    const char* errorMsg;

  public:
    AuthStatusRxException(const char *error)
    {
        errorMsg = error;
    }

    const char * what() const throw()
    {
        return errorMsg;
    }
};

#endif

