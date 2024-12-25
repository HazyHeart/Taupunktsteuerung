#include "BME280Driver.h"
#include <Adafruit_BME280.h>

namespace Hardware {
namespace Sensors {

// Statische Variable initialisieren
Adafruit_BME280 BME280Driver::bme;

// Initialisierungsmethode
bool BME280Driver::init(uint8_t address) {
    return bme.begin(address);
}

// Temperatur auslesen
float BME280Driver::readTemperature() {
    return bme.readTemperature();
}

} // namespace Sensors
} // namespace Hardware