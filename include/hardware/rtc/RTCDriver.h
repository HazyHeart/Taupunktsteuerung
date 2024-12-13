#pragma once
#include <cstdint>
#include <Arduino.h>

namespace Hardware {
namespace RTC {

// RTC Datum/Zeit Struktur
struct RTCDateTime {
    uint16_t year;    // Jahr (2000-2099)
    uint8_t month;    // Monat (1-12)
    uint8_t day;      // Tag (1-31)
    uint8_t hour;     // Stunde (0-23)
    uint8_t minute;   // Minute (0-59)
    uint8_t second;   // Sekunde (0-59)
    uint8_t dayOfWeek; // Wochentag (1-7, 1=Montag)
};

// Mögliche RTC Fehler
enum class RTCError {
    None,
    NotInitialized,
    CommunicationError,
    I2CError,
    InvalidDateTime,
    BatteryError,
    OscillatorError
};

// RTC Konfiguration
struct RTCConfig {
    uint8_t i2cAddress;     // I2C Adresse (default 0x68)
    uint8_t sda;           // SDA Pin
    uint8_t scl;           // SCL Pin
    bool batteryBackup;    // Battery Backup aktiviert
    float tempOffset;      // Temperatur-Offset (optional)
};

// RTC Interface
class RTCDriver {
public:
    virtual ~RTCDriver() = default;
    
    // Lifecycle Management
    virtual RTCError initialize(const RTCConfig& config) = 0;
    virtual bool isInitialized() const = 0;
    virtual void reset() = 0;
    
    // Basis RTC Funktionen
    virtual RTCError getDateTime(RTCDateTime& dateTime) = 0;
    virtual RTCError setDateTime(const RTCDateTime& dateTime) = 0;
    virtual RTCError setFromCompileTime() = 0;  // Setzt Zeit von __DATE__ und __TIME__
    
    // Erweiterte Funktionen
    virtual float getTemperature() = 0;  // Interner Temperatursensor
    virtual bool isRunning() = 0;        // Prüft ob Oszillator läuft
    virtual bool hasLostPower() = 0;     // Prüft Batterie-Status
    
    // Status
    virtual RTCError getLastError() const = 0;
    virtual bool performSelfTest() = 0;
    
    // Hilfsfunktionen für DateTime Konvertierung
    static bool isValidDateTime(const RTCDateTime& dt);
    static String dateTimeToString(const RTCDateTime& dt);
    static uint32_t dateTimeToTimestamp(const RTCDateTime& dt);
};

}} // namespace Hardware::RTC