#include "hardware/display/SH1106Driver.h"

namespace Hardware {
namespace Display {

// Konstruktor
SH1106Driver::SH1106Driver()
    : displayDevice(128, 64, &Wire, -1) {}

// Initialisierung mit I2C-Adresse
bool SH1106Driver::initialize(uint8_t i2cAddress) {
    if (!displayDevice.begin(i2cAddress, true)) {
        Serial.println("SH1106 initialization failed!");
        return false;
    }
    displayDevice.clearDisplay();
    displayDevice.setTextSize(1);
    displayDevice.setTextColor(SH110X_WHITE);
    displayDevice.display();
    return true;
}

// Initialisierung mit DisplayConfig
bool SH1106Driver::initialize(const DisplayConfig& config) {
    return initialize(config.i2cAddress); // Weiterleitung
}

// Display löschen
void SH1106Driver::clear() {
    displayDevice.clearDisplay();
}

// Display aktualisieren
void SH1106Driver::updateDisplay() {
    displayDevice.display();
}

// Text auf Display schreiben
void SH1106Driver::drawString(int16_t x, int16_t y, const String& text) {
    displayDevice.setCursor(x, y);
    displayDevice.print(text);
}

} // namespace Display
} // namespace Hardware
