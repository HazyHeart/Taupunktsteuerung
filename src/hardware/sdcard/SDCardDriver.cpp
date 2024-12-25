#include "hardware/sdcard/SDCardDriver.h"

namespace Hardware {
namespace SDCard {

const char* SDCardDriver::getErrorString(SDError error) const {
    switch(error) {
        case SDError::None: return "No error";
        case SDError::NotInitialized: return "Not initialized";
        case SDError::CardMissing: return "Card missing";
        case SDError::MountFailed: return "Mount failed";
        case SDError::WriteProtected: return "Write protected";
        case SDError::FileSystemError: return "Filesystem error";
        case SDError::IOError: return "I/O error";
        case SDError::SPIError: return "SPI error";
        case SDError::PinConflict: return "Pin conflict";
        case SDError::InvalidPin: return "Invalid pin";
        case SDError::OutOfSpace: return "Out of space";
        case SDError::InvalidParameter: return "Invalid parameter";
        default: return "Unknown error";
    }
}

}} // namespace Hardware::SDCard
