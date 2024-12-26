#ifndef PROGRESSBAR_H
#define PROGRESSBAR_H

class ProgressBar {
public:
    ProgressBar(int x, int y, int width, int height);
    void draw(int progress, const char* label, const char* status = ""); // Fortschrittsanzeige

private:
    int x, y, width, height;
};

#endif // PROGRESSBAR_H
