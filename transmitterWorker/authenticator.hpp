#ifndef AUTHENTICATOR_HPP
#define AUTHENTICATOR_HPP

#include <tinyb.hpp>

using namespace TransmitterWorker;

class Authenticator
{
    private:
      BluetoothGattCharacteristic *auth;
      bool altBtChannel;
      std::string serial;

    public:
      Authenticator(BluetoothGattCharacteristic *authCharacteristic, std::string txSerial, bool altBtChan);
      int authenticate();
      unsigned char authenticated;
      unsigned char bonded;
};

#endif

