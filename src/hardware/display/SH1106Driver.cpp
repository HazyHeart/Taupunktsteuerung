// hardware/display/SH1106Driver.cpp
#include "hardware/display/SH1106Driver.h"
#include "hardware/i2c/I2CManager.h"
#include <Arduino.h>

namespace Hardware {
namespace Display {

SH1106Driver::SH1106Driver() 
    : initialized(false)
    , lastError(DisplayError::None)
    , displayDevice(std::make_unique<Adafruit_SSD1306>(DISPLAY_WIDTH, DISPLAY_HEIGHT, &Wire, -1))
{
}

DisplayError SH1106Driver::initialize(const DisplayConfig& config) {
    if (!displayDevice->begin(SSD1306_SWITCHCAPVCC, config.i2cAddress)) {
        return DisplayError::CommunicationError;
    }
    
    // SH1106 spezifische Anpassung
    displayDevice->ssd1306_command(0xD3);
    displayDevice->ssd1306_command(2);
    
    displayDevice->clearDisplay();
    displayDevice->setTextSize(1);
    displayDevice->setTextColor(SSD1306_WHITE);
    
    initialized = true;
    return DisplayError::None;
}

void SH1106Driver::clear() {
    if (!initialized) return;
    displayDevice->clearDisplay();
}

void SH1106Driver::updateDisplay() {
    if (!initialized) return;
    displayDevice->display();
}

void SH1106Driver::drawString(int16_t x, int16_t y, const String& text) {
    if (!initialized) return;
    displayDevice->setCursor(x, y);
    displayDevice->print(text);
}

void SH1106Driver::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1) {
    if (!initialized) return;
    displayDevice->drawLine(x0, y0, x1, y1, SSD1306_WHITE);
}

void SH1106Driver::drawRect(int16_t x, int16_t y, int16_t width, int16_t height) {
    if (!initialized) return;
    displayDevice->drawRect(x, y, width, height, SSD1306_WHITE);
}

void SH1106Driver::fillRect(int16_t x, int16_t y, int16_t width, int16_t height) {
    if (!initialized) return;
    displayDevice->fillRect(x, y, width, height, SSD1306_WHITE);
}

}} // namespace Hardware::Display