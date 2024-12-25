#include "boot/BootManager.h"
#include "hardware/display/DisplayManager.h"

void BootManager::init() {
    Serial.begin(115200);
    Serial.println("Bootvorgang gestartet...");

    // Display initialisieren
    DisplayManager::init(0x3C);
}

void BootManager::run() {
    DisplayManager::clear();
    DisplayManager::printText("Booting...", 0, 0);
    DisplayManager::show();
    delay(2000);
}
