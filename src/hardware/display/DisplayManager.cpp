#include "hardware/display/DisplayManager.h"

// SH1106-Instanz (128x64, I2C, Reset-Pin -1)
Adafruit_SH1106G DisplayManager::oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void DisplayManager::init() {
    Serial.println("Initializing SH1106 display...");
    if (!oled.begin(0x3C, true)) { // 0x3C ist die Standardadresse, true aktiviert I2C
        Serial.println("SH1106 not found. Check wiring!");
        for (;;); // Stoppe, wenn Initialisierung fehlschlägt
    }
    delay(100); // Wartezeit nach Initialisierung
    clear();
    oled.setTextSize(1);
    oled.setTextColor(SH110X_WHITE);
    Serial.println("SH1106 initialized successfully.");
}

void DisplayManager::clear() {
    oled.clearDisplay();
}

void DisplayManager::printText(const char* text, int16_t x, int16_t y) {
    oled.setCursor(x, y);
    oled.print(text);
}

void DisplayManager::show() {
    oled.display();
}

auto error = displayDriver.initialize(displayConfig);