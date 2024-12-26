#include "UIWidgets.h"
#include "hardware/display/DisplayManager.h"

void UIWidgets::drawCenteredText(const char* text, int y) {
    int textWidth = strlen(text) * 6; // 6 Pixel pro Buchstabe
    int x = (128 - textWidth) / 2;   // Mittig berechnen
    DisplayManager::printText(text, x, y);
}

void UIWidgets::drawFrame(int x, int y, int width, int height) {
    DisplayManager::drawRect(x, y, width, height);
}
