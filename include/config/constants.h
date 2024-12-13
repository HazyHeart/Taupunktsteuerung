// config/constants.h
#pragma once

namespace SystemConstants {
    // System
    constexpr uint32_t SERIAL_BAUD = 115200;
    constexpr uint32_t WATCHDOG_TIMEOUT = 30000;  // 30 Sekunden
    constexpr uint32_t MIN_FREE_HEAP = 32768;     // 32KB minimaler freier Heap
    
    // Sensor Sampling
    constexpr uint32_t SENSOR_READ_INTERVAL = 1000;   // 1 Sekunde
    constexpr uint8_t SENSOR_SAMPLES = 5;             // Anzahl Messungen für Mittelwert
    
    // Grenzwerte
    constexpr float MIN_TEMP = 5.0f;                  // Frostschutz
    constexpr float MAX_HUMIDITY = 70.0f;             // Schimmelprävention
    constexpr float DEWPOINT_DIFFERENCE = 2.0f;       // Mindestdifferenz Taupunkt
    
    // Lüftersteuerung
    constexpr uint32_t MIN_FAN_RUNTIME = 600000;      // 10 Minuten
    constexpr uint32_t MAX_FAN_RUNTIME = 3600000;     // 60 Minuten
    constexpr uint32_t MIN_FAN_PAUSE = 300000;        // 5 Minuten
    
    // Konfiguration
    constexpr uint32_t CONFIG_VERSION = 1;
    constexpr uint32_t CONFIG_MAGIC = 0xC0FFEE42;
}