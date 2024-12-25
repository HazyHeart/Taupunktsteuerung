#include "ui/screens/TemperatureScreen.h"
#include "hardware/sensors/BME280Driver.h" // Sicherstellen, dass der Pfad korrekt ist

void TemperatureScreen::init() {
    // Initialisierung des BME280-Sensors
    if (!Hardware::Sensors::BME280Driver::init(0x76)) {
        Serial.println("BME280 nicht gefunden!");
        while (1); // Endlosschleife bei Fehler
    }

    DisplayManager::clear();
    DisplayManager::printText("Temperatur:", 0, 0);
    DisplayManager::show();
}

void TemperatureScreen::update() {
    // Temperaturwert aktualisieren
    temperature = Hardware::Sensors::BME280Driver::readTemperature();
}

void TemperatureScreen::render() {
    DisplayManager::clear();
    DisplayManager::printText("Temperatur:", 0, 0);

    // Temperaturwert anzeigen
    char tempBuffer[10];
    snprintf(tempBuffer, sizeof(tempBuffer), "%.2f C", temperature);
    DisplayManager::printText(tempBuffer, 0, 10);

    DisplayManager::show();
}
