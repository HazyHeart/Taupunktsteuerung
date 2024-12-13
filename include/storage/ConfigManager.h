// storage/ConfigManager.h
#pragma once

#include <Arduino.h>  // Für uint32_t etc.
#include "../config/SystemConfig.h"  // Für SystemConfig struct

class ConfigManager {
public:
    ConfigManager();
    
    bool begin();
    bool saveConfig(const SystemConfig& config);
    bool loadConfig(SystemConfig& config);
    void resetToDefaults(SystemConfig& config);
    
private:
    static constexpr uint32_t CONFIG_VERSION = 1;
    static constexpr uint32_t CONFIG_MAGIC = 0xC0FFEE42;
    
    // EEPROM Layout
    struct ConfigHeader {
        uint32_t magic;
        uint32_t version;
        uint32_t crc;
    };
    
    bool validateConfig(const SystemConfig& config);
    uint32_t calculateCRC(const SystemConfig& config);
};