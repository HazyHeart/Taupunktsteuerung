// hardware/display/SH1106Driver.h
#pragma once
#include "DisplayDriver.h"
#include <Adafruit_SSD1306.h>
#include <memory>

namespace Hardware {
namespace Display {

class SH1106Driver : public DisplayDriver {
public:
    static constexpr uint16_t DISPLAY_WIDTH = 128;
    static constexpr uint16_t DISPLAY_HEIGHT = 64;

    SH1106Driver();
    virtual ~SH1106Driver() = default;
    
    // DisplayDriver Interface
    DisplayError initialize(const DisplayConfig& config) override;
    bool isInitialized() const override { return initialized; }
    uint16_t getWidth() const override { return DISPLAY_WIDTH; }
    uint16_t getHeight() const override { return DISPLAY_HEIGHT; }
    void reset() override;
    void clear() override;
    void updateDisplay() override;  // Umbenennung von display() zu updateDisplay()
    
    // Zeichenfunktionen
    void setPixel(int16_t x, int16_t y, bool white);
    DisplayError drawBuffer(const uint8_t* data, size_t size) override;
    void drawString(int16_t x, int16_t y, const String& text) override;
    void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1) override;
    void drawRect(int16_t x, int16_t y, int16_t width, int16_t height) override;
    void fillRect(int16_t x, int16_t y, int16_t width, int16_t height) override;
    
    DisplayError getLastError() const override { return lastError; }

private:
    bool initialized{false};
    DisplayError lastError{DisplayError::None};
    std::unique_ptr<Adafruit_SSD1306> displayDevice;  // Umbenennung von display zu displayDevice
};

}} // namespace Hardware::Display