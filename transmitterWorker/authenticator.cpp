/*
 * Author: Petre Eftime <petre.p.eftime@intel.com>
 * Copyright (c) 2015 Intel Corporation.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

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
