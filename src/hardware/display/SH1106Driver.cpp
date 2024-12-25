#include "hardware/display/SH1106Driver.h"

namespace Hardware {
namespace Display {

SH1106Driver::SH1106Driver()
    : displayDevice(DISPLAY_WIDTH, DISPLAY_HEIGHT, &Wire, -1) {}

bool SH1106Driver::initialize(uint8_t i2cAddress) {
    if (!displayDevice.begin(i2cAddress, true)) {
        Serial.println("SH1106 initialization failed!");
        return false;
    }

    displayDevice.clearDisplay();
    displayDevice.setTextSize(1);
    displayDevice.setTextColor(SH110X_WHITE);
    Serial.println("SH1106 initialized successfully.");
    return true;
}

void SH1106Driver::clear() {
    displayDevice.clearDisplay();
}

void SH1106Driver::updateDisplay() {
    displayDevice.display();
}

void SH1106Driver::drawString(int16_t x, int16_t y, const String& text) {
    displayDevice.setCursor(x, y);
    displayDevice.print(text);
}

void SH1106Driver::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1) {
    displayDevice.drawLine(x0, y0, x1, y1, SH110X_WHITE);
}

void SH1106Driver::drawRect(int16_t x, int16_t y, int16_t width, int16_t height) {
    displayDevice.drawRect(x, y, width, height, SH110X_WHITE);
}

void SH1106Driver::fillRect(int16_t x, int16_t y, int16_t width, int16_t height) {
    displayDevice.fillRect(x, y, width, height, SH110X_WHITE);
}

} // namespace Display
} // namespace Hardware
