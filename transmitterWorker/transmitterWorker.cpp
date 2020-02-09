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
#include <thread>
#include <atomic>
#include <csignal>

#include "bluetoothServices.hpp"
#include "transmitterWorker.hpp"

using namespace TransmitterWorker;

std::atomic<bool> running(true);

void signal_handler(int signum)
{
    if (signum == SIGINT) {
        running = false;
    }
}

int main(int argc, char **argv)
{
    if (argc < 2) {
        std::cerr << "Run as: " << argv[0] << " <transmitter id> [alt bluetooth channel]\n";
        std::cerr << std::endl << "Options:\n";
        std::cerr << "  <transmitter id>\tDexcom transmitter serial number\n";
        std::cerr << "  [alt bluetooth channel]\ttrue/false for whether to use alternate Bluetooth channel" << std::endl;
        exit(1);
    }

    std::string serial = argv[1];
    bool alt_bt_channel = false;

    if (argc > 2) {
        std::string str = argv[2];
        std::for_each(str.begin(), str.end(), [](char & c) {
            c = ::tolower(c);
        });
        alt_bt_channel = !(str.compare("true"));
    }

    std::string device_name = "Dexcom" + serial.substr(serial.length()-3); 

    std::cerr << "Transmitter id: " << serial << " alt channel: " << alt_bt_channel << std::endl;
    std::cerr << "Device name: " << device_name << std::endl;

    BluetoothManager *manager = nullptr;
    try {
        manager = BluetoothManager::get_bluetooth_manager();
    } catch(const std::runtime_error& e) {
        std::cerr << "Error while initializing libtinyb: " << e.what() << std::endl;
        exit(1);
    }

    /* Start the discovery of devices */
    bool ret = manager->start_discovery();
    std::cerr << "Started = " << (ret ? "true" : "false") << std::endl;

    BluetoothDevice *transmitter = NULL;
    BluetoothGattService *cgm_service = NULL;

    for (;;) {
        /* Get the list of devices */
        auto list = manager->get_devices();

        if (list.size() > 0) {
            std::cerr << "Discovered devices: " << std::endl;
        }

        for (auto it = list.begin(); it != list.end(); ++it) {

            std::cerr << "Class = " << (*it)->get_class_name() << " ";
            std::cerr << "Path = " << (*it)->get_object_path() << " ";
            std::cerr << "Name = " << (*it)->get_name() << " ";
            std::cerr << "Connected = " << (*it)->get_connected() << " ";
            std::cerr << std::endl;

            /* Search for the device with the address given as a parameter to the program */
            if ((*it)->get_name().compare(device_name) == 0)
                transmitter = (*it).release();
        }

        /* Free the list of devices and stop if the device was found */
        if (transmitter != nullptr)
            break;
        /* If not, wait and try again */
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cerr << std::endl;
    }

    /* Stop the discovery (the device was found or number of tries ran out */
    ret = manager->stop_discovery();
    std::cerr << "Stopped = " << (ret ? "true" : "false") << std::endl;

    if (transmitter == nullptr) {
        std::cerr << "Could not find device " << argv[1] << std::endl;
        return 1;
    }

    /* Connect to the device and get the list of services exposed by it */
    transmitter->connect();
    std::cerr << "Discovered services: " << std::endl;
    while (true) {
        /* Wait for the device to come online */
        std::this_thread::sleep_for(std::chrono::seconds(4));

        auto list = transmitter->get_services();
        if (list.empty())
            continue;

        for (auto it = list.begin(); it != list.end(); ++it) {
            std::cerr << "Class = " << (*it)->get_class_name() << " ";
            std::cerr << "Path = " << (*it)->get_object_path() << " ";
            std::cerr << "UUID = " << (*it)->get_uuid() << " ";
            std::cerr << "Device = " << (*it)->get_device().get_object_path() << " ";
            std::cerr << std::endl;

            /* Search for the cgm service, by UUID */
            if ((*it)->get_uuid().compare(TransmitterServices::CGMService) == 0)
                cgm_service = (*it).release();
        }
        break;
    }

    if (cgm_service == nullptr) {
        std::cerr << "Could not find service f000aa00-0451-4000-b000-000000000000" << std::endl;
        return 1;
    }

    BluetoothGattCharacteristic *communication = nullptr;
    BluetoothGattCharacteristic *control = nullptr;
    BluetoothGattCharacteristic *authentication = nullptr;
    BluetoothGattCharacteristic *backfill = nullptr;

    auto list = cgm_service->get_characteristics();
    std::cerr << "Discovered characteristics: " << std::endl;
    for (auto it = list.begin(); it != list.end(); ++it) {

        std::cerr << "Class = " << (*it)->get_class_name() << " ";
        std::cerr << "Path = " << (*it)->get_object_path() << " ";
        std::cerr << "UUID = " << (*it)->get_uuid() << " ";
        std::cerr << "Service = " << (*it)->get_service().get_object_path() << " ";
        std::cerr << std::endl;

        if ((*it)->get_uuid() == CGMServiceCharacteristics::Communication)
            communication = (*it).release();
        else if ((*it)->get_uuid() == CGMServiceCharacteristics::Control)
            control = (*it).release();
        else if ((*it)->get_uuid() == CGMServiceCharacteristics::Authentication)
            authentication = (*it).release();
        else if ((*it)->get_uuid() == CGMServiceCharacteristics::Backfill)
            backfill = (*it).release();
    }

    if (communication == nullptr || control == nullptr || authentication == nullptr || backfill == nullptr) {
        std::cerr << "Could not find characteristics." << std::endl;
        return 1;
    }

    /* We are connected, now try to read the CGM data */
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

    /* Disconnect from the device */
     try {
        transmitter->disconnect();
    } catch (std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return 0;
}
