#ifndef SH1106DRIVER_H
#define SH1106DRIVER_H

#include <Adafruit_SH110X.h>
#include <Wire.h>

namespace Hardware {
namespace Display {

// Display-Konfiguration
struct DisplayConfig {
    uint8_t i2cAddress = 0x3C; // Standardadresse für SH1106
};

// SH1106-Treiber
class SH1106Driver {
public:
    SH1106Driver(); // Konstruktor
    ~SH1106Driver() = default;

    bool initialize(uint8_t i2cAddress); // Initialisierung mit I2C-Adresse
    bool initialize(const DisplayConfig& config); // Overload für DisplayConfig

    void clear();
    void updateDisplay();
    void drawString(int16_t x, int16_t y, const String& text);

private:
    Adafruit_SH1106G displayDevice; // SH1106 Display-Instanz
};

} // namespace Display
} // namespace Hardware

#endif // SH1106DRIVER_H
