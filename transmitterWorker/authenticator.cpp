#include <tinyb.hpp>

#include <algorithm>
#include <vector>
#include <iostream>

#include "transmitterWorker.hpp"
#include "authenticator.hpp"

using namespace TransmitterWorker;

Authenticator::Authenticator(BluetoothGattCharacteristic *auth)
{
  authentication = auth;
}

int Authenticator::authenticate()
{
    try {
        std::vector<unsigned char> config_on {0x01};
        authentication->write_value(config_on);
        /* Read temperature data and display it */
        std::vector<unsigned char> response = authentication->read_value();
        unsigned char *data;
        unsigned int size = response.size();
        if (size > 0) {
            data = response.data();

            std::cerr << "Raw data=[";
            for (unsigned i = 0; i < response.size(); i++)
                std::cerr << std::hex << static_cast<int>(data[i]) << ", ";
            std::cerr << "] ";

            uint16_t ambient_temp, object_temp;
            object_temp = data[0] | (data[1] << 8);
            ambient_temp = data[2] | (data[3] << 8);

            std::cerr << "Ambient temp: " << ambient_temp << " ";
            std::cerr << "Object temp: " << object_temp << " ";
            std::cerr << std::endl;
        }
    } catch (std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
