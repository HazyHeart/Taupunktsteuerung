// ui/assets/bitmaps/Logo.h
#pragma once
#include <Arduino.h>

namespace UI {
namespace Assets {
namespace Bitmaps {

// 32x32 Logo für Boot-Screen
const uint8_t LOGO_32x32[] PROGMEM = {
    // TODO: Hier kommt das eigentliche Logo-Bitmap
    // Beispiel eines einfachen "TP" Logos (Taupunkt)
    0x00, 0xFF, 0xFF, 0x00,
    0x00, 0xFF, 0xFF, 0x00,
    0x00, 0x18, 0x00, 0x00,
    0x00, 0x18, 0x00, 0x00,
    // ... weitere Bytes
};

}}} // namespace UI::Assets::Bitmaps