// hardware/display/DisplayDriver.h
#pragma once
#include <Arduino.h>
#include <cstdint>
#include "DisplayTypes.h"

namespace Hardware {
namespace Display {

class DisplayDriver {
public:
    virtual ~DisplayDriver() = default;
    
    virtual DisplayError initialize(const DisplayConfig& config) = 0;
    virtual bool isInitialized() const = 0;
    virtual uint16_t getWidth() const = 0;
    virtual uint16_t getHeight() const = 0;
    virtual void reset() = 0;
    virtual void clear() = 0;
    virtual void updateDisplay() = 0;  // Geändert von display() zu updateDisplay()
    
    // Zeichenmethoden
    virtual void drawString(int16_t x, int16_t y, const String& text) = 0;
    virtual void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1) = 0;
    virtual void drawRect(int16_t x, int16_t y, int16_t width, int16_t height) = 0;
    virtual void fillRect(int16_t x, int16_t y, int16_t width, int16_t height) = 0;
    virtual DisplayError drawBuffer(const uint8_t* data, size_t size) = 0;
    
    virtual DisplayError getLastError() const = 0;
};

}} // namespace Hardware::Display
