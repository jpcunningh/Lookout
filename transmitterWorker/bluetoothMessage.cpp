#include "transmitterWorker.hpp"
#include "bluetoothMessage.hpp"

using namespace TransmitterWorker;

BluetoothMessage::BluetoothMessage()
{
  len = 0;
}

int BluetoothMessage::length()
{
  return len;
}

std::vector<unsigned char> BluetoothMessage::getBuff()
{
  return buff;
}

