#ifndef BME280DRIVER_H
#define BME280DRIVER_H

#pragma once

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_BME280.h>
#include <CircularBuffer.hpp>
#include <memory>
#include "config/constants.h"
#include "hardware/i2c/I2CManager.h"

namespace Hardware {
namespace Sensors {

struct SensorData {
    float temperature;
    float humidity;
    float pressure;
    float dewPoint;
    uint32_t timestamp;
    bool valid;
};

class BME280Driver {
public:
    BME280Driver(const String& loc, TwoWire& w);
    
    bool begin(uint8_t addr);
    static bool init(uint8_t address);
    static float readTemperature();
    bool update();
    
    const SensorData& getData() const { return currentData; }
    bool isInitialized() const { return initialized; }
    const String& getLocation() const { return location; }
    bool performSelfTest();

private:
    static Adafruit_BME280 bme; // Statische Klassenvariable
    TwoWire& wire;
    String location;
    bool initialized;
    uint32_t lastRead;
    SensorData currentData;
    
    // Puffer für Mittelwertbildung
    CircularBuffer<float, SystemConstants::SENSOR_SAMPLES> tempBuffer;
    CircularBuffer<float, SystemConstants::SENSOR_SAMPLES> humidBuffer;
    CircularBuffer<float, SystemConstants::SENSOR_SAMPLES> pressBuffer;
    
    float calculateDewPoint(float temp, float humidity);
    
    // Template-Funktion im Header definieren
    float calculateAverage(const CircularBuffer<float, SystemConstants::SENSOR_SAMPLES>& buffer) {
        if (buffer.isEmpty()) return 0;
        
        float sum = 0;
        for (size_t i = 0; i < buffer.size(); i++) {
            sum += buffer[i];
        }
        return sum / static_cast<float>(buffer.size());
    }
};

}} // namespace Hardware::Sensors
#endif // BME280DRIVER_H