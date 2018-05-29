//
// Created by Daniel on 21/2/2018.
//

#ifndef INNO14_D_2017_INNO_COLOR_H
#define INNO14_D_2017_INNO_COLOR_H


#include <cstdint>
#include <cmath>
#include <algorithm>

namespace ui {
    /**
     * Color Utility for RGB565, RGB24, RGBDouble and HSV
     */
    namespace ColorUtil {
        struct RGBDouble;
        struct RGB565;
        struct RGB24;
        struct HSV;

        /**
         * A specified color where each is from 0.0 to 1.0.
         */
        struct RGBDouble {
            double r = 1;
            double g = 1;
            double b = 1;

            RGBDouble(double r_, double g_, double b_);

            HSV toHSV(RGBDouble rgbDouble);
        };

        struct RGB24 {
            uint16_t r = 255;
            uint16_t g = 255;
            uint16_t b = 255;

            RGB24(uint16_t r_, uint16_t g_, uint16_t b_);

            /**
             * Converts a True Color stored in an RGB24 to an RGB-565 uint16_t structure.
             * @param rgb24
             * @return An RGB-565 color in RGB565
             */
            RGB565 toRGB565(RGB24 rgb24);

            /**
             * Converts an RGB-565 color stored in an RGB24 to a uint16_t structure.
             * @param rgb565
             * @return An RGB-565 color in uint16_t
             */
            uint16_t toUInt16(RGB565 rgb565);
        };

        struct RGB565 {
            uint8_t r = 0x1F;
            uint8_t g = 0x3F;
            uint8_t b = 0x1F;

            RGB565(uint8_t r_, uint8_t g_, uint8_t b_);

            /**
             * Converts an RGB-565 color stored in a uint16_t to an RGB24 structure.
             * @param rgb565
             * @return An RGB-565 color in RGB565
             */
            explicit RGB565(uint16_t rgb565);

            /**
             * Converts an RGB-565 color stored in a uint16_t to a True Color RGB24 structure.
             * @param rgb565
             * @return An RGB-24 color in RGB24
             */
            RGB24 toRGB24(uint16_t rgb565);
        };

        struct HSV {
            double h = 0;
            double s = 0;
            double v = 1;

            HSV(double h_, double s_, double v_);

            RGBDouble toDouble(HSV hsv);

            RGB565 toRGB565(HSV hsv);

            RGB24 toRGB24(HSV hsv);

            HSV& darken(double percent);

            HSV& lighten(double percent);
        };

        /**
         * Mixes 2 RGB565 colors on a scale
         * @param color_from
         * @param color_to
         * @param percent A double from 0 to 1
         * @return
         */
        uint16_t rgb565Mix(RGB565 &color_from, RGB565 &color_to, double_t percent);

        /**
         * Mixes 2 RGB565 colors on a scale
         * @param color_from
         * @param color_to
         * @param percent A double from 0 to 1
         * @return
         */
        uint16_t rgb565Mix(uint16_t &color_from, uint16_t &color_to, double_t percent);
    }
}


#endif //INNO14_D_2017_INNO_COLOR_H
