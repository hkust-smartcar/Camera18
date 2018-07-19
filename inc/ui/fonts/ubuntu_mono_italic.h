//
// Created by Daniel on 19/2/2018.
//

#ifndef LIBUI_UBUNTU_MONO_ITALIC_H
#define LIBUI_UBUNTU_MONO_ITALIC_H

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


#endif //LIBUI_UBUNTU_MONO_ITALIC_H
