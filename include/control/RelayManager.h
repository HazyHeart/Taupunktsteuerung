// control/RelayManager.h
#pragma once

#include <Arduino.h>
#include "../config/constants.h"

class RelayManager {
private:
    bool fanRunning;
    uint32_t fanStartTime;
    uint32_t fanStopTime;
    bool emergencyStop;
    bool preventStart;
    bool initialized;

public:
    RelayManager();
    bool begin();
    
    // Lüftersteuerung
    void update();
    bool startFan();
    bool stopFan();
    
    // Status-Abfragen
    bool isFanRunning() const { return fanRunning; }
    bool isInitialized() const { return initialized; }
    uint32_t getCurrentRuntime() const;
    bool canStartFan() const;
    
    // Sicherheitsfunktionen
    void emergencyShutdown();
    bool performSelfTest();
};