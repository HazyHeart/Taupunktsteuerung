// ui/assets/Icons.h
#pragma once
#include <Arduino.h>

namespace UI {
namespace Assets {
namespace Icons {

// 8x8 Icons
const uint8_t ICON_TEMPERATURE[] PROGMEM = {
    0b00100000,
    0b01010000,
    0b01010000,
    0b01010000,
    0b01110000,
    0b11111000,
    0b11111000,
    0b01110000
};

const uint8_t ICON_HUMIDITY[] PROGMEM = {
    0b00100000,
    0b01110000,
    0b11111000,
    0b11111000,
    0b11111000,
    0b11111000,
    0b01110000,
    0b00100000
};

const uint8_t ICON_PRESSURE[] PROGMEM = {
    0b00100000,
    0b01110000,
    0b11111000,
    0b00100000,
    0b00100000,
    0b00100000,
    0b00100000,
    0b00000000
};

} // namespace Icons

// Größere Bitmap-Daten aus einzelnen Dateien einbinden
#include "Logo.h"
#include "Symbols.h"

// Asset Manager für einfachen Zugriff
class AssetManager {
public:
    static const uint8_t* getIcon(const char* name) {
        if (strcmp(name, "temp") == 0) return Icons::ICON_TEMPERATURE;
        if (strcmp(name, "humid") == 0) return Icons::ICON_HUMIDITY;
        if (strcmp(name, "press") == 0) return Icons::ICON_PRESSURE;
        return nullptr;
    }
    
    static uint8_t getIconWidth(const char* name) {
        return 8;  // Für 8x8 Icons
    }
    
    static uint8_t getIconHeight(const char* name) {
        return 8;  // Für 8x8 Icons
    }
};

}} // namespace UI::Assets
