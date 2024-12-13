// hardware/display/DisplayTypes.h
#pragma once
#include <cstdint>

namespace Hardware {
namespace Display {

// Basis-Konfiguration für das Display
struct DisplayConfig {
    uint8_t i2cAddress;
    uint8_t width;
    uint8_t height;
    uint8_t sda;
    uint8_t scl;
};

// Status/Fehler Handling
enum class DisplayError {
    None,
    NotInitialized,
    CommunicationError,
    I2CError,
    InvalidParameter
};

}} // namespace Hardware::Display