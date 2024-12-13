// include/ui/widgets/Label.h
#pragma once
#include "../Widget.h"

namespace UI {
namespace Widgets {

class Label : public Widget {
public:
    Label(const Rect& bounds) 
        : Widget(bounds)
        , text_("")
    {}
    
    void setText(const String& text) {
        if (text_ != text) {
            text_ = text;
            markDirty();
        }
    }
    
    void render(Hardware::Display::DisplayDriver& display) override {
        if (!isVisible()) return;
        
        // Vereinfachte Text-Darstellung ohne Font-Handling
        display.drawString(bounds_.x, bounds_.y, text_);
    }

private:
    String text_;
};

}} // namespace UI::Widgets