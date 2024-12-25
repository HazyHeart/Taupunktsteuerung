// storage/ConfigManager.cpp
#include "storage/ConfigManager.h"
#include <EEPROM.h>

ConfigManager::ConfigManager() {
}

bool ConfigManager::begin() {
    EEPROM.begin(512);  // Reserviere 512 Bytes
    return true;
}

bool ConfigManager::saveConfig(const SystemConfig& config) {
    ConfigHeader header{
        .magic = CONFIG_MAGIC,
        .version = CONFIG_VERSION,
        .crc = calculateCRC(config)
    };
    
    // Header speichern
    EEPROM.put(0, header);
    
    // Konfiguration speichern
    EEPROM.put(sizeof(header), config);
    
    return EEPROM.commit();
}

bool ConfigManager::loadConfig(SystemConfig& config) {
    ConfigHeader header;
    EEPROM.get(0, header);
    
    // Header prüfen
    if (header.magic != CONFIG_MAGIC || 
        header.version != CONFIG_VERSION) {
        return false;
    }
    
    // Konfiguration laden
    EEPROM.get(sizeof(header), config);
    
    // CRC prüfen
    if (header.crc != calculateCRC(config)) {
        return false;
    }
    
    return validateConfig(config);
}

void ConfigManager::resetToDefaults(SystemConfig& config) {
    // Operating defaults
    config.operating.minTemp = 5.0f;
    config.operating.maxHumidity = 70.0f;
    config.operating.dewPointDiff = 2.0f;
    
    // Fan defaults
    config.fan.minRuntime = 600000;   // 10 Minuten
    config.fan.maxRuntime = 3600000;  // 60 Minuten
    config.fan.minPause = 300000;     // 5 Minuten
    
    // Display defaults
    config.display.contrast = 255;
    config.display.timeout = 30;
    config.display.flipScreen = false;
    
    // Sensor defaults
    config.sensors.readInterval = 1000;
    config.sensors.samples = 5;
    config.sensors.tempOffset[0] = 0;
    config.sensors.tempOffset[1] = 0;
}

bool ConfigManager::validateConfig(const SystemConfig& config) {
    // Einfache Plausibilitätsprüfungen
    if (config.operating.minTemp < -20.0f || config.operating.minTemp > 30.0f) return false;
    if (config.operating.maxHumidity < 0.0f || config.operating.maxHumidity > 100.0f) return false;
    if (config.operating.dewPointDiff < 0.0f || config.operating.dewPointDiff > 10.0f) return false;
    
    if (config.fan.minRuntime > config.fan.maxRuntime) return false;
    if (config.fan.maxRuntime > 7200000) return false;  // Max 2 Stunden
    
    if (config.display.timeout == 0 || config.display.timeout > 600) return false;
    if (config.sensors.samples == 0 || config.sensors.samples > 100) return false;
    
    return true;
}

uint32_t ConfigManager::calculateCRC(const SystemConfig& config) {
    // Einfache CRC Berechnung
    const uint8_t* data = reinterpret_cast<const uint8_t*>(&config);
    uint32_t crc = 0;
    
    for (size_t i = 0; i < sizeof(SystemConfig); i++) {
        crc = crc ^ (data[i] << 24);
        for (int j = 0; j < 8; j++) {
            if (crc & 0x80000000) {
                crc = (crc << 1) ^ 0x04C11DB7;
            } else {
                crc = (crc << 1);
            }
        }
    }
    
    return crc;
}
