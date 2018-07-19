//
// Created by Daniel on 21/2/2018.
//

#ifndef LIBUI_UI_BASE_H
#define LIBUI_UI_BASE_H

#include "graphics.h"

namespace ui {

    enum Visibility {
        VISIBLE,
        HIDDEN
    };

    class UIBase {
    public:

        virtual void render();

        /**
         * Sets the ui region
         * @param rect
         */
        void setRegion(graphics::Rectangle rect);

        /**
         * Sets the ui region
         * @param rect
         */
        void setRegion(uint32_t x, uint32_t y, uint32_t w, uint32_t h);

        /**
         * Retrieves the region where text is drawn.
         * @return
         */
        graphics::Rectangle getRegion();

        void setVisibility(Visibility visibility);

        Visibility getVisibility();

    protected:
        graphics::Rectangle ui_region;
        Visibility visibility = VISIBLE;
    };
}


#endif //LIBUI_UI_BASE_H
