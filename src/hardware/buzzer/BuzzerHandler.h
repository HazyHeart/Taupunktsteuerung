#pragma once

#include <Arduino.h>  // Für uint8_t, delay etc.
#include <stdint.h>  // Für weitere integer types

class BuzzerHandler {
private:
    const uint8_t pin;
    const uint8_t channel;
    bool initialized;

public:
    BuzzerHandler(uint8_t pin, uint8_t channel = 0) 
        : pin(pin)
        , channel(channel)
        , initialized(false) 
    {}

    bool begin() {
        if (!initialized) {
            // LEDC Timer konfigurieren (0-15, 5000Hz, 8-bit Auflösung)
            ledcSetup(channel, 5000, 8);
            ledcAttachPin(pin, channel);
            initialized = true;
        }
        return true;
    }

    void tone(uint16_t frequency, uint32_t duration) {
        if (!initialized) return;
        ledcWriteTone(channel, frequency);
        delay(duration);
        ledcWriteTone(channel, 0);
    }

    void beep(uint32_t duration = 100) {
        tone(2000, duration);  // Standard: 2kHz für 100ms
    }
};
