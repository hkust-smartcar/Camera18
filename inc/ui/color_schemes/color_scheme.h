//
// Created by Daniel on 22/2/2018.
//

#ifndef INNO14_D_2017_INNO_COLOR_SCHEME_H
#define INNO14_D_2017_INNO_COLOR_SCHEME_H


#include <cstdint>

namespace ui {
    class ColorScheme {
    public:
        struct Theme {
            uint16_t PRIMARY = 0x251D;
            uint16_t PRIMARY_LIGHTER = 0xC73E;
            uint16_t PRIMARY_DARKER = 0x12D0;
            uint16_t SUCCESS = 0x062A;
            uint16_t DANGER = 0xDA09;
            uint16_t BACKGROUND_LIGHT = 0xF7DE;
            uint16_t BACKGROUND_LIGHTER = 0xFFFF;
            uint16_t BODY = 0x0000;
            uint16_t GRAY = 0xA576;
            uint16_t GRAY_LIGHTER = 0xDEFB;
            uint16_t GRAY_DARKER = 0x73CF;
            uint16_t WHITE = 0xFFFF;
            uint16_t BLACK = 0x0000;
        };

        explicit ColorScheme();

        explicit ColorScheme(Theme theme);

        uint16_t PRIMARY = 0x251D;
        uint16_t PRIMARY_LIGHTER = 0xC73E;
        uint16_t PRIMARY_DARKER = 0x12D0;
        uint16_t SUCCESS = 0x062A;
        uint16_t DANGER = 0xDA09;
        uint16_t BACKGROUND_LIGHT = 0xF7DE;
        uint16_t BACKGROUND_LIGHTER = 0xFFFF;
        uint16_t BODY = 0x0000;
        uint16_t GRAY = 0xA576;
        uint16_t GRAY_LIGHTER = 0xDEFB;
        uint16_t GRAY_DARKER = 0x73CF;
        uint16_t WHITE = 0xFFFF;
        uint16_t BLACK = 0x0000;
    };
}

#endif //INNO14_D_2017_INNO_COLOR_SCHEME_H
