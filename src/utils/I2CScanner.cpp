#include "utils/I2CScanner.h"
#include <Wire.h>

void I2CScanner::scan() { // Statische Methode
    Wire.begin();
    Serial.println("Starte I2C-Scan...");

    for (uint8_t addr = 1; addr < 127; addr++) {
        Wire.beginTransmission(addr);
        if (Wire.endTransmission() == 0) {
            Serial.print("Gefunden: 0x");
            Serial.println(addr, HEX);
        }
    }

    Serial.println("I2C-Scan abgeschlossen.");
}

bool I2CScanner::deviceExists(uint8_t address) {
    Wire.beginTransmission(address);
    return (Wire.endTransmission() == 0);
}
