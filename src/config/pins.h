// config/pins.h
#pragma once
#include <cstdint>

namespace Pins {
    // I2C Bus 1 (Innen & Display)
    constexpr uint8_t I2C1_SDA = 21;
    constexpr uint8_t I2C1_SCL = 22;
    
    // I2C Bus 2 (Aussen)
    constexpr uint8_t I2C2_SDA = 32;
    constexpr uint8_t I2C2_SCL = 33;
    
    // ESP32 I2C Hardware-Ports
    constexpr uint8_t I2C_PORT0 = 0;  // Standard Wire verwendet Port 0
    constexpr uint8_t I2C_PORT1 = 1;  // Zweiter I2C Port
    
    // BME280 Adresse
    constexpr uint8_t BME280_ADDR = 0x76;
    
    // Relais
    constexpr uint8_t RELAY_FAN_1 = 26;  // Erster Kanal
    constexpr uint8_t RELAY_FAN_2 = 27;  // Zweiter Kanal

    // RTC DS3231
    constexpr uint8_t RTC_SDA = I2C1_SDA;  // Teilt sich Bus 1 mit Display
    constexpr uint8_t RTC_SCL = I2C1_SCL;
    constexpr uint8_t RTC_ADDR = 0x68;     // Standard-Adresse des DS3231

    // SD Card (VSPI statt HSPI)
    constexpr uint8_t SD_CS   = 5;     // Neuer CS Pin
    constexpr uint8_t SD_MOSI = 23;    // VSPI MOSI
    constexpr uint8_t SD_MISO = 18;    // VSPI MISO 
    constexpr uint8_t SD_SCK  = 19;    // VSPI SCK

    // Rotary Encoder
    constexpr uint8_t ENCODER_A = 16;  // CLK
    constexpr uint8_t ENCODER_B = 17;  // DT
    constexpr uint8_t ENCODER_SW = 4;  // Switch

}
