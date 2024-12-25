// I2CManager.cpp
#include "hardware/i2c/I2CManager.h"

namespace Hardware {
namespace I2C {

// Statische Member-Variablen initialisieren
bool I2CManager::initialized = false;
TwoWire I2CManager::internalBus(0);
TwoWire I2CManager::externalBus(1);

bool I2CManager::initialize() {
    Serial.println("Initialisiere I2C Bus...");
    
    // Pins vorbereiten
    pinMode(Pins::I2C1_SDA, INPUT_PULLUP);
    pinMode(Pins::I2C1_SCL, INPUT_PULLUP);
    pinMode(Pins::I2C2_SDA, INPUT_PULLUP);
    pinMode(Pins::I2C2_SCL, INPUT_PULLUP);
    delay(50);

    // Busse initialisieren
    internalBus.begin(Pins::I2C1_SDA, Pins::I2C1_SCL);
    externalBus.begin(Pins::I2C2_SDA, Pins::I2C2_SCL);
    internalBus.setClock(100000);
    externalBus.setClock(100000);
    
    initialized = true;
    return true;
}

bool I2CManager::deviceResponds(TwoWire& wire, uint8_t address) {
    // Bus für den Test konfigurieren
    uint8_t sda = (&wire == &internalBus) ? Pins::I2C1_SDA : Pins::I2C2_SDA;
    uint8_t scl = (&wire == &internalBus) ? Pins::I2C1_SCL : Pins::I2C2_SCL;
    
    wire.begin(sda, scl);
    wire.setClock(10000);
    delay(50);

    Serial.printf("\nStarte I2C Test für Gerät 0x%02X:\n", address);
    Serial.printf("- Bus: %s (SDA=%d, SCL=%d)\n", 
                 (&wire == &internalBus) ? "Intern" : "Extern", sda, scl);
    
    wire.beginTransmission(address);
    wire.write(0x00);
    uint8_t error = wire.endTransmission(true);
    
    if (error == 0) {
        Serial.println("- Gerät erfolgreich gefunden");
        return true;
    }

    Serial.printf("- Test fehlgeschlagen (Error: %d)\n", error);
    return false;
}

void I2CManager::scanBus(TwoWire& wire, const char* name) {
    Serial.printf("\nScanne I2C Bus %s...\n", name);
    
    uint8_t devices = 0;
    for(uint8_t addr = 1; addr < 127; addr++) {
        if (deviceResponds(wire, addr)) {
            devices++;
        }
    }
    
    Serial.printf("Gefunden: %d Geräte\n", devices);
}

}} // namespace Hardware::I2C
