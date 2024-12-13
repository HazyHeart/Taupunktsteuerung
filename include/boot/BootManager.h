// boot/BootManager.h
#pragma once
#include <memory>
#include "config/SystemConfig.h"
#include "storage/ConfigManager.h"
#include "boot/SystemCheck.h"
#include "boot/WatchdogManager.h"
#include "hardware/sensors/BME280Driver.h"
#include "hardware/relay/SingleRelayDriver.h"
#include "hardware/display/SH1106Driver.h"
#include "hardware/rtc/DS3231Driver.h"
#include "ui/UIManager.h"
#include "ui/screens/BootScreen.h"

enum class BootPhase {
    PRE_BOOT,
    HARDWARE_INIT,
    CONFIG_LOAD,
    PERIPHERAL_INIT,
    SENSOR_INIT,
    SYSTEM_FINALIZE
};

class BootManager {
private:
    // Member-Variablen
    Hardware::Sensors::BME280Driver& sensorInnen;
    Hardware::Sensors::BME280Driver& sensorAussen;
    Hardware::Relay::SingleRelayDriver& relayManager;
    Hardware::Display::SH1106Driver& displayDriver;
    Hardware::RTC::DS3231Driver& rtc;
    std::unique_ptr<UI::UIManager> uiManager;
    std::unique_ptr<UI::Screens::BootScreen> bootScreen;
    std::unique_ptr<ConfigManager> configManager;
    WatchdogManager watchdog;
    String lastError;
    SystemConfig config;

    bool bootComplete;
    uint8_t bootAttempts;
    uint32_t bootStartTime;
    bool recoveryMode;

public:
    // Konstanten
    static constexpr uint8_t MAX_BOOT_ATTEMPTS = 3;

    BootManager(Hardware::Sensors::BME280Driver& sensorIn,
                Hardware::Sensors::BME280Driver& sensorOut,
                Hardware::Relay::SingleRelayDriver& relays,
                Hardware::Display::SH1106Driver& display,
                Hardware::RTC::DS3231Driver& rtcDriver);
    
    bool startupSequence();
    void emergencyShutdown();
    bool isBootComplete() const { return bootComplete; }
    const String& getLastError() const { return lastError; }
    static const char* getPhaseString(BootPhase phase);

private:
    // Test-Methoden
    bool testMemorySystem();
    bool testI2CDevices();
    bool testDisplay();
    bool testRTC();

    // Boot-Sequenz Methoden
    bool initializePreBoot();
    bool initializeHardware();
    bool initializeSensors();
    bool finalizeSystem();
    bool handleBootFailure(const char* reason);
    bool initializeConfig();
    bool loadConfiguration();
    bool checkRecoveryMode();
    void enterRecoveryMode();

    // Utility Methoden
    void logSystemStatus();
    uint32_t getBootDuration() const;
    void debugI2CBus(TwoWire& wire, const char* name);
};