#include "DisplayManager.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

Adafruit_SH1106G DisplayManager::display = Adafruit_SH1106G(128, 64, &Wire, -1);

void DisplayManager::init(uint8_t address) {
    if (!display.begin(0x3C, true)) { // SH1106 verwendet begin(adresse, reseteinstellung)
        Serial.println("Display konnte nicht initialisiert werden!");
        while (1); // Endlosschleife bei Fehler
    }
    display.clearDisplay();
    display.display();
    Serial.println("Display erfolgreich initialisiert.");
}

void DisplayManager::clear() {
    display.clearDisplay(); // Bildschirm löschen
    display.display();      // Puffer aktualisieren
}

void DisplayManager::printText(const char* text, int16_t x, int16_t y) {
    display.setCursor(x, y);
    display.print(text);
}

void DisplayManager::show() {
    display.display();
}

// Rechteck zeichnen (nur Rahmen)
void DisplayManager::drawRect(int x, int y, int width, int height) {
    display.drawRect(x, y, width, height, SH110X_WHITE);
    display.display();
}

// Rechteck zeichnen (gefüllt)
void DisplayManager::fillRect(int x, int y, int width, int height) {
    display.fillRect(x, y, width, height, SH110X_WHITE);
    display.display();
}