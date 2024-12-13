// include/utils/I2CScanner.h
#pragma once
#include <Wire.h>
#include <vector>
#include <Arduino.h>

namespace Utils {

struct I2CDevice {
    uint8_t address;
    const char* description;  // Optional: Bekannte Geräte beschreiben
};

class I2CScanner {
public:
    // Einzelnen Bus scannen
    static std::vector<I2CDevice> scan(TwoWire& wire, const char* busName) {
        std::vector<I2CDevice> devices;
        
        Serial.printf("\nScanne I2C Bus (%s)...\n", busName);
        
        for(uint8_t address = 1; address < 127; address++) {
            wire.beginTransmission(address);
            uint8_t error = wire.endTransmission();
            
            if (error == 0) {
                I2CDevice device = {
                    .address = address,
                    .description = getDeviceDescription(address)
                };
                devices.push_back(device);
                
                Serial.printf("Gerät gefunden auf 0x%02X: %s\n", 
                    address, device.description);
            }
        }
        
        if (devices.empty()) {
            Serial.printf("Keine I2C Geräte auf Bus (%s) gefunden!\n", busName);
        }
        
        return devices;
    }
    
    // Mehrere Busse scannen
    static void scanBuses(std::initializer_list<std::pair<TwoWire&, const char*>> buses) {
        for (const auto& bus : buses) {
            scan(bus.first, bus.second);
        }
    }
    
    // Debug-Ausgabe für Boot-Sequenz
    static void printBusStatus(TwoWire& wire, const char* busName, 
                             uint8_t sda, uint8_t scl) {
        Serial.printf("I2C Bus (%s):\n", busName);
        Serial.printf("- SDA: GPIO%d\n", sda);
        Serial.printf("- SCL: GPIO%d\n", scl);
        Serial.printf("- Clock: %d Hz\n", wire.getClock());
        auto devices = scan(wire, busName);
        Serial.printf("- Gefundene Geräte: %d\n\n", devices.size());
    }

private:
    static const char* getDeviceDescription(uint8_t address) {
        switch(address) {
            case 0x3C: return "SH1106/SSD1306 Display";
            case 0x76: return "BME280 Sensor";
            case 0x77: return "BME280 Sensor (alt. Addr)";
            case 0x68: return "RTC";
            case 0x57: return "EEPROM";
            default:   return "Unbekanntes Gerät";
        }
    }
};

} // namespace Utils