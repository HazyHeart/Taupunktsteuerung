#ifndef SH1106DRIVER_H
#define SH1106DRIVER_H

#include <Adafruit_SH110X.h>
#include <Wire.h>

#include "DisplayDriver.h"

namespace Hardware {
namespace Display {

constexpr uint8_t DISPLAY_WIDTH = 128;
constexpr uint8_t DISPLAY_HEIGHT = 64;

class SH1106Driver : public DisplayDriver {
public:
    SH1106Driver();
    ~SH1106Driver() = default;

    bool initialize(const DisplayConfig& config) {
    return initialize(config.i2cAddress);
}
    void clear();
    void updateDisplay();
    void drawString(int16_t x, int16_t y, const String& text);
    void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1);
    void drawRect(int16_t x, int16_t y, int16_t width, int16_t height);
    void fillRect(int16_t x, int16_t y, int16_t width, int16_t height);

private:
    Adafruit_SH1106G displayDevice; // Direkte Instanz des Displays
};

} // namespace Display
} // namespace Hardware

#endif // SH1106DRIVER_H