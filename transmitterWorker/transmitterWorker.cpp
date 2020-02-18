#include <tinyb.hpp>

#include <algorithm>
#include <vector>
#include <iostream>
#include <thread>
#include <atomic>
#include <csignal>
#include <chrono>
#include <ctime>

#include "bluetoothServices.hpp"
#include "transmitterWorker.hpp"
#include "authenticator.hpp"
#include "bondRequestTxMessage.hpp"
#include "keepAliveTxMessage.hpp"
#include "transmitterTimeTxMessage.hpp"
#include "transmitterTimeRxMessage.hpp"
#include "hexdump.hpp"

using namespace TransmitterWorker;

std::atomic<bool> running(true);
std::atomic<bool> debug_scan(false);
std::condition_variable cv;

void signal_handler(int signum)
{
    if (signum == SIGINT) {
        running = false;
    }
}

static void wait_ctrl_c()
{
    std::mutex m;
    std::unique_lock<std::mutex> lock(m);
    std::signal(SIGINT, signal_handler);
    cv.wait(lock);
}

void control_data_cb(__attribute__((unused)) BluetoothGattCharacteristic &c, std::vector<unsigned char> &data, __attribute__((unused)) void *userdata)
{
    std::cerr << "Received control data:" << std::endl;

    hexdump(data.data(), data.size());
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
    bool altBtChannel = false;

    if (argc > 2) {
        std::string str = argv[2];
        std::for_each(str.begin(), str.end(), [](char & c) {
            c = ::tolower(c);
        });
        altBtChannel = !(str.compare("true"));
    }

    std::string device_name = "Dexcom" + serial.substr(serial.length()-2); 

    std::cerr << "Transmitter id: " << serial << " alt channel: " << altBtChannel << std::endl;
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

        if (debug_scan && (list.size() > 0)) {
            auto timenow = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
            std::cerr << "Looking for: " << device_name << "\n";
            std::cerr << "Discovered devices at " << ctime(&timenow) << std::endl;
        }

        for (auto it = list.begin(); it != list.end(); ++it) {

            /* Search for the device with the address given as a parameter to the program */
            if ((*it)->get_name().compare(device_name) == 0) {
                std::cerr << "Class = " << (*it)->get_class_name() << " ";
                std::cerr << "Path = " << (*it)->get_object_path() << " ";
                std::cerr << "Name = " << (*it)->get_name() << " ";
                std::cerr << "Connected = " << (*it)->get_connected() << " ";
                std::cerr << "\n" << std::endl;

                transmitter = (*it).release();
            } else if (debug_scan) {
                std::cerr << "Class = " << (*it)->get_class_name() << " ";
                std::cerr << "Path = " << (*it)->get_object_path() << " ";
                std::cerr << "Name = " << (*it)->get_name() << " ";
                std::cerr << "Connected = " << (*it)->get_connected() << " ";
                std::cerr << "\n" << std::endl;
            }

        }

        /* Free the list of devices and stop if the device was found */
        if (transmitter != nullptr)
            break;
        /* If not, wait and try again */
        std::this_thread::sleep_for(std::chrono::seconds(1));
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
        std::this_thread::sleep_for(std::chrono::seconds(2));

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

    BluetoothGattCharacteristic *commCharacteristic = nullptr;
    BluetoothGattCharacteristic *controlCharacteristic = nullptr;
    BluetoothGattCharacteristic *authCharacteristic = nullptr;
    BluetoothGattCharacteristic *backfillCharacteristic = nullptr;

    auto list = cgm_service->get_characteristics();
    std::cerr << "Discovered characteristics: " << std::endl;
    for (auto it = list.begin(); it != list.end(); ++it) {

        std::cerr << "Class = " << (*it)->get_class_name() << " ";
        std::cerr << "Path = " << (*it)->get_object_path() << " ";
        std::cerr << "UUID = " << (*it)->get_uuid() << " ";
        std::cerr << "Service = " << (*it)->get_service().get_object_path() << " ";
        std::cerr << std::endl;

        if ((*it)->get_uuid() == CGMServiceCharacteristics::Communication)
            commCharacteristic = (*it).release();
        else if ((*it)->get_uuid() == CGMServiceCharacteristics::Control)
            controlCharacteristic = (*it).release();
        else if ((*it)->get_uuid() == CGMServiceCharacteristics::Authentication)
            authCharacteristic = (*it).release();
        else if ((*it)->get_uuid() == CGMServiceCharacteristics::Backfill)
            backfillCharacteristic = (*it).release();
    }

    if (commCharacteristic == nullptr || controlCharacteristic == nullptr || authCharacteristic == nullptr || backfillCharacteristic == nullptr) {
        std::cerr << "Could not find characteristics." << std::endl;
        return 1;
    }

    /* We are connected, now try to read the CGM data */
    try {
        Authenticator auth(authCharacteristic, serial, altBtChannel);
        auth.authenticate();

        std::cerr << "Transmitter bonded: " << +auth.bonded << "\n";
        std::cerr << "Transmitter authenticated: " << +auth.authenticated;
        std::cerr << std::endl;

        if (auth.bonded != 0x1) {
            std::cerr << "Requesting bond" << std::endl;

            KeepAliveTxMessage keepAliveMsg(25);
            std::cerr << "Sending KeepAliveTxMessage:\n";
            hexdump(keepAliveMsg.getBuff().data(), keepAliveMsg.length());
            authCharacteristic->write_value(keepAliveMsg.getBuff());

            BondRequestTxMessage bondRequestMsg;
            std::cerr << "Sending BondRequestTxMessage:\n";
            hexdump(bondRequestMsg.getBuff().data(), bondRequestMsg.length());
            authCharacteristic->write_value(bondRequestMsg.getBuff());
/*
> ACL Data RX: Handle 64 flags 0x02 dlen 6                 #84 [hci0] 93.336170
      SMP: Security Request (0x0b) len 1
        Authentication requirement: Bonding, No MITM, Legacy, No Keypresses (0x01)
< ACL Data TX: Handle 64 flags 0x00 dlen 11                #85 [hci0] 93.336305
      SMP: Pairing Request (0x01) len 6
        IO capability: NoInputNoOutput (0x03)
        OOB data: Authentication data not present (0x00)
        Authentication requirement: Bonding, No MITM, Legacy, No Keypresses (0x01)
        Max encryption key size: 16
        Initiator key distribution: <none> (0x00)
        Responder key distribution: EncKey (0x01)
< ACL Data TX: Handle 64 flags 0x00 dlen 11                #86 [hci0] 93.344880
      ATT: Read By Type Request (0x08) len 6
        Handle range: 0x001c-0x001e
        Attribute type: Client Characteristic Configuration (0x2902)
> ACL Data RX: Handle 64 flags 0x02 dlen 11                #87 [hci0] 93.351453
      SMP: Pairing Response (0x02) len 6
        IO capability: NoInputNoOutput (0x03)
        OOB data: Authentication data not present (0x00)
        Authentication requirement: Bonding, No MITM, Legacy, No Keypresses (0x01)
        Max encryption key size: 16
        Initiator key distribution: <none> (0x00)
        Responder key distribution: EncKey (0x01)
< ACL Data TX: Handle 64 flags 0x00 dlen 21                #88 [hci0] 93.351716
      SMP: Pairing Confirm (0x03) len 16
        Confim value: b8e4cbdfa43c1132b79a93d01f7b9bf1
*/
        }

        controlCharacteristic->enable_value_notifications(control_data_cb, nullptr);

        wait_ctrl_c();

        TransmitterTimeTxMessage timeTxMsg;
        std::cerr << "Sending TransmitterTimeTxMessage:\n";
        hexdump(timeTxMsg.getBuff().data(), timeTxMsg.length());
        controlCharacteristic->write_value(timeTxMsg.getBuff());

        std::vector<unsigned char> response = controlCharacteristic->read_value();
        std::cerr << "Received TransmitterTimeRxMessage:\n";
        hexdump(response.data(), response.size());
        TransmitterTimeRxMessage timeRxMsg(response);

        std::cerr << "Transmitter time: " << timeRxMsg.currentTime;
        std::cerr << "Transmitter status: " << timeRxMsg.status;
        std::cerr << "Transmitter session start time: " << timeRxMsg.sessionStartTime;
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
