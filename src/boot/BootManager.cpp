// boot/BootManager.cpp
#include "boot/BootManager.h"
#include <Wire.h>
#include "config/pins.h"
#include "config/constants.h"
#include "utils/I2CScanner.h"
#include "hardware/i2c/I2CManager.h"

// Constructor
BootManager::BootManager(
    Hardware::Sensors::BME280Driver& sensorIn,
    Hardware::Sensors::BME280Driver& sensorOut,
    Hardware::Relay::SingleRelayDriver& relays,
    Hardware::Display::SH1106Driver& display,
    Hardware::RTC::DS3231Driver& rtcDriver)
    : sensorInnen(sensorIn)
    , sensorAussen(sensorOut)
    , relayManager(relays)
    , displayDriver(display)
    , rtc(rtcDriver)
    , bootComplete(false)
    , bootAttempts(0)
    , bootStartTime(0)
    , recoveryMode(false)
{
    uiManager = std::make_unique<UI::UIManager>(display);
    bootScreen = std::make_unique<UI::Screens::BootScreen>();
}

// Hauptstart-Sequenz
bool BootManager::startupSequence() {
    bootStartTime = millis();
    bootAttempts++;
    
    Serial.println("\n=== Starting Boot Sequence ===");
    
    // UI für Boot initialisieren
    uiManager->registerScreen("boot", bootScreen.get());
    uiManager->activateScreen("boot");
    bootScreen->setStatus("Starte...");
    bootScreen->setProgress(0);
    uiManager->update();  // -> statt .
    
    // Pre-Boot Phase
    bootScreen->setStatus("Hardware-Test...");
    bootScreen->setProgress(20);
    uiManager->update();  // -> statt .
    
    if (!initializePreBoot()) {
        return handleBootFailure("Pre-boot failed");
    }
    
    bootScreen->setStatus("Initialisiere Hardware...");
    bootScreen->setProgress(40);
    uiManager->update();  // -> statt .
    
    if (!initializeHardware()) {
        return handleBootFailure("Hardware init failed");
    }

    bootScreen->setStatus("Prüfe Sensoren...");
    bootScreen->setProgress(60);
    uiManager->update();  // -> statt .
    
    if (!initializeSensors()) {
        return handleBootFailure("Sensor init failed");
    }
    
    bootScreen->setStatus("Finalisiere...");
    bootScreen->setProgress(80);
    uiManager->update();  // -> statt .
    
    if (!finalizeSystem()) {
        return handleBootFailure("System finalization failed");
    }
    
    // Boot abgeschlossen
    bootScreen->setStatus("Boot erfolgreich!");
    bootScreen->setProgress(100);
    uiManager->update();
    delay(1000);

    bootComplete = true;
    Serial.printf("\n=== Boot Complete ===\nDuration: %lums\n", getBootDuration());
    logSystemStatus();
    return true;
}

// Pre-Boot Initialisierung
bool BootManager::initializePreBoot() {
    if (!testMemorySystem()) {
        lastError = "Memory check failed";
        return false;
    }
   
    if (!watchdog.begin()) {
        lastError = "Watchdog initialization failed";
        return false;
    }
   
    return true;
}

bool BootManager::initializeHardware() {
    // Zuerst I2C initialisieren
    if (!Hardware::I2C::I2CManager::initialize()) {
        lastError = "I2C Initialisierung fehlgeschlagen";
        return false;
    }
    
    delay(100);  // Zeit zur Stabilisierung
    
    // Display initialisieren
    Serial.println("Display initialisieren...");
    Hardware::Display::DisplayConfig displayConfig{
        .i2cAddress = 0x3C,
        .width = 128,
        .height = 64,
        .sda = Pins::I2C1_SDA,
        .scl = Pins::I2C1_SCL
    };
    
    auto error = displayDriver.initialize(displayConfig);
    if (error != Hardware::Display::DisplayError::None) {
        lastError = "Display Initialisierung fehlgeschlagen";
        return false;
    }

    // I2C Geräte testen
    if (!testI2CDevices()) {
        lastError = "I2C device check failed";
        return false;
    }
    
    return true;
}

bool BootManager::testMemorySystem() {
    uint32_t freeHeap = ESP.getFreeHeap();
    uint32_t minHeap = SystemConstants::MIN_FREE_HEAP;  // Geändert von SystemConfig zu SystemConstants
   
    Serial.println("Memory Status:");
    Serial.printf("- Free Heap: %d bytes\n", freeHeap);
    Serial.printf("- Minimum Required: %d bytes\n", minHeap);
    Serial.printf("- Largest Free Block: %d bytes\n", ESP.getMaxAllocHeap());
   
    return freeHeap >= minHeap;
}

bool BootManager::initializeSensors() {
    Serial.println("\nPrüfe Sensoren:");
    bootScreen->setStatus("Sensoren werden initialisiert...");
    uiManager->update();
    
    // Innensensor
    Serial.println("\nPrüfe Innensensor:");
    if (!sensorInnen.begin(Pins::BME280_ADDR)) {
        lastError = "Innensensor nicht gefunden";
        return false;
    }
    if (!sensorInnen.performSelfTest()) {
        lastError = "Innensensor Selbsttest fehlgeschlagen";
        return false;
    }
    
    // Außensensor
    Serial.println("\nPrüfe Außensensor:");
    if (!sensorAussen.begin(Pins::BME280_ADDR)) {
        lastError = "Außensensor nicht gefunden";
        return false;
    }
    if (!sensorAussen.performSelfTest()) {
        lastError = "Außensensor Selbsttest fehlgeschlagen";
        return false;
    }
    
    // Relais konfigurieren
    Hardware::Relay::RelayConfig relayConfig{
        .pin = Pins::RELAY_FAN_1,
        .invertLogic = false,
        .minSwitchInterval = 1000,  // 1 Sekunde Mindestabstand
        .description = "Lüfter Relais"
    };
   
    // Relais initialisieren
    if (relayManager.initialize(relayConfig) != Hardware::Relay::RelayError::None) {
        lastError = "Relay initialization failed";
        return false;
    }
    
    return true;
}

bool BootManager::finalizeSystem() {
    // RTC prüfen
    if (!testRTC()) {
        lastError = "RTC check failed";
        return false;
    }
   
    // Relais Konfiguration erstellen
    Hardware::Relay::RelayConfig relayConfig{
        .pin = Pins::RELAY_FAN_1,
        .invertLogic = false,
        .minSwitchInterval = 1000,  // 1 Sekunde Mindestabstand
        .description = "Fan Relay"
    };
   
    // Relais initialisieren
    if (relayManager.initialize(relayConfig) != Hardware::Relay::RelayError::None) {
        lastError = "Relay initialization failed";
        return false;
    }
   
    return true;
}

bool BootManager::testRTC() {
    Serial.println("\nPrüfe RTC...");
    
    Hardware::RTC::RTCConfig rtcConfig{
        .i2cAddress = Pins::RTC_ADDR,  // 0x68
        .sda = Pins::I2C1_SDA,
        .scl = Pins::I2C1_SCL,
        .batteryBackup = true,
        .tempOffset = 0.0f
    };

    if (rtc.initialize(rtcConfig) != Hardware::RTC::RTCError::None) {
        lastError = "RTC Initialisierung fehlgeschlagen";
        return false;
    }

    // Power-Loss Check
    if (rtc.hasLostPower()) {
        Serial.println("RTC hat Stromversorgung verloren - setze Zeit");
        rtc.setFromCompileTime();
    }

    return true;
}

bool BootManager::handleBootFailure(const char* reason) {
    String error = String(reason) + ": " + lastError;
    Serial.printf("\n!!! Boot Failed !!!\nReason: %s\n", error.c_str());
    
    bootScreen->setStatus("Boot Fehler: " + error);
    bootScreen->setProgress(0);
    uiManager->update();
   
    if (bootAttempts >= MAX_BOOT_ATTEMPTS) {
        bootScreen->setStatus("Max Boot Versuche erreicht!");
        uiManager->update();
        Serial.println("Maximum boot attempts reached!");
        Serial.println("System needs manual intervention!");
        return false;
    }
    
    return startupSequence();
}

void BootManager::logSystemStatus() {
    Serial.println("\n=== System Status ===");
    Serial.printf("Boot Duration: %lums\n", getBootDuration());
    Serial.printf("Free Heap: %d bytes\n", ESP.getFreeHeap());
    Serial.printf("CPU Frequency: %dMHz\n", ESP.getCpuFreqMHz());
    Serial.printf("Flash Size: %d bytes\n", ESP.getFlashChipSize());
    Serial.printf("SDK Version: %s\n", ESP.getSdkVersion());
}

uint32_t BootManager::getBootDuration() const {
    return millis() - bootStartTime;
}

void BootManager::emergencyShutdown() {
    Serial.println("\n!!! EMERGENCY SHUTDOWN !!!");
    relayManager.reset();
    
    if (bootScreen) {
        bootScreen->setStatus("EMERGENCY SHUTDOWN");
        bootScreen->setProgress(0);
        uiManager->update();
    }
    
    delay(1000);
    ESP.restart();
}

const char* BootManager::getPhaseString(BootPhase phase) {
    switch (phase) {
        case BootPhase::PRE_BOOT:        return "Pre-Boot";
        case BootPhase::HARDWARE_INIT:    return "Hardware Init";
        case BootPhase::CONFIG_LOAD:      return "Config Load";
        case BootPhase::PERIPHERAL_INIT:  return "Peripheral Init";
        case BootPhase::SENSOR_INIT:      return "Sensor Init";
        case BootPhase::SYSTEM_FINALIZE:  return "System Finalize";
        default:                          return "Unknown";
    }
}

void BootManager::debugI2CBus(TwoWire& wire, const char* name) {
    Serial.printf("\nScanning I2C bus %s...\n", name);
    
    int devices = 0;
    uint8_t error;
    
    for (uint8_t address = 1; address < 127; address++) {
        // Sende nur ein Stop-Bit um den Device-Status zu prüfen
        wire.beginTransmission(address);
        error = wire.endTransmission();

        if (error == 0) {
            devices++;
            Serial.printf("Found device at 0x%02X: ", address);
            
            // Bekannte Geräte identifizieren
            switch(address) {
                case 0x3C:
                    Serial.println("SH1106 Display");
                    break;
                case 0x76:
                    Serial.println("BME280 Sensor");
                    break;
                case 0x68:
                    Serial.println("RTC");
                    break;
                default:
                    Serial.println("Unknown device");
            }
            
            delay(10);  // Kurze Pause zwischen gefundenen Geräten
        }
    }
    
    if (devices == 0) {
        Serial.printf("No I2C devices found on bus %s\n", name);
    } else {
        Serial.printf("Found %d device(s) on bus %s\n", devices, name);
    }
}

bool BootManager::testI2CDevices() {
    Serial.println("\nStarting I2C device test...");
    
    auto& internalBus = Hardware::I2C::I2CManager::getInternalBus();
    auto& externalBus = Hardware::I2C::I2CManager::getExternalBus();
    
    // Display wurde bereits erfolgreich initialisiert,
    // also überspringen wir den Test
    bool display_found = true;  // Da wir wissen dass es funktioniert
    
    Serial.println("Teste BME280 Innen...");
    bool bme280_innen_found = Hardware::I2C::I2CManager::deviceResponds(internalBus, Pins::BME280_ADDR);
    delay(200);  // Längere Pause zwischen Tests
    
    Serial.println("Teste BME280 Außen...");
    bool bme280_aussen_found = Hardware::I2C::I2CManager::deviceResponds(externalBus, Pins::BME280_ADDR);
    
    // Debug Ausgaben
    Serial.println("\nTest Ergebnisse:");
    Serial.printf("- Display: %s\n", display_found ? "OK" : "Nicht gefunden");
    Serial.printf("- BME280 Innen: %s\n", bme280_innen_found ? "OK" : "Nicht gefunden");
    Serial.printf("- BME280 Außen: %s\n", bme280_aussen_found ? "OK" : "Nicht gefunden");
    
    return display_found && bme280_innen_found && bme280_aussen_found;
}

bool BootManager::testDisplay() {
    displayDriver.clear();
    displayDriver.updateDisplay();  // Geändert von display() zu updateDisplay()
    return true;
}

bool BootManager::initializeConfig() {
    bootScreen->setStatus("Lade Konfiguration...");
    uiManager->update();
    
    configManager = std::make_unique<ConfigManager>();
    if (!configManager->begin()) {
        Serial.println("Config Manager Init fehlgeschlagen!");
        lastError = "Config Init Error";
        return false;
    }

    return loadConfiguration();
}

bool BootManager::loadConfiguration() {
    // Versuche Konfiguration zu laden
    if (!configManager->loadConfig(config)) {
        Serial.println("Keine gültige Konfiguration gefunden!");
        bootScreen->setStatus("Verwende Defaults...");
        uiManager->update();
        
        configManager->resetToDefaults(config);
        
        // Speichere Default-Konfiguration
        if (!configManager->saveConfig(config)) {
            Serial.println("Fehler beim Speichern der Default-Konfiguration!");
            lastError = "Config Save Error";
            return false;
        }
    }

    return true;
}

bool BootManager::checkRecoveryMode() {
    // Recovery Mode wenn RTC Power verloren hat und mehrere Boot-Versuche
    recoveryMode = rtc.hasLostPower() && bootAttempts > 2;
    
    if (recoveryMode) {
        enterRecoveryMode();
        return true;
    }
    return false;
}

void BootManager::enterRecoveryMode() {
    Serial.println("Entering Recovery Mode!");
    bootScreen->setStatus("Recovery Mode");
    bootScreen->setProgress(0);
    uiManager->update();
    
    // Grundlegende Hardware initialisieren
    if (!initializeHardware()) {
        lastError = "Hardware Init Failed";
        return;
    }

    // Defaults laden
    configManager->resetToDefaults(config);
    
    // Warte auf Benutzerinteraktion oder timeout
    // TODO: Implementiere Recovery UI
    delay(5000);
}