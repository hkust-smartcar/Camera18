//
// Created by Daniel on 2/3/2018.
//

#ifndef LIBUI_TOOLBAR_H
#define LIBUI_TOOLBAR_H


#include "ui/context.h"
#include "ui/color_util.h"
#include "ui/ui_base.h"
#include "ui/icons.h"
#include "ui/text_block.h"
#include "ui/fonts/humanist.h"
#include <sstream>
#include <cassert>

namespace ui {
    /**
     * A simple bar with a name, and the optional icon on its left
     */
    class Toolbar: public UIBase {
    public:
        Toolbar();

        void render() override;

        void setName(std::string name);

        void setHasBackArrow(bool has_back_arrow);

        void reserveHSpace(uint32_t w);

    private:
        static const uint16_t PADDING = 18;
        static const uint8_t TOOLBAR_HEIGHT = 18;
        static const uint16_t TEXT_OFFSET = 3;
        static const uint16_t CARET_OFFSET = TEXT_OFFSET + 1;

        std::string name;

        bool has_back_arrow = false;

        TextBlock textBlockTitle;
    };
}


#endif //LIBUI_TOOLBAR_H
