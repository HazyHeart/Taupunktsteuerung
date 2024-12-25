#ifndef DISPLAYMANAGER_H
#define DISPLAYMANAGER_H

#include <Adafruit_SH110X.h>
#include <Wire.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR 0x3C // Standardadresse des Displays

class DisplayManager {
public:
    static void init();
    static void clear();
    static void printText(const char* text, int16_t x, int16_t y);
    static void show(); // Aktualisiert das Display

private:
    static Adafruit_SH1106G oled; // SH1106-Treiber
};

#endif // DISPLAYMANAGER_H
