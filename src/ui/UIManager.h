// ui/UIManager.h
#pragma once
#include "hardware/display/DisplayDriver.h"
#include "Widget.h"
#include <map>
#include <string>

namespace UI {

class UIManager {
public:
    UIManager(Hardware::Display::DisplayDriver& display) 
        : display_(display)
        , activeScreen_(nullptr)
        , needsRedraw_(true)
    {}
    
    void registerScreen(const String& name, Screen* screen) {
        if (screen) {
            screens_[name] = screen;
        }
    }
    
    bool activateScreen(const String& name) {
        auto it = screens_.find(name);
        if (it != screens_.end()) {
            if (activeScreen_) {
                activeScreen_->onDeactivate();
            }
            activeScreen_ = it->second;
            activeScreen_->onActivate();
            needsRedraw_ = true;
            return true;
        }
        return false;
    }
    
    void update() {
    if (activeScreen_) {
        activeScreen_->update();
        if (activeScreen_->isDirty() || needsRedraw_) {
            display_.clear();
            activeScreen_->render(display_);
            display_.updateDisplay();  // Geändert von display() zu updateDisplay()
            needsRedraw_ = false;
        }
    }
}

private:
    Hardware::Display::DisplayDriver& display_;
    Screen* activeScreen_;
    std::map<String, Screen*> screens_;
    bool needsRedraw_;
};

} // namespace UI
