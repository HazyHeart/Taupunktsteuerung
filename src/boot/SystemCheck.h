// boot/SystemCheck.h
#pragma once

#include <Arduino.h>
#include "config/constants.h"

class SystemCheck {
public:
    static bool checkVoltage();
    static bool checkFreeMem();
    static float getSupplyVoltage();
    static uint32_t getFreeHeap();
    static void printSystemInfo();
};
