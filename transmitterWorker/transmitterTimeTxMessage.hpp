#ifndef TRANSMITTER_TIME_TX_MESSAGE_HPP
#define TRANSMITTER_TIME_TX_MESSAGE_HPP

#include "bluetoothMessage.hpp"
#include <sstream>
#include <random>
#include <string>

class TransmitterTimeTxMessage : public BluetoothMessage
{
  private:
    static unsigned char opcode_;

    struct TransmitterTimeTxMsg {
      struct {
          unsigned char opcode;
      } msg;
      unsigned char crcLSB;  // Ugh - not word aligned
      unsigned char crcMSB;  // Ugh - not word aligned
    };

    TransmitterTimeTxMsg transmitterTimeTxMessage;

  public:
    TransmitterTimeTxMessage();
    unsigned char opcode();
};

#endif

