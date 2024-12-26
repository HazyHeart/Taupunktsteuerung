#include "BootManager.h"
#include "hardware/display/DisplayManager.h"
#include "hardware/sensors/BME280Driver.h"
#include "utils/I2CScanner.h"
#include "ui/widgets/ProgressBar.h"
#include "ui/UIWidgets.h"
#include "ui/UIFrame.h"
#include "hardware/sensors/BME280Driver.h"

void BootManager::init() {
    DisplayManager::init(0x3C); // Display initialisieren
    DisplayManager::clear();

    Serial.println("Starte System...");
    DisplayManager::printText("Starte...", 0, 0);
    DisplayManager::show();
    delay(500);

    // I2C-Check
    Serial.println("Prüfe I2C-Bus...");
    DisplayManager::printText("I2C-Check...", 0, 10);
    DisplayManager::show();
    delay(500);

    // Sensor-Check
    if (!BME280Driver::init(0x76)) {
        Serial.println("Sensorfehler!");
        DisplayManager::printText("Sensorfehler!", 0, 20);
        DisplayManager::show();
        while (1); // Endlosschleife bei Fehler
    }
    Serial.println("Sensor OK.");
    DisplayManager::printText("Sensor OK.", 0, 20);
    DisplayManager::show();
    delay(500);

    // Fertigmeldung
    Serial.println("System bereit!");
    DisplayManager::printText("Bereit!", 0, 30);
    DisplayManager::show();
    delay(1000);
}

// Fehlende Methode run() hinzugefügt
void BootManager::run() {
    // Für die Weiterentwicklung später vorbereitet
    Serial.println("BootManager::run() aufgerufen!");
    delay(1000);
}

bool BootManager::selfCheck() {
    // Schritt 1: I2C-Scan
    updateProgress("Prüfe I2C-Bus...", 25);
    delay(1000); // 1 Sekunde Pause
    I2CScanner::scan();

    // Schritt 2: BME280-Test
    updateProgress("Prüfe Sensoren...", 50);
    delay(1000); // 1 Sekunde Pause
    if (!Hardware::Sensors::BME280Driver::init(0x76)) {
        Serial.println("Fehler: BME280 nicht gefunden!");
        errorFlag = true;
    } else {
        Serial.println("BME280 erfolgreich initialisiert.");
    }

    // Ergebnis des Self-Checks
    return !errorFlag;
}

void BootManager::showError(const char* message) {
    for (int i = 30; i > 0; i--) {
        DisplayManager::clear();
        DisplayManager::printText("Fehler:", 0, 0);

        // Fehlertext zentriert anzeigen
        int msgWidth = strlen(message) * 6;
        int msgX = (128 - msgWidth) / 2;
        DisplayManager::printText(message, msgX, 10);

        // Countdown visuell zentrieren
        char countdown[20];
        snprintf(countdown, sizeof(countdown), "Restart: %ds", i);
        int countWidth = strlen(countdown) * 6;
        int countX = (128 - countWidth) / 2;
        DisplayManager::printText(countdown, countX, 30);

        DisplayManager::show(); // Anzeige aktualisieren
        delay(1000); // 1 Sekunde Pause pro Countdown-Schritt
    }

    // Neustart auslösen
    ESP.restart();
}

void BootManager::updateProgress(const char* message, int progress) {
    DisplayManager::clear();

    // Titel zentriert anzeigen
    UIWidgets::drawCenteredText(message, 5);

    // Fortschrittsbalken
    ProgressBar bar(4, 20, 120, 10); // Position des Fortschrittsbalkens
    bar.draw(progress, "Laden...", "Prüfe Sensoren"); // Status unterhalb des Balkens anzeigen

    // Rahmen für Fortschrittsbereich
    UIWidgets::drawFrame(2, 15, 124, 20);

    DisplayManager::show();
    delay(500); // Lesbarkeit verbessern
}