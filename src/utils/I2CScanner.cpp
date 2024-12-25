#include "utils/I2CScanner.h"

void I2CScanner::scan() {
    Serial.println("Starting I2C scan...");
    for (uint8_t address = 1; address < 127; address++) {
        Wire.beginTransmission(address);
        if (Wire.endTransmission() == 0) {
            Serial.print("I2C device found at address 0x");
            if (address < 16) Serial.print("0");
            Serial.println(address, HEX);
        }
    }
    Serial.println("I2C scan complete.");
}

bool I2CScanner::deviceExists(uint8_t address) {
    Wire.beginTransmission(address);
    return (Wire.endTransmission() == 0);
}
