// src/ui/screens/BootScreen.cpp
#include "ui/screens/BootScreen.h"
#include "hardware/display/DisplayDriver.h"

namespace UI {
namespace Screens {

BootScreen::BootScreen() {
    // Screen-Initialisierung
}

void BootScreen::setStatus(const String& status) {
    currentStatus = status;
    markDirty();
}

void BootScreen::setProgress(uint8_t progress) {
    currentProgress = progress;
    markDirty();
}

void BootScreen::render(Hardware::Display::DisplayDriver& display) {
    // Header mit Titel
    display.drawString(20, 3, "TAUPUNKTREGLER");
    display.drawLine(0, 12, 127, 12);
    
    // Status-Text
    display.drawString(4, 24, currentStatus);
    
    // Progress Bar
    display.drawRect(14, 48, 100, 8);
    uint8_t fillWidth = currentProgress * 96 / 100;  // 96 = 100 - 4 Pixel Rand
    if (fillWidth > 0) {
        display.fillRect(16, 50, fillWidth, 4);
    }
    
    // Prozentanzeige
    char buf[8];
    snprintf(buf, sizeof(buf), "%d%%", currentProgress);
    display.drawString(116, 48, buf);
}

}} // namespace UI::Screens