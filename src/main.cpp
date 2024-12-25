#include <Arduino.h>
#include "hardware/display/SH1106Driver.h"

Hardware::Display::SH1106Driver display;

void setup() {
    Serial.begin(115200);
    Serial.println("SH1106 Test");

    if (display.initialize(0x3C)) {
        display.clear();
        display.drawString(0, 0, "Hello, SH1106!");
        display.updateDisplay();
    }
}

void loop() {
    // Keine Logik
}
