#include "UIFrame.h"
#include "hardware/display/DisplayManager.h"

UIFrame::UIFrame(int x, int y, int width, int height, const char* title)
    : x(x), y(y), width(width), height(height), title(title) {}

void UIFrame::setTitle(const char* title) {
    this->title = title;
}

void UIFrame::addWidget(const char* text, int line) {
    // Sicherstellen, dass es genügend Platz gibt
    if (line >= lines.size()) {
        lines.resize(line + 1);
    }
    lines[line] = text;
}

void UIFrame::draw() {
    // Debug-Ausgaben
    Serial.printf("Frame: X=%d, Y=%d, W=%d, H=%d\n", x, y, width, height);

    // Überprüfen, ob die Werte im erlaubten Bereich liegen
    if (x < 0 || y < 0 || width <= 0 || height <= 0 || x + width > 128 || y + height > 64) {
        Serial.println("Fehler: Ungültige Rahmenwerte!");
        return; // Abbrechen bei falschen Werten
    }

    // Zeichnen des Rahmens
    DisplayManager::drawRect(x, y, width, height);

    // Titel anzeigen
    if (!title.empty()) {
        int titleWidth = title.length() * 6;
        int titleX = x + (width - titleWidth) / 2;
        DisplayManager::printText(title.c_str(), titleX, y + 2);
    }

    DisplayManager::show();
}
