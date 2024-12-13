// ui/screens/BootScreen.h
#pragma once
#include "ScreenTemplate.h"
#include "../widgets/Label.h"
#include <memory>

namespace UI {
namespace Screens {

class BootScreen : public ScreenTemplate<BootScreen> {
public:
    BootScreen();
    
    void setStatus(const String& status);
    void setProgress(uint8_t progress);
    
    void render(Hardware::Display::DisplayDriver& display) override;

private:
    String currentStatus;
    uint8_t currentProgress{0};
};

}} // namespace UI::Screens