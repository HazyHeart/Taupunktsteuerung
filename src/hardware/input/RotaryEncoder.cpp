// hardware/input/RotaryEncoder.cpp
#include "hardware/input/RotaryEncoder.h"

namespace Hardware {
namespace Input {

// Lookup-Tabelle für Zustandsübergänge
static const int8_t ENCODER_STATES[] = {
    0,  // 00 -> 00
    1,  // 00 -> 01
    -1, // 00 -> 10
    0,  // 00 -> 11
    -1, // 01 -> 00
    0,  // 01 -> 01
    0,  // 01 -> 10
    1,  // 01 -> 11
    1,  // 10 -> 00
    0,  // 10 -> 01
    0,  // 10 -> 10
    -1, // 10 -> 11
    0,  // 11 -> 00
    -1, // 11 -> 01
    1,  // 11 -> 10
    0   // 11 -> 11
};

RotaryEncoder::RotaryEncoder(uint8_t pinA, uint8_t pinB)
    : pinA(pinA)
    , pinB(pinB)
    , position(0)
    , moved(false)
    , minValue(INT32_MIN)
    , maxValue(INT32_MAX)
    , wrapAround(false)
    , stepsPerDetent(1)
    , lastState(0)
{
}

bool RotaryEncoder::begin() {
    // Pins konfigurieren mit Pull-up
    pinMode(pinA, INPUT_PULLUP);
    pinMode(pinB, INPUT_PULLUP);
    
    // Initialen Status lesen
    lastState = (digitalRead(pinA) << 1) | digitalRead(pinB);
    
    // Interrupts für beide Pins einrichten
    attachInterruptArg(digitalPinToInterrupt(pinA), isrHandler, this, CHANGE);
    attachInterruptArg(digitalPinToInterrupt(pinB), isrHandler, this, CHANGE);
    
    Serial.printf("Encoder initialisiert (Pins A:%d B:%d)\n", pinA, pinB);
    return true;
}

void RotaryEncoder::reset() {
    position = 0;
    moved = false;
    lastState = (digitalRead(pinA) << 1) | digitalRead(pinB);
}

void IRAM_ATTR RotaryEncoder::isrHandler(void* arg) {
    RotaryEncoder* encoder = static_cast<RotaryEncoder*>(arg);
    encoder->handleInterrupt();
}

void IRAM_ATTR RotaryEncoder::handleInterrupt() {
    // Neuen Status lesen
    uint8_t currentState = (digitalRead(pinA) << 1) | digitalRead(pinB);
    
    // Position basierend auf Zustandsübergang aktualisieren
    uint8_t tableIndex = (lastState << 2) | currentState;
    int8_t direction = ENCODER_STATES[tableIndex];
    
    if (direction != 0) {
        // Position aktualisieren
        int32_t newPosition = position + (direction * stepsPerDetent);
        
        // Grenzen prüfen
        if (newPosition > maxValue) {
            position = wrapAround ? minValue : maxValue;
        } else if (newPosition < minValue) {
            position = wrapAround ? maxValue : minValue;
        } else {
            position = newPosition;
        }
        
        moved = true;
    }
    
    lastState = currentState;
}

bool RotaryEncoder::hasMovement() {
    if (moved) {
        moved = false;
        return true;
    }
    return false;
}

int32_t RotaryEncoder::getMovementDelta() {
    static int32_t lastPosition = 0;
    int32_t delta = position - lastPosition;
    lastPosition = position;
    return delta;
}

void RotaryEncoder::setBoundaries(int32_t min, int32_t max, bool wrap) {
    minValue = min;
    maxValue = max;
    wrapAround = wrap;
    
    // Aktuelle Position an neue Grenzen anpassen
    if (position > maxValue) position = maxValue;
    if (position < minValue) position = minValue;
}

void RotaryEncoder::setStepsPerDetent(uint8_t steps) {
    stepsPerDetent = steps;
}

}} // namespace Hardware::Input
