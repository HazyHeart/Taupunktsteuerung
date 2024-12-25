// ui/screens/ScreenTemplate.h
#pragma once
#include <Arduino.h>
#include "../Widget.h"
#include <memory>

namespace UI {
namespace Screens {

template<typename T>
class ScreenTemplate : public Screen {
public:
    // CRTP Pattern für statisches Interface
    T& derived() { return static_cast<T&>(*this); }
    const T& derived() const { return static_cast<const T&>(*this); }

protected:
    // Hilfsklasse für Layoutberechnung
    struct LayoutArea {
        int16_t x, y, width, height;
        
        LayoutArea split_horizontal(float ratio) const {
            int16_t split_height = static_cast<int16_t>(height * ratio);
            return {x, y, width, split_height};
        }
        
        LayoutArea split_vertical(float ratio) const {
            int16_t split_width = static_cast<int16_t>(width * ratio);
            return {x, y, split_width, height};
        }
        
        LayoutArea offset(int16_t dx, int16_t dy) const {
            return {
                static_cast<int16_t>(x + dx), 
                static_cast<int16_t>(y + dy), 
                width, 
                height
            };
        }
        
        LayoutArea inset(int16_t margin) const {
            return {
                static_cast<int16_t>(x + margin),
                static_cast<int16_t>(y + margin),
                static_cast<int16_t>(width - 2*margin),
                static_cast<int16_t>(height - 2*margin)
            };
        }
    };

    // Screen-Bereiche
    LayoutArea getHeaderArea() const { 
        return LayoutArea{0, 0, 128, 16}; 
    }
    
    LayoutArea getContentArea() const { 
        return LayoutArea{0, 16, 128, 32}; 
    }
    
    LayoutArea getFooterArea() const { 
        return LayoutArea{0, 48, 128, 16}; 
    }

    // Hilfsfunktionen für Text-Formatierung
    template<typename V>
    String formatValue(const V& value, int decimals) {
        if (std::isnan(value)) return "---";
        return String(value, decimals);
    }
};

}} // namespace UI::Screens
