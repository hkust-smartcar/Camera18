//
// Created by Daniel on 21/2/2018.
//

#ifndef INNO14_D_2017_INNO_UI_BASE_H
#define INNO14_D_2017_INNO_UI_BASE_H


#include <libsc/lcd.h>

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
        void setRegion(libsc::Lcd::Rect rect);

        /**
         * Sets the ui region
         * @param rect
         */
        void setRegion(Uint x, Uint y, Uint w, Uint h);

        /**
         * Retrieves the region where text is drawn.
         * @return
         */
        libsc::Lcd::Rect getRegion();

        void setVisibility(Visibility visibility);

        Visibility getVisibility();

    protected:
        libsc::Lcd::Rect ui_region{};
        Visibility visibility = VISIBLE;
    };
}


#endif //INNO14_D_2017_INNO_UI_BASE_H
