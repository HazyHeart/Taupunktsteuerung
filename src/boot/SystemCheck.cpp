// boot/SystemCheck.cpp
#include "boot/SystemCheck.h"
#include "config/constants.h"

bool SystemCheck::checkVoltage() {
    // TODO: Implementiere echte Spannungsprüfung
    return true;
}

bool SystemCheck::checkFreeMem() {
    return ESP.getFreeHeap() >= SystemConstants::MIN_FREE_HEAP;  // Geändert zu SystemConstants
}

float SystemCheck::getSupplyVoltage() {
    // TODO: Implementiere echte Spannungsmessung
    return 3.3f;
}

uint32_t SystemCheck::getFreeHeap() {
    return ESP.getFreeHeap();
}

void SystemCheck::printSystemInfo() {
    Serial.println("System Information:");
    Serial.printf("Free Heap: %d bytes\n", getFreeHeap());
    Serial.printf("Supply Voltage: %.2fV\n", getSupplyVoltage());
    Serial.printf("CPU Freq: %dMHz\n", ESP.getCpuFreqMHz());
    Serial.printf("SDK Version: %s\n", ESP.getSdkVersion());
}