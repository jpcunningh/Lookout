#ifndef TRANSMITTER_TIME_RX_MESSAGE_HPP
#define TRANSMITTER_TIME_RX_MESSAGE_HPP

#include <exception>

#include "bluetoothMessage.hpp"

class TransmitterTimeRxMessage : public BluetoothMessage
{
  private:
    static unsigned char opcode_;

    struct TransmitterTimeRxMsg {
        struct {
            unsigned char opcode;
            unsigned char status;
            unsigned char currentTime[4];     // Not byte aligned!
            unsigned char sessionStartTime[4];
            unsigned char unknown[4];
        } msg;

        unsigned short crc;
    };

  public:
    TransmitterTimeRxMessage(std::vector<unsigned char> &msg);
    unsigned char opcode();

    unsigned char status;
    unsigned int currentTime;
    unsigned int sessionStartTime;
};

class TransmitterTimeRxException : public std::exception
{
  private:
    const char* errorMsg;

  public:
    TransmitterTimeRxException(const char *error)
    {
        errorMsg = error;
    }

    const char * what() const throw()
    {
        return errorMsg;
    }
};

#endif

