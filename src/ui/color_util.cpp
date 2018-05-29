//
// Created by Daniel on 21/2/2018.
//

#include "ui/color_util.h"

namespace ui {
    ColorUtil::RGBDouble::RGBDouble(double r_, double g_, double b_)
            : r(r_), g(g_), b(b_) {}

    ColorUtil::HSV ColorUtil::RGBDouble::toHSV(ColorUtil::RGBDouble rgbDouble) {
        HSV hsv(0, 0, 0);
        double min, max, delta;

        min = rgbDouble.r < rgbDouble.g ? rgbDouble.r : rgbDouble.g;
        min = min < rgbDouble.b ? min : rgbDouble.b;

        max = rgbDouble.r > rgbDouble.g ? rgbDouble.r : rgbDouble.g;
        max = max > rgbDouble.b ? max : rgbDouble.b;

        hsv.v = max;                                // v
        delta = max - min;
        if (delta < 0.00001) {
            hsv.s = 0;
            hsv.h = 0; // undefined, maybe nan?
            return hsv;
        }
        if (max > 0.0) { // NOTE: if Max is == 0, this divide would cause a crash
            hsv.s = (delta / max);                  // s
        } else {
            // if max is 0, then r = g = b = 0
            // s = 0, h is undefined
            hsv.s = 0.0;
            hsv.h = NAN;                            // its now undefined
            return hsv;
        }
        if (rgbDouble.r >= max)                           // > is bogus, just keeps compilor happy
            hsv.h = (rgbDouble.g - rgbDouble.b) / delta;        // between yellow & magenta
        else if (rgbDouble.g >= max)
            hsv.h = 2.0 + (rgbDouble.b - rgbDouble.r) / delta;  // between cyan & yellow
        else
            hsv.h = 4.0 + (rgbDouble.r - rgbDouble.g) / delta;  // between magenta & cyan

        hsv.h *= 60.0;                              // degrees

        if (hsv.h < 0.0)
            hsv.h += 360.0;

        return hsv;
    }

    ColorUtil::RGB24::RGB24(uint16_t r_, uint16_t g_, uint16_t b_)
            : r(r_), g(g_), b(b_) {}

    ColorUtil::RGB565 ColorUtil::RGB24::toRGB565(ColorUtil::RGB24 rgb24) {
        return {
                (uint8_t) (rgb24.r * 31 / 255),
                (uint8_t) (rgb24.g * 63 / 255),
                (uint8_t) (rgb24.b * 31 / 255)
        };
    }

    uint16_t ColorUtil::RGB24::toUInt16(ColorUtil::RGB565 rgb565) {
        return rgb565.r << 11 ^ rgb565.g << 5 ^ rgb565.b;
    }

    ColorUtil::RGB565::RGB565(uint8_t r_, uint8_t g_, uint8_t b_)
            : r(r_), g(g_), b(b_) {}

    ColorUtil::RGB565::RGB565(uint16_t rgb565) {
        uint16_t red_mask = 0xF800;
        uint16_t green_mask = 0x07E0;
        uint16_t blue_mask = 0x001F;
        r = (uint8_t) ((rgb565 & red_mask) >> 11);
        g = (uint8_t) ((rgb565 & green_mask) >> 5);
        b = (uint8_t) ((rgb565 & blue_mask));
    }

    ColorUtil::RGB24 ColorUtil::RGB565::toRGB24(uint16_t rgb565) {
        uint16_t red_mask = 0xF800;
        uint16_t green_mask = 0x07E0;
        uint16_t blue_mask = 0x001F;
        return {
                (uint16_t) ((rgb565 & red_mask) >> 11 * 255 / 31),
                (uint16_t) ((rgb565 & green_mask) >> 5 * 255 / 63),
                (uint16_t) ((rgb565 & blue_mask) >> 5 * 255 / 31)
        };
    }

    uint16_t ColorUtil::rgb565Mix(ColorUtil::RGB565 &color_from, ColorUtil::RGB565 &color_to, double_t percent) {
        uint8_t r = (uint8_t) round(color_to.r * percent + color_from.r * (1-percent));
        uint8_t g = (uint8_t) round(color_to.g * percent + color_from.g * (1-percent));
        uint8_t b = (uint8_t) round(color_to.b * percent + color_from.b * (1-percent));

        return r << 11 ^ g << 5 ^ b;
    }

    uint16_t ColorUtil::rgb565Mix(uint16_t &color_from, uint16_t &color_to, double_t percent) {
        ColorUtil::RGB565 from = RGB565(color_from);
        ColorUtil::RGB565 to = RGB565(color_to);

        return rgb565Mix(from, to, percent);
    }

    ColorUtil::HSV::HSV(double h_, double s_, double v_) {
        h = h_;
        s = s_;
        v = v_;
    }

    ColorUtil::RGBDouble ColorUtil::HSV::toDouble(ColorUtil::HSV hsv) {
        double r;
        double g;
        double b;

        double& h = hsv.h;
        double& s = hsv.s;
        double& v = hsv.v;

        auto i = (uint8_t) floor(h * 6);
        double f = h * 6 - i;
        double p = (v * (1 - s));
        double q = (v * (1 - f * s));
        double t = (v * (1 - (1 - f) * s));

        switch (i % 6) {
            case 0: r = v, g = t, b = p; break;
            case 1: r = q, g = v, b = p; break;
            case 2: r = p, g = v, b = t; break;
            case 3: r = p, g = q, b = v; break;
            case 4: r = t, g = p, b = v; break;
            case 5: r = v, g = p, b = q; break;
            default: r = 0; g = 0; b = 0; break;
        }

        return {r, g, b};
    }

    ColorUtil::RGB565 ColorUtil::HSV::toRGB565(ColorUtil::HSV hsv) {
        RGBDouble rgbDouble = toDouble(hsv);
        return {
                (uint8_t) round(rgbDouble.r * 0x1F),
                (uint8_t) round(rgbDouble.g * 0x3F),
                (uint8_t) round(rgbDouble.b * 0x1F)
        };
    }

    ColorUtil::RGB24 ColorUtil::HSV::toRGB24(ColorUtil::HSV hsv) {
        RGBDouble rgbDouble = toDouble(hsv);
        return {
                (uint8_t) round(rgbDouble.r * 255),
                (uint8_t) round(rgbDouble.g * 255),
                (uint8_t) round(rgbDouble.b * 255)
        };
    }

    ColorUtil::HSV &ColorUtil::HSV::darken(double percent) {
        v -= percent;
        v = std::max(1.0, std::min(0.0, v));
        return *this;
    }

    ColorUtil::HSV &ColorUtil::HSV::lighten(double percent) {
        v += percent;
        v = std::max(1.0, std::min(0.0, v));
        return *this;
    }
}