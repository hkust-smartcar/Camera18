//
// Created by Daniel on 29/5/2018.
//

#ifndef LIBUI_SCREEN_ADAPTER_H
#define LIBUI_SCREEN_ADAPTER_H

#include <ui/graphics.h>
#include <ui/color_util.h>
#include <ui/reactive/reactive.h>

namespace ui {
    namespace adapters {
        class ScreenAdapterInterface {
        public:
            virtual void setRegion(ui::graphics::Rectangle) = 0;
            virtual void setRegion(uint32_t x, uint32_t y, uint32_t w, uint32_t h) {setRegion({x, y, w, h});};
            virtual ui::graphics::Rectangle getRegion() = 0;
            virtual void clear() = 0;
            virtual void fill(uint16_t) = 0;
            virtual void fill(ui::ColorUtil::RGB565 rgb565) {fill(rgb565.toUInt16());};
            virtual uint16_t getWidth() = 0;
            virtual uint16_t getHeight() = 0;
        };
    }
}


#endif //LIBUI_SCREEN_ADAPTER_H
