// boot/WatchdogManager.h
#pragma once

#include <Arduino.h>
#include <esp_task_wdt.h>

class WatchdogManager {
private:
    static const uint32_t WDT_TIMEOUT = 30; // Sekunden
    bool initialized;

public:
    WatchdogManager() : initialized(false) {}
    
    bool begin();
    void feed();
    void enable();
    void disable();
    bool isInitialized() const { return initialized; }
};