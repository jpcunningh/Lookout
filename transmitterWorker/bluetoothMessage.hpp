#ifndef BLUETOOTH_MESSAGE_HPP
#define BLUETOOTH_MESSAGE_HPP

class BluetoothMessage
{
  protected:
    int len;
    unsigned char *buff;

  public:
    BluetoothMessage();
    unsigned char *getBuff();
    int length();
    virtual unsigned char opcode() = 0;
};

#endif

