#ifndef BOND_REQUEST_TX_MESSAGE_HPP
#define BOND_REQUEST_TX_MESSAGE_HPP

#include "bluetoothMessage.hpp"
#include <sstream>
#include <random>
#include <string>

class BondRequestTxMessage : public BluetoothMessage
{
  private:
    static unsigned char opcode_;

    struct BondRequestTxMsg {
      unsigned char opcode;
    };

    BondRequestTxMsg bondRequestTxMessage;

  public:
    BondRequestTxMessage();
    unsigned char opcode();
};

#endif

