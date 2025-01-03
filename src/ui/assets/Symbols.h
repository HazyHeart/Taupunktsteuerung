// ui/assets/bitmaps/Symbols.h
#pragma once
#include <Arduino.h>

namespace UI {
namespace Assets {
namespace Bitmaps {

// 16x16 Status-Symbole
const uint8_t SYMBOL_OK[] PROGMEM = {
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x0C,
    0x00, 0x1C,
    0x00, 0x38,
    0x00, 0x70,
    0x38, 0xE0,
    0x7D, 0xC0,
    0xFF, 0x80,
    0x7F, 0x00,
    0x3E, 0x00,
    0x1C, 0x00,
    0x08, 0x00,
    0x00, 0x00,
    0x00, 0x00
};

const uint8_t SYMBOL_WARNING[] PROGMEM = {
    0x00, 0x00,
    0x03, 0x80,
    0x07, 0xC0,
    0x0E, 0xE0,
    0x1C, 0x70,
    0x38, 0x38,
    0x70, 0x1C,
    0x70, 0x1C,
    0x70, 0x1C,
    0x38, 0x38,
    0x1C, 0x70,
    0x0E, 0xE0,
    0x07, 0xC0,
    0x03, 0x80,
    0x00, 0x00,
    0x00, 0x00
};

}}} // namespace UI::Assets::Bitmaps
