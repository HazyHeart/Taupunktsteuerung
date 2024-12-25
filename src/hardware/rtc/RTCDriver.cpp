#include "hardware/rtc/RTCDriver.h"

namespace Hardware {
namespace RTC {

bool RTCDriver::isValidDateTime(const RTCDateTime& dt) {
    // Grundlegende Bereichsprüfungen
    if (dt.year < 2000 || dt.year > 2099) return false;
    if (dt.month < 1 || dt.month > 12) return false;
    if (dt.hour > 23) return false;
    if (dt.minute > 59) return false;
    if (dt.second > 59) return false;
    if (dt.dayOfWeek < 1 || dt.dayOfWeek > 7) return false;
    
    // Tage pro Monat prüfen
    const uint8_t daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    uint8_t maxDays = daysInMonth[dt.month - 1];
    
    // Schaltjahr-Korrektur für Februar
    if (dt.month == 2) {
        bool isLeapYear = (dt.year % 4 == 0 && (dt.year % 100 != 0 || dt.year % 400 == 0));
        if (isLeapYear) maxDays = 29;
    }
    
    if (dt.day < 1 || dt.day > maxDays) return false;
    
    return true;
}

String RTCDriver::dateTimeToString(const RTCDateTime& dt) {
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%04d-%02d-%02d %02d:%02d:%02d",
             dt.year, dt.month, dt.day,
             dt.hour, dt.minute, dt.second);
    return String(buffer);
}

}} // namespace Hardware::RTC
