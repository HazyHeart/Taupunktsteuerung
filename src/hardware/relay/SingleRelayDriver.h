#pragma once
#include "RelayDriver.h"
#include <Arduino.h>

namespace Hardware {
namespace Relay {

class SingleRelayDriver : public RelayDriver {
public:
    SingleRelayDriver();
    ~SingleRelayDriver() override = default;
    
    // RelayDriver Interface
    RelayError initialize(const RelayConfig& config) override;
    bool isInitialized() const override { return initialized; }
    void reset() override;
    
    RelayError switchOn() override;
    RelayError switchOff() override;
    bool getState() const override { return currentState; }
    
    RelayError getLastError() const override { return lastError; }
    bool performSelfTest() override;
    
    uint32_t getOnTime() const override;
    uint32_t getOffTime() const override;
    bool canSwitch() const override;

private:
    bool initialized;
    RelayError lastError;
    
    // Konfiguration
    uint8_t relayPin;
    bool invertedLogic;
    uint32_t minSwitchInterval;
    String description;
    
    // Status
    bool currentState;          // Aktueller Schaltzustand
    uint32_t lastSwitchTime;   // Zeitpunkt des letzten Schaltvorgangs
    uint32_t switchOnTime;     // Zeitpunkt des Einschaltens
    uint32_t switchOffTime;    // Zeitpunkt des Ausschaltens
    
    void setError(RelayError error);
    bool writePin(bool state);
};

}} // namespace Hardware::Relay
