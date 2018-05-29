//
// Created by Daniel on 19/2/2018.
//

#ifndef INNO14_D_2017_INNO_UI_ELEMENT_H
#define INNO14_D_2017_INNO_UI_ELEMENT_H

#include <cstdint>
#include <ui/ui_base.h>
#include <libsc/lcd.h>

namespace ui {

    enum Layout {
        WRAP_CONTENT,
        MATCH_PARENT
    };

    enum Opacity {
        TRANSPARENT,
        OPAQUE
    };

    /**
     * Abstract UIElement class. The class is used for organizing on-screen elements
     */
    class UIElement: public virtual UIBase {
    public:

        void setParent(UIElement* ui_element_ptr);

        UIElement* getParent();

        /**
         * Sets background color in RGB565
         * @param color
         */
        void setBackgroundColor(uint16_t color);

        /**
         * Retrieves background color in RGB565
         * @return color in RGB565
         */
        uint16_t getBackgroundColor();

    protected:
        UIElement() = default;

        UIElement* parent_ptr = nullptr;
        Layout horizontal_layout = WRAP_CONTENT;
        Layout vertical_layout = WRAP_CONTENT;
        Opacity background_opacity = TRANSPARENT;
        uint16_t background_color = libsc::Lcd::kBlack;
    };
}


#endif //INNO14_D_2017_INNO_UI_ELEMENT_H
