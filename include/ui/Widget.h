// ui/Widget.h
#pragma once
#include <Arduino.h>
#include "hardware/display/DisplayDriver.h"
#include <vector>

namespace UI {

// Basis-Geometrie
struct Rect {
    int16_t x, y, width, height;
    
    bool contains(int16_t px, int16_t py) const {
        return px >= x && px < x + width && 
               py >= y && py < y + height;
    }
};

// Widget Basis-Klasse
class Widget {
public:
    Widget(const Rect& bounds) : bounds_(bounds), dirty_(true), visible_(true) {}
    virtual ~Widget() = default;
    
    // Lifecycle
    virtual void update() { }
    virtual void render(Hardware::Display::DisplayDriver& display) = 0;
    
    // Geometrie
    const Rect& getBounds() const { return bounds_; }
    void setBounds(const Rect& bounds) { 
        bounds_ = bounds;
        markDirty();
    }
    
    // Sichtbarkeit
    bool isVisible() const { return visible_; }
    void setVisible(bool visible) {
        if (visible_ != visible) {
            visible_ = visible;
            markDirty();
        }
    }
    
    // Dirty State
    bool isDirty() const { return dirty_; }
    void markDirty() { dirty_ = true; }
    void clearDirty() { dirty_ = false; }

protected:
    Rect bounds_;
    bool dirty_;
    bool visible_;
};

// Container für mehrere Widgets
class Container : public Widget {
public:
    Container(const Rect& bounds) : Widget(bounds) {}
    virtual ~Container() = default;
    
    void addWidget(Widget* widget) {
        if (widget) {
            widgets_.push_back(widget);
            markDirty();
        }
    }
    
    void update() override {
        for (auto* widget : widgets_) {
            if (widget && widget->isVisible()) {
                widget->update();
                if (widget->isDirty()) {
                    markDirty();
                }
            }
        }
    }
    
    void render(Hardware::Display::DisplayDriver& display) override {
        for (auto* widget : widgets_) {
            if (widget && widget->isVisible() && widget->isDirty()) {
                widget->render(display);
                widget->clearDirty();
            }
        }
        clearDirty();
    }
    
protected:
    std::vector<Widget*> widgets_;
};

// Screen Basis-Klasse
class Screen : public Container {
public:
    Screen() : Container({0, 0, 128, 64}) {}  // Standard OLED Größe
    virtual ~Screen() = default;
    
    virtual void onActivate() {}
    virtual void onDeactivate() {}
    
    // Event Handlers
    virtual void onEncoderRotate(int16_t delta) {}
    virtual void onEncoderPress() {}
    virtual void onEncoderLongPress() {}
};

} // namespace UI