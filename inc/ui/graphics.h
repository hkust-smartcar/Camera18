//
// Created by Daniel on 22/2/2018.
//

#ifndef LIBUI_GRAPHICS_H
#define LIBUI_GRAPHICS_H

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

        struct Rectangle {
            uint32_t x;
            uint32_t y;
            uint32_t w;
            uint32_t h;
        };
    }
}

#endif //LIBUI_GRAPHICS_H
