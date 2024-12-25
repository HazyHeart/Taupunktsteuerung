// boot/WatchdogManager.cpp
#include "boot/WatchdogManager.h"

bool WatchdogManager::begin() {
    esp_task_wdt_init(WDT_TIMEOUT, true);
    esp_task_wdt_add(NULL);
    initialized = true;
    return true;
}

void WatchdogManager::feed() {
    if (initialized) {
        esp_task_wdt_reset();
    }
}

void WatchdogManager::enable() {
    if (initialized) {
        esp_task_wdt_add(NULL);
    }
}

void WatchdogManager::disable() {
    if (initialized) {
        esp_task_wdt_delete(NULL);
    }
}
