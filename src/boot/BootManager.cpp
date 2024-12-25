#include "boot/BootManager.h"
#include "hardware/display/DisplayManager.h"
#include "ui/screens/BootScreen.h"
#include "ui/screens/TemperatureScreen.h"
#include "utils/I2CScanner.h"

BootScreen bootScreen;
TemperatureScreen temperatureScreen; // Temperaturanzeige hinzufügen
bool bootCompleted = false; // Zustand für Boot-Prozess
unsigned long lastUpdate = 0; // Zeitstempel für Update

void BootManager::init() {
    Serial.begin(115200);
    Serial.println("Bootvorgang gestartet...");

    // Display initialisieren
    DisplayManager::init(0x3C);
    
    // I2C-Scan aufrufen
    I2CScanner::scan();

    // BootScreen initialisieren
    bootScreen.init();
}

void BootManager::run() {
    if (!bootCompleted) { // Boot-Prozess läuft noch
        bootScreen.update();
        bootScreen.render();

        if (bootScreen.isDone()) {
            Serial.println("Bootvorgang abgeschlossen!");
            bootCompleted = true;

            // Temperaturanzeige vorbereiten
            temperatureScreen.init();
        }
    } else {
        // Temperaturanzeige aktualisieren und rendern
        unsigned long now = millis();
        if (now - lastUpdate > 2000) { // Update alle 2 Sekunden
            temperatureScreen.update();
            lastUpdate = now;
        }
        temperatureScreen.render();
    }
}
