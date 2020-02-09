#ifndef BLUETOOTH_SERVICES_HPP
#define BLUETOOTH_SERVICES_HPP

namespace TransmitterServices
{
    const std::string DeviceInfo = "180a";
    const std::string Advertisement = "febc";
    const std::string CGMService = "f8083532-849e-531c-c594-30f1f86a4ea5";
    const std::string ServiceB = "f8084532-849e-531c-c594-30f1f86a4ea5";
}

namespace CGMServiceCharacteristics
{
    const std::string Communication = "f8083533-849e-531c-c594-30f1f86a4ea5";
    const std::string Control = "f8083534-849e-531c-c594-30f1f86a4ea5";
    const std::string Authentication = "f8083535-849e-531c-c594-30f1f86a4ea5";
    const std::string Backfill = "f8083536-849e-531c-c594-30f1f86a4ea5";
}

#endif
