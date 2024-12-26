#include "ProgressBar.h"
#include "hardware/display/DisplayManager.h"

ProgressBar::ProgressBar(int x, int y, int width, int height)
    : x(x), y(y), width(width), height(height) {}

void ProgressBar::draw(int progress, const char* label, const char* status) {
    DisplayManager::clear();

    // Fortschrittsbalken zeichnen
    int filledWidth = (progress * width) / 100;
    DisplayManager::drawRect(x, y, width, height);
    DisplayManager::fillRect(x + 1, y + 1, filledWidth - 2, height - 2);

    // Label im Balken
    int labelWidth = strlen(label) * 6;
    int labelX = x + (width - labelWidth) / 2;
    int labelY = y + (height - 8) / 2;
    DisplayManager::printText(label, labelX, labelY);

    // Status-Text über dem Balken
    if (status && strlen(status) > 0) {
        int statusWidth = strlen(status) * 6;
        int statusX = x + (width - statusWidth) / 2;
        DisplayManager::printText(status, statusX, y - 10);
    }

    DisplayManager::show();
}

