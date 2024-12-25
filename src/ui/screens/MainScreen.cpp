#include "ui/screens/MainScreen.h"

void MainScreen::init() {
    DisplayManager::clear();
    DisplayManager::printText("Hauptanzeige", 0, 0);
    DisplayManager::show();
}

void MainScreen::update() {
    // Hier später weitere Logik für Sensoren
}

void MainScreen::render() {
    DisplayManager::clear();
    DisplayManager::printText("Willkommen!", 0, 10);
    DisplayManager::show();
}
