//
// Created by Daniel on 22/2/2018.
//

#include "ui/color_schemes/color_scheme.h"

ui::ColorScheme::ColorScheme() {

}

ui::ColorScheme::ColorScheme(ui::ColorScheme::Theme theme) :
        PRIMARY(theme.PRIMARY),
        PRIMARY_LIGHTER(theme.PRIMARY_LIGHTER),
        PRIMARY_DARKER(theme.PRIMARY_DARKER),
        SUCCESS(theme.SUCCESS),
        DANGER(theme.DANGER),
        BACKGROUND_LIGHT(theme.BACKGROUND_LIGHT),
        BACKGROUND_LIGHTER(theme.BACKGROUND_LIGHTER),
        GRAY(theme.GRAY),
        GRAY_LIGHTER(theme.GRAY_LIGHTER),
        GRAY_DARKER(theme.GRAY_DARKER),
        WHITE(theme.WHITE),
        BLACK(theme.BLACK) {}
