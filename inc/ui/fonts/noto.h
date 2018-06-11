//
// Created by Daniel on 19/2/2018.
//

#ifndef LIBUI_NOTO_H
#define LIBUI_NOTO_H

#include <cstdint>
#include "ui/font.h"

namespace ui {
    namespace fonts {
        // Font data for Noto Sans Regular 32pt
        extern const uint8_t noto_32ptBitmaps[];
        extern const FONT_INFO noto_32ptFontInfo;
        extern const FONT_CHAR_INFO noto_32ptDescriptors[];

        class Noto: public Font {
        public:
            Noto();
        };
    }
}


#endif //LIBUI_NOTO_H
