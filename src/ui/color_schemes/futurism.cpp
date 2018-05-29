//
// Created by Daniel on 22/2/2018.
//

#include "ui/color_schemes/futurism.h"

namespace ui {
    namespace color_schemes {
        Futurism::Futurism() : ColorScheme([](){
            Theme theme;
            theme.PRIMARY = 0x251D;
            theme.PRIMARY_LIGHTER = 0xC73E;
            theme.PRIMARY_DARKER = 0x12D0;
            theme.SUCCESS = 0x0F0C;
            theme.DANGER = 0xE926;
            theme.BACKGROUND_LIGHT = 0xF7DE;
            theme.BACKGROUND_LIGHTER = 0xFFFF;
            theme.BODY = 0x0000;
            theme.GRAY = 0xA576;
            theme.GRAY_LIGHTER = 0xDEFB;
            theme.GRAY_DARKER = 0x73CF;
            theme.WHITE = 0xFFFF;
            theme.BLACK = 0x0000;
            return theme;
        }()) {}

    }
}