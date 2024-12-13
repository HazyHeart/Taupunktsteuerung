#include <Arduino.h>
#include <Wire.h>
#include <memory>
#include "hardware/display/SH1106Driver.h"
#include "config/pins.h"

std::unique_ptr<Hardware::Display::SH1106Driver> display;

// Boot-Sequenz simulieren
void simulateBootSequence() {
    const struct BootStep {
        const char* status;
        uint8_t progress;
        uint32_t delay;
    } bootSteps[] = {
        {"Initialisiere...", 0, 1000},
        {"Hardware-Test...", 20, 800},
        {"Sensoren...", 40, 1000},
        {"BME280...", 60, 800},
        {"RTC...", 80, 800},
        {"System bereit", 100, 1000}
    };

    // Durchlaufe alle Boot-Schritte
    for (const auto& step : bootSteps) {
        // Display löschen
        display->clear();
        
        // Header
        display->drawString(20, 3, "TAUPUNKTREGLER");
        display->drawLine(0, 12, 127, 12);
        
        // Status
        display->drawString(4, 24, step.status);
        
        // Progress Bar
        display->drawRect(14, 48, 100, 8);
        uint8_t fillWidth = step.progress * 96 / 100;
        if (fillWidth > 0) {
            display->fillRect(16, 50, fillWidth, 4);
        }
        
        // Progress Text
        char buf[8];
        snprintf(buf, sizeof(buf), "%d%%", step.progress);
        display->drawString(116, 48, buf);
        
        // Display aktualisieren
        display->updateDisplay();
        
        // Warten
        delay(step.delay);
    }
}

void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("\n=== Display Test ===\n");
    
    // Display-Treiber erstellen
    display = std::make_unique<Hardware::Display::SH1106Driver>();
    
    // Display konfigurieren
    Hardware::Display::DisplayConfig config{
        .i2cAddress = 0x3C,
        .width = 128,
        .height = 64,
        .sda = Pins::I2C1_SDA,
        .scl = Pins::I2C1_SCL
    };
    
    if (display->initialize(config) != Hardware::Display::DisplayError::None) {
        Serial.println("Display-Initialisierung fehlgeschlagen!");
        return;
    }
    
    Serial.println("Display initialisiert - starte Boot-Sequenz");
    simulateBootSequence();
}

void loop() {
    delay(100);
}