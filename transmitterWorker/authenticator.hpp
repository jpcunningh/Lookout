#ifndef AUTHENTICATOR_HPP
#define AUTHENTICATOR_HPP

#include <tinyb.hpp>

using namespace TransmitterWorker;

class Authenticator
{
    private:
      BluetoothGattCharacteristic *authentication;

    public:
      Authenticator(BluetoothGattCharacteristic *auth);
      int authenticate();
};

#endif

