#include "hardware/rtc/DS3231Driver.h"
#include "config/pins.h"
#include <Arduino.h>

namespace Hardware {
namespace RTC {

DS3231Driver::DS3231Driver() 
    : initialized(false)
    , lastError(RTCError::None)
    , wire(nullptr)
    , address(0)
    , sdaPin(0)
    , sclPin(0)
{
}

RTCError DS3231Driver::initialize(const RTCConfig& config) {
    if (initialized) {
        return RTCError::None;
    }

    Serial.println("\nInitialisiere DS3231:");
    Serial.printf("- I2C Adresse: 0x%02X\n", config.i2cAddress);
    
    // Interne Bus-Referenz holen
    TwoWire& busRef = Hardware::I2C::I2CManager::getInternalBus();
    
    // Bus neu konfigurieren
    busRef.end();
    delay(50);
    busRef.begin(config.sda, config.scl);
    busRef.setClock(10000);  // Sehr langsam für Stabilität
    delay(100);

    // Ping Test
    busRef.beginTransmission(config.i2cAddress);
    uint8_t error = busRef.endTransmission();
    
    if (error != 0) {
        Serial.printf("- Kommunikationstest fehlgeschlagen (Error: %d)\n", error);
        return RTCError::CommunicationError;
    }
    
    // Minimaler Funktionstest
    bool success = false;
    for(int retry = 0; retry < 3; retry++) {
        busRef.beginTransmission(config.i2cAddress);
        busRef.write(0x0F);  // Status Register
        error = busRef.endTransmission();
        
        if (error == 0) {
            if (busRef.requestFrom(config.i2cAddress, (uint8_t)1) == 1) {
                uint8_t status = busRef.read();
                Serial.printf("- Status Register: 0x%02X\n", status);
                success = true;
                break;
            }
        }
        delay(50);
    }
    
    if (!success) {
        Serial.println("- Register-Test fehlgeschlagen");
        return RTCError::CommunicationError;
    }

    // Interne Konfiguration speichern
    wire = &busRef;  // Wire-Referenz speichern
    address = config.i2cAddress;
    initialized = true;
    
    Serial.println("- Initialisierung erfolgreich");
    return RTCError::None;
}

bool DS3231Driver::resetI2CBus() {
    wire->end();  // Bus freigeben
    delay(100);   // Zeit zum Reset
    
    // Pins als Output setzen und Clock-Cycle generieren
    pinMode(sdaPin, OUTPUT);
    pinMode(sclPin, OUTPUT);
    
    // SDA high und 9 Clock-Cycles
    digitalWrite(sdaPin, HIGH);
    for(int i = 0; i < 9; i++) {
        digitalWrite(sclPin, HIGH);
        delayMicroseconds(5);
        digitalWrite(sclPin, LOW);
        delayMicroseconds(5);
    }
    
    // Stop-Condition
    digitalWrite(sdaPin, LOW);
    delayMicroseconds(5);
    digitalWrite(sclPin, HIGH);
    delayMicroseconds(2);
    digitalWrite(sdaPin, HIGH);
    delayMicroseconds(2);
    
    // I2C-Bus neu starten
    wire->begin(sdaPin, sclPin);
    wire->setClock(100000);  // 100kHz für Stabilität
    
    return true;
}

void DS3231Driver::reset() {
    if (!initialized) return;
    
    // Sanfter Reset ohne Zeitverlust
    uint8_t control = 0x00;  // Alle Flags zurücksetzen
    if (writeRegister(RTC_CONTROL, control) == RTCError::None) {
        // Status-Register bereinigen
        writeRegister(RTC_STATUS, 0x00);
    }
}

RTCError DS3231Driver::getDateTime(RTCDateTime& dateTime) {
    if (!initialized) {
        setError(RTCError::NotInitialized);
        return lastError;
    }
    
    uint8_t buffer[7];
    RTCError error = burstRead(RTC_SECONDS, buffer, 7);
    if (error != RTCError::None) {
        return error;
    }
    
    // Konvertierung von BCD
    dateTime.second = bcdToDec(buffer[0] & 0x7F);
    dateTime.minute = bcdToDec(buffer[1]);
    dateTime.hour = bcdToDec(buffer[2] & 0x3F);  // 24h Format
    dateTime.dayOfWeek = buffer[3];
    dateTime.day = bcdToDec(buffer[4]);
    dateTime.month = bcdToDec(buffer[5] & 0x1F);
    dateTime.year = bcdToDec(buffer[6]) + 2000;
    
    return RTCError::None;
}

RTCError DS3231Driver::setDateTime(const RTCDateTime& dateTime) {
    if (!initialized) {
        setError(RTCError::NotInitialized);
        return lastError;
    }
    
    if (!isValidDateTime(dateTime)) {
        setError(RTCError::InvalidDateTime);
        return lastError;
    }
    
    uint8_t buffer[7];
    buffer[0] = decToBcd(dateTime.second);
    buffer[1] = decToBcd(dateTime.minute);
    buffer[2] = decToBcd(dateTime.hour);
    buffer[3] = dateTime.dayOfWeek;
    buffer[4] = decToBcd(dateTime.day);
    buffer[5] = decToBcd(dateTime.month);
    buffer[6] = decToBcd(dateTime.year - 2000);
    
    return burstWrite(RTC_SECONDS, buffer, 7);
}

RTCError DS3231Driver::setFromCompileTime() {
    const char* date = __DATE__;
    const char* time = __TIME__;
    
    // Parse __DATE__ "MMM DD YYYY"
    char monthStr[4];
    RTCDateTime dt;
    int day, year;
    sscanf(date, "%s %d %d", monthStr, &day, &year);
    dt.day = day;
    dt.year = year;
    
    // Konvertiere Monatsname in Nummer
    const char* months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", 
                           "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    for(uint8_t i = 0; i < 12; i++) {
        if(strncmp(monthStr, months[i], 3) == 0) {
            dt.month = i + 1;  // 1-basiert
            break;
        }
    }
    
    // Parse __TIME__ "HH:MM:SS"
    int hour, minute, second;
    sscanf(time, "%d:%d:%d", &hour, &minute, &second);
    dt.hour = hour;
    dt.minute = minute;
    dt.second = second;
    
    // Wochentag berechnen (optional)
    dt.dayOfWeek = 1;  // Standardwert
    
    return setDateTime(dt);
}

float DS3231Driver::getTemperature() {
    if (!initialized) {
        return 0.0f;
    }
    
    uint8_t msb, lsb;
    if (readRegister(TEMP_MSB, msb) != RTCError::None ||
        readRegister(TEMP_LSB, lsb) != RTCError::None) {
        return 0.0f;
    }
    
    return (float)msb + ((lsb >> 6) * 0.25f);
}

bool DS3231Driver::isRunning() {
    if (!initialized) {
        return false;
    }
    
    uint8_t status;
    if (readRegister(RTC_STATUS, status) == RTCError::None) {
        return !(status & 0x80);  // OSF Flag prüfen
    }
    return false;
}

bool DS3231Driver::hasLostPower() {
    if (!initialized) {
        return true;
    }
    
    uint8_t status;
    if (readRegister(RTC_STATUS, status) == RTCError::None) {
        return (status & 0x80) != 0;  // OSF Flag prüfen
    }
    return true;
}

bool DS3231Driver::performSelfTest() {
    if (!initialized) {
        Serial.println("DS3231: Nicht initialisiert!");
        return false;
    }
    
    Serial.println("\nDS3231 Selbsttest:");
    
    // 1. Oszillator-Status
    Serial.printf("- Oszillator: %s\n", isRunning() ? "OK" : "FEHLER");
    
    // 2. Batterie/Power-Loss Check
    Serial.printf("- Power Status: %s\n", 
                 !hasLostPower() ? "OK" : "Power Loss detektiert");
    
    // 3. Temperatur-Sensor
    float temp = getTemperature();
    bool tempOk = (temp > -40.0f && temp < 85.0f);
    Serial.printf("- Temperatur: %.1f°C [%s]\n", 
                 temp, tempOk ? "OK" : "FEHLER");
    
    // 4. Zeitlese-Test
    RTCDateTime dt;
    bool timeOk = (getDateTime(dt) == RTCError::None);
    if (timeOk) {
        Serial.printf("- Aktuelle Zeit: %s [OK]\n", 
                     dateTimeToString(dt).c_str());
    } else {
        Serial.println("- Zeitlesefehler [FEHLER]");
    }
    
    return isRunning() && !hasLostPower() && tempOk && timeOk;
}

// Private Hilfsmethoden
RTCError DS3231Driver::readRegister(uint8_t reg, uint8_t& value) {
    wire->beginTransmission(address);
    wire->write(reg);
    if (wire->endTransmission() != 0) {
        setError(RTCError::CommunicationError);
        return lastError;
    }
    
    if (wire->requestFrom(address, (uint8_t)1) != 1) {
        setError(RTCError::CommunicationError);
        return lastError;
    }
    
    value = wire->read();
    return RTCError::None;
}

RTCError DS3231Driver::writeRegister(uint8_t reg, uint8_t value) {
    wire->beginTransmission(address);
    wire->write(reg);
    wire->write(value);
    if (wire->endTransmission() != 0) {
        setError(RTCError::CommunicationError);
        return lastError;
    }
    return RTCError::None;
}

RTCError DS3231Driver::burstRead(uint8_t reg, uint8_t* buffer, uint8_t length) {
    wire->beginTransmission(address);
    wire->write(reg);
    if (wire->endTransmission() != 0) {
        setError(RTCError::CommunicationError);
        return lastError;
    }
    
    if (wire->requestFrom(address, length) != length) {
        setError(RTCError::CommunicationError);
        return lastError;
    }
    
    for (uint8_t i = 0; i < length; i++) {
        buffer[i] = wire->read();
    }
    
    return RTCError::None;
}

RTCError DS3231Driver::burstWrite(uint8_t reg, const uint8_t* buffer, uint8_t length) {
    wire->beginTransmission(address);
    wire->write(reg);
    for (uint8_t i = 0; i < length; i++) {
        wire->write(buffer[i]);
    }
    
    if (wire->endTransmission() != 0) {
        setError(RTCError::CommunicationError);
        return lastError;
    }
    
    return RTCError::None;
}

uint8_t DS3231Driver::bcdToDec(uint8_t bcd) {
    return (bcd / 16 * 10) + (bcd % 16);
}

uint8_t DS3231Driver::decToBcd(uint8_t dec) {
    return (dec / 10 * 16) + (dec % 10);
}

void DS3231Driver::setError(RTCError error) {
    lastError = error;
    if (error != RTCError::None) {
        Serial.printf("DS3231 Error: %d\n", static_cast<int>(error));
    }
}

}} // namespace Hardware::RTC