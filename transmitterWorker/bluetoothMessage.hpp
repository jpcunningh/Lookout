#ifndef BLUETOOTH_MESSAGE_HPP
#define BLUETOOTH_MESSAGE_HPP

#include <vector>

class BluetoothMessage
{
  protected:
    int len;
    std::vector<unsigned char> buff;

  public:
    BluetoothMessage();
    std::vector<unsigned char> getBuff();
    int length();
    virtual unsigned char opcode() = 0;
};

#endif

