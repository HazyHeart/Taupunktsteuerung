#pragma once
#include "SDCardDriver.h"
#include <SPI.h>
#include <SD.h>

namespace Hardware {
namespace SDCard {

class SDCardDriver_SPI : public SDCardDriver {
public:
    SDCardDriver_SPI();
    ~SDCardDriver_SPI() override;
    
    // SDCardDriver Interface Implementation
    SDError initialize(const SDConfig& config) override;
    bool isInitialized() const override { return initialized; }
    void reset() override;
    
    SDError mount() override;
    SDError unmount() override;
    bool isMounted() const override { return mounted; }
    
    // Dateioperationen
    SDError openFile(const char* path, const char* mode) override;
    SDError closeFile() override;
    SDError writeData(const uint8_t* data, size_t length) override;
    SDError readData(uint8_t* buffer, size_t length, size_t* bytesRead) override;
    SDError seek(size_t offset) override;
    SDError sync() override;
    
    // Verzeichnisoperationen
    SDError createDir(const char* path) override;
    SDError removeDir(const char* path) override;
    SDError listDir(const char* path) override;
    
    // Dateimanagement
    bool exists(const char* path) const override;
    SDError remove(const char* path) override;
    SDError rename(const char* oldPath, const char* newPath) override;
    
    // Status
    SDError getStatus(SDCardStatus& status) override;
    SDError getLastError() const override { return lastError; }
    bool performSelfTest() override;

private:
    bool initialized;
    bool mounted;
    SDError lastError;
    File currentFile;
    
    // Konfiguration
    SDConfig config;
    String mountPoint;
    
    // SPI & Pin Management
    SPIClass* spi;
    uint8_t csPin;
    
    // Hilfsmethoden
    void setError(SDError error);
    bool validatePins();
    void initializePins();
    bool testSPICommunication();
    void printCardInfo();
    bool checkMounted(const char* operation);
    void sendByte(uint8_t b);
    uint8_t readByte();

    // Hilfs-Funktion für Pin-Validierung
    bool isConflictingPin(uint8_t pin) const {
        return (pin >= 6 && pin <= 11);  // Flash-Pins
    }
};

}} // namespace Hardware::SDCard