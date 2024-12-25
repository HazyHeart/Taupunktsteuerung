#include "ui/screens/BootScreen.h"

// Konstruktor
BootScreen::BootScreen()
    : startTime(0), duration(3000), progress(0) {} // 3 Sekunden Bootzeit

void BootScreen::init() {
    startTime = millis(); // Startzeit speichern
    progress = 0;         // Fortschritt zurücksetzen
    DisplayManager::clear();
    DisplayManager::printText("Booting...", 0, 0);
    DisplayManager::show();
}

void BootScreen::update() {
    unsigned long elapsed = millis() - startTime;
    progress = (elapsed * 100) / duration; // Fortschritt berechnen (0–100%)

    if (progress > 100) progress = 100; // Maximalwert begrenzen
}

void BootScreen::render() {
    DisplayManager::clear();

    // Text anzeigen
    DisplayManager::printText("Booting...", 0, 0);

    // Fortschrittsbalken zeichnen (maximal 120 Pixel breit)
    int barWidth = (progress * 120) / 100; // Maximal 120 Pixel Breite
    if (barWidth > 120) barWidth = 120;   // Begrenzung

    for (int i = 0; i < barWidth; i++) {
        DisplayManager::printText("=", i, 10); // Balken zeichnen
    }

    DisplayManager::show();
}

bool BootScreen::isDone() {
    return (millis() - startTime) >= duration; // Ist der Bootprozess abgeschlossen?
}
