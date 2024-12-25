#include <Arduino.h>
#include "hardware/display/SH1106Driver.h"

Hardware::Display::SH1106Driver display;

void setup() {
    Serial.begin(115200);

    // Display initialisieren
    Hardware::Display::DisplayConfig config = {0x3C}; // I2C-Adresse
    if (!display.initialize(config)) {
        Serial.println("Display konnte nicht initialisiert werden!");
        while (1); // Endlosschleife bei Fehler
    }

    // Testanzeige
    display.drawString(0, 0, "Test erfolgreich!");
    display.updateDisplay();
}

void loop() {
    // Keine weitere Logik
}
