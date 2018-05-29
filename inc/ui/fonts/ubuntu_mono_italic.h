//
// Created by Daniel on 19/2/2018.
//

#ifndef INNO14_D_2017_INNO_UBUNTU_MONO_ITALIC_H
#define INNO14_D_2017_INNO_UBUNTU_MONO_ITALIC_H

#include <cstdint>
#include "ui/font.h"

namespace ui {
    namespace fonts {
        // Font data for Ubuntu Mono Italic 10pt
        extern const uint8_t ubuntuMonoItalic_10ptBitmaps[];
        extern const FONT_INFO ubuntuMonoItalic_10ptFontInfo;
        extern const FONT_CHAR_INFO ubuntuMonoItalic_10ptDescriptors[];

        class UbuntuMonoItalic: public Font {
        public:
            UbuntuMonoItalic();
        };
    }
}


#endif //INNO14_D_2017_INNO_UBUNTU_MONO_ITALIC_H
