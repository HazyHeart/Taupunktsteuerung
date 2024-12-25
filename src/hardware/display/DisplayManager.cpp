#include "hardware/display/DisplayManager.h"

Adafruit_SH1106G DisplayManager::display = Adafruit_SH1106G(128, 64, &Wire, -1);

void DisplayManager::init(uint8_t i2cAddress) {
    if (!display.begin(i2cAddress, true)) {
        Serial.println("Display konnte nicht initialisiert werden!");
        for (;;); // Endlosschleife bei Fehler
    }
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SH110X_WHITE);
    display.display();
    Serial.println("Display erfolgreich initialisiert.");
}

void DisplayManager::clear() {
    display.clearDisplay();
}

void DisplayManager::printText(const char* text, int16_t x, int16_t y) {
    display.setCursor(x, y);
    display.print(text);
}

void DisplayManager::show() {
    display.display();
}
