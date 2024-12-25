#include <Arduino.h>
#include "boot/BootManager.h"

BootManager bootManager;

void setup() {
    bootManager.init();
}

void loop() {
    bootManager.run();
}
