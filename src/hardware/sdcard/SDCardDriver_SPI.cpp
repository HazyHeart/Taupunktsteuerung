// hardware/sdcard/SDCardDriver_SPI.cpp
#include "hardware/sdcard/SDCardDriver_SPI.h"
#include <Arduino.h>

namespace Hardware {
namespace SDCard {

// Constructor & Destructor
SDCardDriver_SPI::SDCardDriver_SPI()
    : initialized(false)
    , mounted(false)
    , lastError(SDError::None)
    , spi(nullptr)
    , csPin(0)
{
}

SDCardDriver_SPI::~SDCardDriver_SPI() {
    if (mounted) {
        unmount();
    }
}

SDError SDCardDriver_SPI::initialize(const SDConfig& config) {
    if (initialized) {
        return SDError::None;
    }

    Serial.println("\n=== Safe SD Card Init ===");
    
    // Timeout initialization
    const uint32_t TOTAL_TIMEOUT = 5000;  // 5 seconds
    const uint32_t startTime = millis();
    
    // Save configuration
    this->config = config;
    this->csPin = config.csPin;
    this->mountPoint = config.mountPoint;

    // Set CS pin HIGH first
    digitalWrite(config.csPin, HIGH);
    pinMode(config.csPin, OUTPUT);
    delay(100);
    
    // SPI with reduced speed
    spi = new SPIClass(HSPI);
    spi->begin(config.sckPin, config.misoPin, config.mosiPin, -1); // handle CS separately
    
    Serial.println("Hardware Configuration:");
    Serial.println("- Using VSPI Bus");
    Serial.printf("- CS Pin: GPIO%d\n", config.csPin);
    Serial.printf("- MOSI Pin: GPIO%d\n", config.mosiPin);
    Serial.printf("- MISO Pin: GPIO%d\n", config.misoPin);
    Serial.printf("- SCK Pin: GPIO%d\n", config.sckPin);
    
    // Pin validation
    if (!validatePins()) {
        setError(SDError::PinConflict);
        return lastError;
    }
    
    // Communication test
    if (!testSPICommunication()) {
        setError(SDError::SPIError);
        return lastError;
    }

    // Mount SD card
    if (!SD.begin(csPin, *spi)) {
        Serial.println("SD Card mount failed");
        setError(SDError::MountFailed);
        return lastError;
    }

    initialized = true;
    mounted = true;
    setError(SDError::None);
    
    // Print status
    printCardInfo();
    
    // Final timing report
    Serial.printf("\nTotal initialization time: %lu ms\n", millis() - startTime);
    Serial.println("SD Card initialization successful!\n");
    
    return SDError::None;
}

bool SDCardDriver_SPI::testSPICommunication() {
    Serial.println("\nSD Card Full Initialization");
    
    // 1. Setup
    Serial.println("1. Configure Pins");
    pinMode(csPin, OUTPUT);
    pinMode(config.mosiPin, OUTPUT);
    pinMode(config.sckPin, OUTPUT);
    pinMode(config.misoPin, INPUT_PULLUP);
    
    digitalWrite(csPin, HIGH);
    digitalWrite(config.mosiPin, HIGH);
    digitalWrite(config.sckPin, LOW);
    delay(250);
    
    // 2. Power-up
    Serial.println("2. Power-up sequence");
    for(int i = 0; i < 160; i++) {
        digitalWrite(config.sckPin, HIGH);
        delayMicroseconds(10);
        digitalWrite(config.sckPin, LOW);
        delayMicroseconds(10);
        if(i % 20 == 0) Serial.print(".");
    }
    Serial.println(" done");
    delay(100);
    
    // 3. CMD0
    Serial.println("\n3. GO_IDLE_STATE (CMD0)");
    digitalWrite(csPin, LOW);
    delay(20);
    sendByte(0xFF);  // Sync
    
    // CMD0 senden
    uint8_t cmd0[] = {0x40, 0x00, 0x00, 0x00, 0x00, 0x95};
    for(uint8_t b : cmd0) {
        sendByte(b);
        delay(1);
    }
    
    uint8_t r1 = readByte();
    Serial.printf("CMD0 response: 0x%02X\n", r1);
    
    if(r1 != 0x01) {
        Serial.println("CMD0 failed!");
        return false;
    }
    
    digitalWrite(csPin, HIGH);
    delay(100);
    
    // 4. CMD8 für SDHC Karten
    Serial.println("\n4. SEND_IF_COND (CMD8)");
    digitalWrite(csPin, LOW);
    delay(20);
    sendByte(0xFF);  // Sync
    
    uint8_t cmd8[] = {0x48, 0x00, 0x00, 0x01, 0xAA, 0x87};
    for(uint8_t b : cmd8) {
        sendByte(b);
        delay(1);
    }
    
    r1 = readByte();
    Serial.printf("CMD8 R1 response: 0x%02X\n", r1);
    
    if(r1 == 0x01) {  // Karte akzeptiert CMD8
        uint8_t r7[4];
        for(int i = 0; i < 4; i++) {
            r7[i] = readByte();
        }
        Serial.printf("CMD8 R7: %02X %02X %02X %02X\n", r7[0], r7[1], r7[2], r7[3]);
        
        if(r7[2] != 0x01 || r7[3] != 0xAA) {
            Serial.println("Card voltage not supported!");
            return false;
        }
    }
    
    digitalWrite(csPin, HIGH);
    delay(100);
    
    Serial.println("\nCard initialization successful!");
    return true;
}

void SDCardDriver_SPI::sendByte(uint8_t b) {
    for(int bit = 7; bit >= 0; bit--) {
        digitalWrite(config.mosiPin, (b >> bit) & 1);
        digitalWrite(config.sckPin, HIGH);
        delayMicroseconds(5);
        digitalWrite(config.sckPin, LOW);
        delayMicroseconds(5);
    }
}

uint8_t SDCardDriver_SPI::readByte() {
    uint8_t b = 0;
    for(int i = 7; i >= 0; i--) {
        digitalWrite(config.sckPin, HIGH);
        if(digitalRead(config.misoPin)) {
            b |= (1 << i);
        }
        digitalWrite(config.sckPin, LOW);
        delayMicroseconds(5);
    }
    return b;
}

SDError SDCardDriver_SPI::seek(size_t offset) {
    if (!checkMounted("seek")) {
        return lastError;
    }
    
    if (!currentFile) {
        setError(SDError::IOError);
        return lastError;
    }
    
    if (!currentFile.seek(offset)) {
        setError(SDError::IOError);
        return lastError;
    }
    
    return SDError::None;
}

SDError SDCardDriver_SPI::sync() {
    if (!checkMounted("sync")) {
        return lastError;
    }
    
    if (currentFile) {
        currentFile.flush();
    }
    
    return SDError::None;
}

SDError SDCardDriver_SPI::createDir(const char* path) {
    if (!checkMounted("createDir")) {
        return lastError;
    }
    
    if (!SD.mkdir(path)) {
        setError(SDError::FileSystemError);
        return lastError;
    }
    
    return SDError::None;
}

SDError SDCardDriver_SPI::removeDir(const char* path) {
    if (!checkMounted("removeDir")) {
        return lastError;
    }
    
    if (!SD.rmdir(path)) {
        setError(SDError::FileSystemError);
        return lastError;
    }
    
    return SDError::None;
}

SDError SDCardDriver_SPI::listDir(const char* path) {
    if (!checkMounted("listDir")) {
        return lastError;
    }
    
    File dir = SD.open(path);
    if (!dir) {
        setError(SDError::FileSystemError);
        return lastError;
    }
    
    if (!dir.isDirectory()) {
        dir.close();
        setError(SDError::InvalidParameter);
        return lastError;
    }
    
    Serial.printf("\nListing directory: %s\n", path);
    
    File entry;
    while (entry = dir.openNextFile()) {
        if (entry.isDirectory()) {
            Serial.printf("  DIR: %s\n", entry.name());
        } else {
            Serial.printf("  FILE: %s (%d bytes)\n", entry.name(), entry.size());
        }
        entry.close();
    }
    
    dir.close();
    return SDError::None;
}

bool SDCardDriver_SPI::exists(const char* path) const {
    if (!mounted) return false;
    return SD.exists(path);
}

SDError SDCardDriver_SPI::remove(const char* path) {
    if (!checkMounted("remove")) {
        return lastError;
    }
    
    if (!SD.remove(path)) {
        setError(SDError::FileSystemError);
        return lastError;
    }
    
    return SDError::None;
}

SDError SDCardDriver_SPI::rename(const char* oldPath, const char* newPath) {
    if (!checkMounted("rename")) {
        return lastError;
    }
    
    if (!SD.rename(oldPath, newPath)) {
        setError(SDError::FileSystemError);
        return lastError;
    }
    
    return SDError::None;
}

SDError SDCardDriver_SPI::getStatus(SDCardStatus& status) {
    if (!checkMounted("getStatus")) {
        return lastError;
    }
    
    status.mounted = mounted;
    status.writeProtected = false;  // SD-Bibliothek bietet keine direkte Methode hierfür
    status.totalBytes = SD.totalBytes();
    status.usedBytes = SD.usedBytes();
    status.freeBytes = status.totalBytes - status.usedBytes;
    
    uint8_t cardType = SD.cardType();
    switch(cardType) {
        case CARD_MMC:  status.fileSystem = "MMC"; break;
        case CARD_SD:   status.fileSystem = "SDSC"; break;
        case CARD_SDHC: status.fileSystem = "SDHC"; break;
        default:        status.fileSystem = "Unknown";
    }
    
    status.label = "";  // Optional: Implementiere Volumelabel-Lesung wenn benötigt
    
    return SDError::None;
}

// Private Hilfsmethode für Mount-Status-Prüfung
bool SDCardDriver_SPI::checkMounted(const char* operation) {
    if (!initialized) {
        Serial.printf("Error: Driver not initialized for operation '%s'\n", operation);
        setError(SDError::NotInitialized);
        return false;
    }
    
    if (!mounted) {
        Serial.printf("Error: Card not mounted for operation '%s'\n", operation);
        setError(SDError::NotInitialized);
        return false;
    }
    
    return true;
}

void SDCardDriver_SPI::initializePins() {
    // CS Pin zuerst konfigurieren
    pinMode(csPin, OUTPUT);
    digitalWrite(csPin, HIGH);  // Chip deselektieren
    
    // MOSI, MISO, SCK werden von SPI.begin() konfiguriert
    
    // Debug Output
    Serial.println("Pin initialization:");
    Serial.printf("- CS Pin (GPIO%d): OUTPUT, Initially HIGH\n", csPin);
    Serial.printf("- MOSI Pin (GPIO%d): Handled by SPI\n", config.mosiPin);
    Serial.printf("- MISO Pin (GPIO%d): Handled by SPI\n", config.misoPin);
    Serial.printf("- SCK Pin (GPIO%d): Handled by SPI\n", config.sckPin);
}

void SDCardDriver_SPI::setError(SDError error) {
    lastError = error;
    if (error != SDError::None) {
        Serial.printf("SD Card Error: %d - %s\n", 
            static_cast<int>(error),
            getErrorString(error));
    }
}

bool SDCardDriver_SPI::validatePins() {
    Serial.println("Checking for pin conflicts...");
    
    bool hasConflict = false;
    
    // Prüfe MOSI
    if (config.mosiPin >= 6 && config.mosiPin <= 11) {
        Serial.printf("MOSI (GPIO%d) conflicts with flash memory\n", config.mosiPin);
        hasConflict = true;
    }
    
    // Prüfe MISO
    if (config.misoPin >= 6 && config.misoPin <= 11) {
        Serial.printf("MISO (GPIO%d) conflicts with flash memory\n", config.misoPin);
        hasConflict = true;
    }
    
    // Prüfe SCK
    if (config.sckPin >= 6 && config.sckPin <= 11) {
        Serial.printf("SCK (GPIO%d) conflicts with flash memory\n", config.sckPin);
        hasConflict = true;
    }
    
    // Prüfe CS
    if (config.csPin >= 6 && config.csPin <= 11) {
        Serial.printf("CS (GPIO%d) conflicts with flash memory\n", config.csPin);
        hasConflict = true;
    }
    
    if (!hasConflict) {
        Serial.println("✓ No pin conflicts detected");
    }
    
    return !hasConflict;
}

void SDCardDriver_SPI::printCardInfo() {
    uint8_t cardType = SD.cardType();
    
    Serial.println("\nSD Card Info:");
    Serial.printf("- Type: %s\n", 
        cardType == CARD_MMC  ? "MMC" :
        cardType == CARD_SD   ? "SDSC" :
        cardType == CARD_SDHC ? "SDHC" : "UNKNOWN");
        
    if (cardType != CARD_NONE) {
        uint64_t cardSize = SD.cardSize() / (1024 * 1024);
        uint64_t totalBytes = SD.totalBytes() / (1024 * 1024);
        uint64_t usedBytes = SD.usedBytes() / (1024 * 1024);
        
        Serial.printf("- Card Size: %lluMB\n", cardSize);
        Serial.printf("- Total Space: %lluMB\n", totalBytes);
        Serial.printf("- Used Space: %lluMB\n", usedBytes);
        Serial.printf("- Free Space: %lluMB\n", totalBytes - usedBytes);
        
        // Connection status
        Serial.println("\nConnection Info:");
        Serial.printf("- SPI Bus: HSPI\n");
        Serial.printf("- CS Pin: GPIO%d\n", csPin);
        Serial.printf("- MOSI Pin: GPIO%d\n", config.mosiPin);
        Serial.printf("- MISO Pin: GPIO%d\n", config.misoPin);
        Serial.printf("- SCK Pin: GPIO%d\n", config.sckPin);
        Serial.printf("- Initial clock: 400kHz\n");  // Wir wissen das aus der Konfiguration
    } else {
        Serial.println("- No card detected or invalid card type!");
    }
}

SDError SDCardDriver_SPI::mount() {
    if (!initialized) {
        setError(SDError::NotInitialized);
        return lastError;
    }
    
    if (mounted) {
        return SDError::None;
    }
    
    if (!SD.begin(csPin, *spi)) {
        setError(SDError::MountFailed);
        return lastError;
    }
    
    mounted = true;
    return SDError::None;
}

SDError SDCardDriver_SPI::unmount() {
    if (!mounted) {
        return SDError::None;
    }
    
    SD.end();
    mounted = false;
    return SDError::None;
}

void SDCardDriver_SPI::reset() {
    if (!initialized) return;
    
    if (mounted) {
        unmount();
    }
    
    if (spi) {
        spi->end();
        delay(100);
        spi->begin();
    }
    
    initializePins();
    initialized = false;
    mounted = false;
    lastError = SDError::None;
    
    initialize(config);
}

SDError SDCardDriver_SPI::openFile(const char* path, const char* mode) {
    if (!checkMounted("openFile")) {
        return lastError;
    }
    
    if (currentFile) {
        currentFile.close();
    }
    
    currentFile = SD.open(path, mode);
    if (!currentFile) {
        setError(SDError::IOError);
        return lastError;
    }
    
    return SDError::None;
}

SDError SDCardDriver_SPI::closeFile() {
    if (currentFile) {
        currentFile.close();
    }
    return SDError::None;
}

SDError SDCardDriver_SPI::writeData(const uint8_t* data, size_t length) {
    if (!checkMounted("writeData")) {
        return lastError;
    }
    
    if (!currentFile) {
        setError(SDError::IOError);
        return lastError;
    }
    
    if (currentFile.write(data, length) != length) {
        setError(SDError::IOError);
        return lastError;
    }
    
    return SDError::None;
}

SDError SDCardDriver_SPI::readData(uint8_t* buffer, size_t length, size_t* bytesRead) {
    if (!checkMounted("readData")) {
        return lastError;
    }
    
    if (!currentFile) {
        setError(SDError::IOError);
        return lastError;
    }
    
    *bytesRead = currentFile.read(buffer, length);
    return SDError::None;
}

bool SDCardDriver_SPI::performSelfTest() {
    if (!mounted || !initialized) {
        Serial.println("SD Card: Not initialized!");
        return false;
    }
    
    const char* testFile = "/_test_.txt";
    const char* testData = "SD Card Test 123";
    bool success = true;
    
    // Write test
    if (openFile(testFile, "w") != SDError::None) {
        Serial.println("Failed to open test file!");
        return false;
    }
    
    if (writeData((uint8_t*)testData, strlen(testData)) != SDError::None) {
        Serial.println("Failed to write!");
        closeFile();
        return false;
    }
    closeFile();
    
    // Read test
    uint8_t buffer[32];
    size_t bytesRead;
    if (openFile(testFile, "r") != SDError::None ||
        readData(buffer, sizeof(buffer), &bytesRead) != SDError::None) {
        success = false;
    }
    closeFile();
    
    // Cleanup
    remove(testFile);
    
    return success;
}

}} // namespace Hardware::SDCard