//
// Created by Daniel on 22/2/2018.
//

#ifndef LIBUI_ICONS_H
#define LIBUI_ICONS_H

#include <ui/context.h>
#include "ui/graphics.h"


namespace ui {

    using namespace graphics;

    class Icons {
    public:
        /**
         * Draws a caret
         *
         * @param x
         * @param y
         * @param color RGB-565 color
         * @param direction The pointing direction
         * @param size Thickness of the caret in pixels along the pointing direction.
         *
         * @example
         * drawCaret(0, 0, 0x0000, RIGHT, 4);<br>
         *
         * @code
         * #
         * ##
         * ###
         * #### RIGHT
         * ###
         * ##
         * #
         * @endcode
         *
         * @see ui::graphics::Direction
         */
        static void drawCaret(uint32_t x, uint32_t y, uint16_t color, Direction direction, uint16_t size);

        /**
         * Draws a right angle triangle.
         *
         * @param x
         * @param y
         * @param color RGB-565 color
         * @param direction Direction of growth. The 90 deg angle opens in the anti-clockwise direction. In other words,
         * there will be a straight edge perpendicular to the direction, going along the direction.
         * @param size The width and the height of the triangle in pixels.
         *
         * @code
         * | #
         * | ##
         * | ###
         * | ####
         * V #####
         * --DOWN->
         *
         *     #^
         *    ##|
         *   ###RIGHT
         *  ####|
         * #####|
         * ---->|
         * @endcode
         *
         */
        static void drawTriangle(uint32_t x, uint32_t y, uint16_t color, Direction direction, uint16_t size);

        /**
         * Draws a battery gauge
         *
         * @param x
         * @param y
         * @param color RGB-565 color.
         * @param width Width of the battery gauge.
         * @param height Width of the battery gauge.
         *
         * @code
         * #########
         * #       #
         * #       ##
         * #       ##
         * #       #
         * #########
         * @endcode
         */
        static void drawBatteryGauge(uint32_t x, uint32_t y, uint16_t color, uint16_t width, uint32_t height);
    };
}


#endif //LIBUI_ICONS_H
