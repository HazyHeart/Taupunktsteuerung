#include "BME280Driver.h"

namespace Hardware {
namespace Sensors {

Adafruit_BME280 BME280Driver::bme;

bool BME280Driver::init(uint8_t address) {
    return bme.begin(address);
}

float BME280Driver::readTemperature() {
    return bme.readTemperature();
}

float BME280Driver::readHumidity() {
    return bme.readHumidity();
}

} // namespace Sensors
} // namespace Hardware
