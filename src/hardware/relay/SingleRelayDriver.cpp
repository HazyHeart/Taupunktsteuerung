#include "hardware/relay/SingleRelayDriver.h"

namespace Hardware {
namespace Relay {

SingleRelayDriver::SingleRelayDriver()
    : initialized(false)
    , lastError(RelayError::None)
    , relayPin(0)
    , invertedLogic(false)
    , minSwitchInterval(0)
    , currentState(false)
    , lastSwitchTime(0)
    , switchOnTime(0)
    , switchOffTime(0)
{
}

RelayError SingleRelayDriver::initialize(const RelayConfig& config) {
    if (initialized) {
        return RelayError::None;
    }
    
    Serial.printf("\nInitialisiere Relais (%s):\n", config.description);
    Serial.printf("- Pin: %d\n", config.pin);
    Serial.printf("- Invertierte Logik: %s\n", config.invertLogic ? "ja" : "nein");
    
    // Pin konfigurieren
    relayPin = config.pin;
    invertedLogic = config.invertLogic;
    minSwitchInterval = config.minSwitchInterval;
    description = String(config.description);
    
    pinMode(relayPin, OUTPUT);
    
    // Initialen Zustand setzen (aus)
    writePin(false);
    currentState = false;
    switchOffTime = millis();
    
    initialized = true;
    setError(RelayError::None);
    
    Serial.println("- Initialisierung erfolgreich\n");
    return RelayError::None;
}

void SingleRelayDriver::reset() {
    if (!initialized) return;
    
    // Sicheres Ausschalten
    writePin(false);
    currentState = false;
    switchOffTime = millis();
    
    Serial.printf("Relay (%s) wurde zurückgesetzt\n", description.c_str());
}

RelayError SingleRelayDriver::switchOn() {
    if (!initialized) {
        setError(RelayError::NotInitialized);
        return lastError;
    }
    
    if (!canSwitch()) {
        Serial.printf("Relay (%s): Zu schnelles Schalten verhindert\n", description.c_str());
        return RelayError::None; // Kein Fehler, nur Schutz
    }
    
    Serial.printf("Relay (%s): Einschalten...\n", description.c_str());
    
    if (!writePin(true)) {
        setError(RelayError::SwitchingError);
        return lastError;
    }
    
    currentState = true;
    lastSwitchTime = millis();
    switchOnTime = lastSwitchTime;
    
    return RelayError::None;
}

RelayError SingleRelayDriver::switchOff() {
    if (!initialized) {
        setError(RelayError::NotInitialized);
        return lastError;
    }
    
    if (!canSwitch()) {
        Serial.printf("Relay (%s): Zu schnelles Schalten verhindert\n", description.c_str());
        return RelayError::None; // Kein Fehler, nur Schutz
    }
    
    Serial.printf("Relay (%s): Ausschalten...\n", description.c_str());
    
    if (!writePin(false)) {
        setError(RelayError::SwitchingError);
        return lastError;
    }
    
    currentState = false;
    lastSwitchTime = millis();
    switchOffTime = lastSwitchTime;
    
    return RelayError::None;
}

bool SingleRelayDriver::performSelfTest() {
    if (!initialized) {
        Serial.printf("Relay (%s): Nicht initialisiert!\n", description.c_str());
        return false;
    }
    
    Serial.printf("\nRelay (%s) Selbsttest:\n", description.c_str());
    
    // Kurzer Schalttest
    Serial.println("- Schalte kurz ein...");
    if (switchOn() != RelayError::None) return false;
    delay(500);
    
    Serial.println("- Schalte aus...");
    if (switchOff() != RelayError::None) return false;
    
    // Schutzintervall testen wenn konfiguriert
    if (minSwitchInterval > 0) {
        Serial.println("- Teste Schaltsperre...");
        if (canSwitch()) {
            Serial.println("  Fehler: Schaltsperre nicht aktiv!");
            return false;
        }
        Serial.println("  Schaltsperre aktiv (OK)");
    }
    
    Serial.println("- Selbsttest erfolgreich\n");
    return true;
}

uint32_t SingleRelayDriver::getOnTime() const {
    if (!currentState) return 0;
    return millis() - switchOnTime;
}

uint32_t SingleRelayDriver::getOffTime() const {
    if (currentState) return 0;
    return millis() - switchOffTime;
}

bool SingleRelayDriver::canSwitch() const {
    if (!initialized) return false;
    
    // Prüfen ob Mindestzeit seit letztem Schaltvorgang vergangen
    if (minSwitchInterval > 0 && 
        (millis() - lastSwitchTime) < minSwitchInterval) {
        return false;
    }
    
    return true;
}

void SingleRelayDriver::setError(RelayError error) {
    lastError = error;
    if (error != RelayError::None) {
        Serial.printf("Relay (%s) Error: %d\n", description.c_str(), static_cast<int>(error));
    }
}

bool SingleRelayDriver::writePin(bool state) {
    if (!initialized) return false;
    
    // Logik invertieren wenn konfiguriert
    bool pinState = invertedLogic ? !state : state;
    digitalWrite(relayPin, pinState ? HIGH : LOW);
    
    // Verifizieren des Pin-Zustands
    bool readState = digitalRead(relayPin) == HIGH;
    return (pinState == readState);
}

}} // namespace Hardware::Relay