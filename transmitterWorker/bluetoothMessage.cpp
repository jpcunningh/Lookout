#include <tinyb.hpp>

#include <algorithm>
#include <vector>
#include <iostream>

#include "transmitterWorker.hpp"
#include "bluetoothMessage.hpp"

using namespace TransmitterWorker;

BluetoothMessage::BluetoothMessage()
{
  opcode = 0;
  len = 0;

  buff = nullptr;
}

int BluetoothMessage::length()
{
  return len;
}

unsigned char *BluetoothMessage::getBuff()
{
  return buff;
}

