#ifndef TEMPERATURESCREEN_H
#define TEMPERATURESCREEN_H

#include "hardware/display/DisplayManager.h"
#include "hardware/sensors/BME280Driver.h" // Treiber für BME280-Sensor

class TemperatureScreen {
public:
    void init();
    void update();
    void render();

private:
    float temperature = 0.0f; // Aktuelle Temperatur
};

#endif // TEMPERATURESCREEN_H
