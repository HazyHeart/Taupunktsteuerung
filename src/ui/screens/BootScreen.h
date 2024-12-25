#ifndef BOOTSCREEN_H
#define BOOTSCREEN_H

#include "../../hardware/display/DisplayManager.h"

class BootScreen {
public:
    BootScreen(); // Konstruktor
    void init();  // Initialisiert den Screen
    void update(); // Logik-Update für Fortschrittsanzeige
    void render(); // Darstellung auf dem Display
    bool isDone(); // Prüft, ob der Bootprozess abgeschlossen ist

private:
    unsigned long startTime;   // Startzeit des BootScreens
    unsigned long duration;   // Dauer des Bootvorgangs in Millisekunden
    int progress;              // Fortschritt (0–100%)
};

#endif // BOOTSCREEN_H
