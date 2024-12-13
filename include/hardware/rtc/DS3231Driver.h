#pragma once
#include "RTCDriver.h"
#include <Wire.h>
#include "hardware/i2c/I2CManager.h"

namespace Hardware {
namespace RTC {

class DS3231Driver : public RTCDriver {
public:
    DS3231Driver();
    ~DS3231Driver() override = default;
    
    // RTCDriver Interface Implementation
    RTCError initialize(const RTCConfig& config) override;
    bool isInitialized() const override { return initialized; }
    void reset() override;
    
    RTCError getDateTime(RTCDateTime& dateTime) override;
    RTCError setDateTime(const RTCDateTime& dateTime) override;
    RTCError setFromCompileTime() override;
    
    float getTemperature() override;
    bool isRunning() override;
    bool hasLostPower() override;
    
    RTCError getLastError() const override { return lastError; }
    bool performSelfTest() override;

protected:
    bool initialized;
    RTCError lastError;
    TwoWire* wire;
    uint8_t address;
    uint8_t sdaPin;
    uint8_t sclPin;
    
    // Register-Map des DS3231
    static constexpr uint8_t RTC_SECONDS      = 0x00;
    static constexpr uint8_t RTC_MINUTES      = 0x01;
    static constexpr uint8_t RTC_HOURS        = 0x02;
    static constexpr uint8_t RTC_DAY          = 0x03;
    static constexpr uint8_t RTC_DATE         = 0x04;
    static constexpr uint8_t RTC_MONTH        = 0x05;
    static constexpr uint8_t RTC_YEAR         = 0x06;
    static constexpr uint8_t RTC_CONTROL      = 0x0E;
    static constexpr uint8_t RTC_STATUS       = 0x0F;
    static constexpr uint8_t TEMP_MSB         = 0x11;
    static constexpr uint8_t TEMP_LSB         = 0x12;

protected:
    // Private Hilfsmethoden für I2C-Kommunikation
    bool resetI2CBus();
    RTCError readRegister(uint8_t reg, uint8_t& value);
    RTCError writeRegister(uint8_t reg, uint8_t value);
    RTCError burstRead(uint8_t reg, uint8_t* buffer, uint8_t length);
    RTCError burstWrite(uint8_t reg, const uint8_t* buffer, uint8_t length);
    
    // BCD Konvertierung
    uint8_t bcdToDec(uint8_t bcd);
    uint8_t decToBcd(uint8_t dec);
    
    // Error Handling
    void setError(RTCError error);
};

}} // namespace Hardware::RTC