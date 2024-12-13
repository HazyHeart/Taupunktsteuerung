// hardware/sensors/BME280Driver.cpp
#include "hardware/sensors/BME280Driver.h"
#include "config/constants.h"
#include <Arduino.h>
#include <memory>

namespace Hardware {
namespace Sensors {

BME280Driver::BME280Driver(const String& loc, TwoWire& w) 
    : wire(w)
    , location(loc)
    , initialized(false)
    , lastRead(0) 
{
    currentData.valid = false;
}

bool BME280Driver::begin(uint8_t addr) {
    if (!initialized) {
        Serial.printf("\nInitialisiere BME280 (%s):\n", location.c_str());
        Serial.printf("- Adresse: 0x%02X\n", addr);

        // Bus für den Sensor konfigurieren
        auto& wire = (location == "Aussen") ? 
            Hardware::I2C::I2CManager::getExternalBus() : 
            Hardware::I2C::I2CManager::getInternalBus();
            
        uint8_t sda = (location == "Aussen") ? Pins::I2C2_SDA : Pins::I2C1_SDA;
        uint8_t scl = (location == "Aussen") ? Pins::I2C2_SCL : Pins::I2C1_SCL;

        // Bus konfigurieren
        wire.begin(sda, scl);
        wire.setClock(100000);
        delay(50);

        // Chip ID lesen
        wire.beginTransmission(addr);
        wire.write(0xD0);  // BME280 Chip ID Register
        uint8_t error = wire.endTransmission();
        
        if (error == 0) {
            wire.requestFrom(addr, (uint8_t)1);
            if (wire.available()) {
                uint8_t chipId = wire.read();
                Serial.printf("- Chip ID: 0x%02X\n", chipId);
                if (chipId == 0x60) {
                    // BME280 mit aktueller Wire-Instanz initialisieren
                    if (bme.begin(addr, &wire)) {
                        initialized = true;
                        Serial.println("BME280 erfolgreich initialisiert");
                        return true;
                    }
                }
            }
        }
        
        Serial.println("BME280 Initialisierung fehlgeschlagen");
    }
    return initialized;
}

bool BME280Driver::update() {
    if (!initialized) {
        currentData.valid = false;
        return false;
    }
    
    uint32_t now = millis();
    if (now - lastRead < SystemConstants::SENSOR_READ_INTERVAL) {
        return currentData.valid;
    }
    
    // Neue Messwerte einlesen
    float temp = bme.readTemperature();
    float hum = bme.readHumidity();
    float press = bme.readPressure();
    
    // Plausibilitätsprüfung
    if (isnan(temp) || isnan(hum) || isnan(press)) {
        currentData.valid = false;
        Serial.printf("BME280 %s: Fehlerhafte Messwerte\n", location.c_str());
        return false;
    }
    
    // In Puffer aufnehmen
    tempBuffer.push(temp);
    humidBuffer.push(hum);
    pressBuffer.push(press);
    
    // Mittelwerte berechnen
    currentData.temperature = calculateAverage(tempBuffer);
    currentData.humidity = calculateAverage(humidBuffer);
    currentData.pressure = calculateAverage(pressBuffer);
    currentData.dewPoint = calculateDewPoint(currentData.temperature, currentData.humidity);
    currentData.timestamp = now;
    currentData.valid = true;
    
    lastRead = now;
    return true;
}

float BME280Driver::calculateDewPoint(float temp, float humidity) {
    // Magnus-Formel für Taupunktberechnung
    float alpha = ((17.27 * temp) / (237.7 + temp)) + log(humidity/100.0);
    return (237.7 * alpha) / (17.27 - alpha);
}

bool BME280Driver::performSelfTest() {
    if (!initialized) {
        Serial.printf("BME280 %s: Nicht initialisiert!\n", location.c_str());
        return false;
    }
    
    // Testwerte lesen
    float temp = bme.readTemperature();
    float humidity = bme.readHumidity();
    float pressure = bme.readPressure();
    
    // Auf gültige Werte prüfen
    if (isnan(temp) || isnan(humidity) || isnan(pressure)) {
        Serial.printf("BME280 %s: Sensor liefert ungültige Werte\n", location.c_str());
        return false;
    }
    
    // Plausibilitätsprüfung
    bool tempOk = (temp >= -40.0f && temp <= 85.0f);  // Spezifikationsbereich BME280
    bool humidityOk = (humidity >= 0.0f && humidity <= 100.0f);
    bool pressureOk = (pressure >= 30000.0f && pressure <= 110000.0f); // 300-1100 hPa
    
    Serial.printf("\nBME280 %s Selbsttest:\n", location.c_str());
    Serial.printf("- Temperatur: %.1f°C [%s]\n", temp, tempOk ? "OK" : "FEHLER");
    Serial.printf("- Luftfeuchte: %.1f%% [%s]\n", humidity, humidityOk ? "OK" : "FEHLER");
    Serial.printf("- Luftdruck: %.0fhPa [%s]\n", pressure/100.0f, pressureOk ? "OK" : "FEHLER");
    
    return tempOk && humidityOk && pressureOk;
}

}} // namespace Hardware::Sensors