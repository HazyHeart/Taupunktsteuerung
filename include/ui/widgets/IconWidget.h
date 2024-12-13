// ui/widgets/IconWidget.h
#pragma once
#include "../Widget.h"

namespace UI {
namespace Widgets {

class IconWidget : public Widget {
public:
    IconWidget(const Rect& bounds) 
        : Widget(bounds)
        , iconData_(nullptr)
        , width_(0)
        , height_(0)
    {}
    
    void setIcon(const uint8_t* icon, uint8_t width, uint8_t height) {
        iconData_ = icon;
        width_ = width;
        height_ = height;
        markDirty();
    }
    
    void render(Hardware::Display::DisplayDriver& display) override {
        if (!isVisible() || !iconData_) return;
        
        // Korrigiert zu drawBitmap
        display.drawBitmap(bounds_.x, bounds_.y, iconData_, width_, height_);
    }

private:
    const uint8_t* iconData_;
    uint8_t width_;
    uint8_t height_;
};

}} // namespace UI::Widgets