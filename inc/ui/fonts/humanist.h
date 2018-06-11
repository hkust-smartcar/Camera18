//
// Created by Daniel on 19/2/2018.
//

#ifndef LIBUI_HUMANIST_H
#define LIBUI_HUMANIST_H

#include <cstdint>
#include "ui/font.h"

namespace ui {
    namespace fonts {
        // Font data for Humanist 9pt
        extern const uint8_t humanist_9ptBitmaps[];
        extern const FONT_INFO humanist_9ptFontInfo;
        extern const FONT_CHAR_INFO humanist_9ptDescriptors[];

        class Humanist: public Font {
        public:
            Humanist();
        };
    }
}

#endif //LIBUI_HUMANIST_H
