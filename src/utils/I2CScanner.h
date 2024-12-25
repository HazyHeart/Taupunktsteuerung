#ifndef I2CSCANNER_H
#define I2CSCANNER_H

#include <Wire.h>
#include <Arduino.h>

class I2CScanner {
public:
    static void scan(); // Scannt alle I2C-Geräte und gibt Adressen aus
    bool deviceExists(uint8_t address); // Prüft, ob ein Gerät unter der Adresse existiert
};

#endif // I2CSCANNER_H
