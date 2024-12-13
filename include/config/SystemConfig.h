// config/SystemConfig.h
#pragma once
#include <cstdint>

// Konfigurationsstruktur mit Default-Werten
struct SystemConfig {
    // Betriebsparameter
    struct Operating {
        float minTemp = 5.0f;           // Frostschutz
        float maxHumidity = 70.0f;      // Schimmelprävention
        float dewPointDiff = 2.0f;      // Mindest-Taupunktdifferenz
    } operating;

    // Lüftersteuerung
    struct Fan {
        uint32_t minRuntime = 600000;   // 10 Minuten
        uint32_t maxRuntime = 3600000;  // 60 Minuten
        uint32_t minPause = 300000;     // 5 Minuten
    } fan;

    // Display
    struct Display {
        uint8_t contrast = 255;
        uint8_t timeout = 30;           // Sekunden bis Dimmen
        bool flipScreen = false;
    } display;

    // Sensoren
    struct Sensors {
        uint16_t readInterval = 1000;   // ms zwischen Messungen
        uint8_t samples = 5;            // Anzahl Messungen für Mittelwert
        float tempOffset[2] = {0, 0};   // Kalibrierungsoffset
    } sensors;
};