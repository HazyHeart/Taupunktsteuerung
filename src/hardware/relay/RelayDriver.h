#pragma once
#include <cstdint>

namespace Hardware {
namespace Relay {

// Mögliche Relay-Fehler
enum class RelayError {
    None,
    NotInitialized,
    InvalidPin,
    SwitchingError,
    OverloadProtection
};

// Konfiguration für einen Relay-Kanal
struct RelayConfig {
    uint8_t pin;                    // GPIO Pin
    bool invertLogic{false};        // true = LOW ist aktiv
    uint32_t minSwitchInterval{0};  // Minimale Zeit zwischen Schaltvorgängen
    const char* description{""};    // Beschreibung für Debug
};

// Interface für Relay-Treiber
class RelayDriver {
public:
    virtual ~RelayDriver() = default;
    
    // Lifecycle
    virtual RelayError initialize(const RelayConfig& config) = 0;
    virtual bool isInitialized() const = 0;
    virtual void reset() = 0;
    
    // Grundfunktionen
    virtual RelayError switchOn() = 0;
    virtual RelayError switchOff() = 0;
    virtual bool getState() const = 0;
    
    // Status & Test
    virtual RelayError getLastError() const = 0;
    virtual bool performSelfTest() = 0;
    
    // Timing & Schutz
    virtual uint32_t getOnTime() const = 0;     // Zeit seit Einschalten
    virtual uint32_t getOffTime() const = 0;    // Zeit seit Ausschalten
    virtual bool canSwitch() const = 0;         // Prüft Schaltsperre
};

}} // namespace Hardware::Relay
