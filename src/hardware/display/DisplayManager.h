#ifndef DISPLAYMANAGER_H
#define DISPLAYMANAGER_H

#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <Wire.h>

class DisplayManager {
public:
    static void init(uint8_t i2cAddress = 0x3C);
    static void clear();
    static void printText(const char* text, int16_t x, int16_t y);
    static void show();

private:
    static Adafruit_SH1106G display; // Verwende fertigen Treiber
};

#endif // DISPLAYMANAGER_H
