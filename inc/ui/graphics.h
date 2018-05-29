//
// Created by Daniel on 22/2/2018.
//

#ifndef INNO14_D_2017_INNO_GRAPHICS_H
#define INNO14_D_2017_INNO_GRAPHICS_H

#include <cstdint>

namespace ui {
    namespace graphics {
        enum Direction {
            UP = 0,
            DOWN,
            LEFT,
            RIGHT
        };

        struct Position {
            int16_t x;
            int16_t y;
        };
    }
}

#endif //INNO14_D_2017_INNO_GRAPHICS_H
