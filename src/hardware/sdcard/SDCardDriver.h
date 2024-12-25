#pragma once
#include <cstdint>
#include <Arduino.h>
#include <SPI.h>  // Wichtig für SPIClass

namespace Hardware {
namespace SDCard {

struct SDCardStatus {
    bool mounted;
    bool writeProtected;
    uint64_t totalBytes;
    uint64_t usedBytes;
    uint64_t freeBytes;
    String fileSystem;
    String label;
};

struct SDConfig {
    uint8_t csPin;
    uint8_t sckPin;
    uint8_t mosiPin;
    uint8_t misoPin;
    uint32_t spiFreq;
    SPIClass* spi;
    bool formatIfMounted;
    const char* mountPoint;
};

enum class SDError {
    None,
    NotInitialized,
    CardMissing,
    MountFailed,
    WriteProtected,
    FileSystemError,
    IOError,
    SPIError,
    PinConflict,
    InvalidPin,
    OutOfSpace,
    InvalidParameter,
    TimeoutError
};

class SDCardDriver {
public:
    virtual ~SDCardDriver() = default;
    
    // Lifecycle Management
    virtual SDError initialize(const SDConfig& config) = 0;
    virtual bool isInitialized() const = 0;
    virtual void reset() = 0;
    
    // Mount Management
    virtual SDError mount() = 0;
    virtual SDError unmount() = 0;
    virtual bool isMounted() const = 0;
    
    // File Operations
    virtual SDError openFile(const char* path, const char* mode) = 0;
    virtual SDError closeFile() = 0;
    virtual SDError writeData(const uint8_t* data, size_t length) = 0;
    virtual SDError readData(uint8_t* buffer, size_t length, size_t* bytesRead) = 0;
    virtual SDError seek(size_t offset) = 0;
    virtual SDError sync() = 0;
    
    // Directory Operations
    virtual SDError createDir(const char* path) = 0;
    virtual SDError removeDir(const char* path) = 0;
    virtual SDError listDir(const char* path) = 0;
    
    // File Management
    virtual bool exists(const char* path) const = 0;
    virtual SDError remove(const char* path) = 0;
    virtual SDError rename(const char* oldPath, const char* newPath) = 0;
    
    // Status
    virtual SDError getStatus(SDCardStatus& status) = 0;
    virtual SDError getLastError() const = 0;
    virtual bool performSelfTest() = 0;
protected:
    const char* getErrorString(SDError error) const;
};

}} // namespace Hardware::SDCard
