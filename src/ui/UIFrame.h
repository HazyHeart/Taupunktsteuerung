#ifndef UIFRAME_H
#define UIFRAME_H

#include <vector>
#include <string>

class UIFrame {
public:
    UIFrame(int x, int y, int width, int height, const char* title = "");

    void draw(); // Fensterrahmen und Titel zeichnen
    void setTitle(const char* title); // Titel ändern
    void addWidget(const char* text, int line); // Text oder Widgets hinzufügen

private:
    int x, y, width, height; // Größe und Position
    std::string title;       // Titel des Fensters
    std::vector<std::string> lines; // Zeilen für Inhalte
};

#endif // UIFRAME_H
