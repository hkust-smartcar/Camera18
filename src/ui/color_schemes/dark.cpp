//
// Created by Daniel on 22/2/2018.
//

#include "ui/color_schemes/dark.h"

namespace ui {
    namespace color_schemes {
        Dark::Dark() : ColorScheme([](){
            Theme theme;
            theme.PRIMARY = 0x251D;             //#20A1EE
            theme.PRIMARY_LIGHTER = 0xC73E;     //#C5E6F6
            theme.PRIMARY_DARKER = 0x12D0;      //#105983
            theme.SUCCESS = 0x0F0C;             //#08E262
            theme.DANGER = 0xE926;              //#EE2431
            theme.BACKGROUND_LIGHT = 0xF7DE;    //#F6FAF6
            theme.BACKGROUND_LIGHTER = 0xFFFF;  //#FFFFFF
            theme.BODY = 0x0000;                //#000000
            theme.GRAY = 0xA576;                //#A4AEB4
            theme.GRAY_LIGHTER = 0xDEFB;        //#DEDEDE
            theme.GRAY_DARKER = 0x73CF;         //#73797B
            theme.WHITE = 0xFFFF;               //#FFFFFF
            theme.BLACK = 0x0000;               //#000000
            return theme;
        }()) {}

    }
}