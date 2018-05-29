//
// Created by Daniel on 19/2/2018.
//

#ifndef INNO14_D_2017_INNO_NOTO_H
#define INNO14_D_2017_INNO_NOTO_H

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


#endif //INNO14_D_2017_INNO_NOTO_H
