#ifndef KEEP_ALIVE_TX_MESSAGE_HPP
#define KEEP_ALIVE_TX_MESSAGE_HPP

#include "bluetoothMessage.hpp"
#include <sstream>
#include <random>
#include <string>

class KeepAliveTxMessage : public BluetoothMessage
{
  private:
    static unsigned char opcode_;

    struct KeepAliveTxMsg {
      unsigned char opcode;
      unsigned char time;
    };

    KeepAliveTxMsg keepAliveTxMessage;

  public:
    KeepAliveTxMessage(unsigned char time);
    unsigned char opcode();
};

#endif

