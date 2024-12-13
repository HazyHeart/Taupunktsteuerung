// hardware/input/RotaryEncoder.h
#pragma once
#include <Arduino.h>

namespace Hardware {
namespace Input {

class RotaryEncoder {
public:
    RotaryEncoder(uint8_t pinA, uint8_t pinB);
    
    bool begin();
    void reset();
    
    // Basis-Funktionen
    int32_t getPosition() const { return position; }
    void setPosition(int32_t pos) { position = pos; }
    
    // Bewegungserkennung
    bool hasMovement();
    int32_t getMovementDelta();
    
    // Konfiguration
    void setBoundaries(int32_t min, int32_t max, bool wrap = false);
    void setStepsPerDetent(uint8_t steps = 1);

private:
    const uint8_t pinA;
    const uint8_t pinB;
    volatile int32_t position;
    volatile bool moved;
    
    // Grenzen
    int32_t minValue;
    int32_t maxValue;
    bool wrapAround;
    uint8_t stepsPerDetent;
    
    // ISR Handling
    static void IRAM_ATTR isrHandler(void* arg);
    void handleInterrupt();
    
    // Status
    uint8_t lastState;
    int8_t getDirection();
};

}} // namespace Hardware::Input