//
// Created by Daniel on 19/2/2018.
//

#ifndef INNO14_D_2017_INNO_FONT_INFO_H
#define INNO14_D_2017_INNO_FONT_INFO_H

#include <map>

namespace ui {
    struct FONT_CHAR_INFO {
        uint8_t width;
        uint16_t offset;
    };

    struct FONT_INFO {
        uint8_t height; //  Character height
        char start_character; //  Start character
        char end_character;
        uint8_t space_width; //  Width, in pixels, of space character
        uint8_t tracking;
        const FONT_CHAR_INFO* descriptor; //  Character descriptor array
        const uint8_t* ubuntuMono_10ptBitmaps; //  Character bitmap array
    };

    /**
     * Declares a new font, where bitmaps, font_info and descriptors point to types generated by The Dot Factory.
     * The font should be stored in the font repo for better management.
     */
    class Font {
    public:
        enum FontType {
            PROPORTIONAL,
            MONOSPACED
        };

        Font(const uint8_t* bitmaps, const FONT_INFO* font_info, const FONT_CHAR_INFO* descriptors);

        Font(const uint8_t* bitmaps, const FONT_INFO* font_info, const FONT_CHAR_INFO* descriptors, bool monospaced);

        const uint8_t* bitmaps;
        const FONT_INFO* font_info_ptr;
        const FONT_CHAR_INFO* descriptors;
        const FontType fontType = PROPORTIONAL;

        const FONT_CHAR_INFO& getCharInfo(char c) const;

        const FONT_INFO& getFontInfo() const;
    };
}

#endif //INNO14_D_2017_INNO_FONT_INFO_H
