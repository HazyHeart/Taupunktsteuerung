#ifndef BOOTMANAGER_H
#define BOOTMANAGER_H

class BootManager {
public:
    void init(); // Startet den Boot-Prozess
    void run();  // Führt weitere Abläufe nach dem Boot aus

private:
    bool selfCheck(); // Überprüft alle Komponenten
    void showError(const char* message); // Zeigt Fehler an
    void updateProgress(const char* message, int progress); // Fortschrittsanzeige
    bool errorFlag = false; // Fehlerstatus
};

#endif // BOOTMANAGER_H
