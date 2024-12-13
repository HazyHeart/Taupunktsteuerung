// I2CManager.h
#pragma once
#include <Wire.h>
#include <Arduino.h>  // Für Serial
#include "config/pins.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

namespace Hardware {
namespace I2C {

class I2CManager {
public:
    static bool initialize();
    static TwoWire& getInternalBus() { return internalBus; }
    static TwoWire& getExternalBus() { return externalBus; }
    
    // Vereinfachtes Bus Management
    static bool lockBus(TwoWire& wire) {
        return true;  // Vorerst immer erlauben
    }
    static void unlockBus(TwoWire& wire) {
        // Nichts tun
    }
    
    static bool deviceResponds(TwoWire& wire, uint8_t address);
    static void scanBus(TwoWire& wire, const char* name);

private:
    static bool initialized;
    static TwoWire internalBus;
    static TwoWire externalBus;
};

}} // namespace Hardware::I2C