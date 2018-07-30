//
// Created by Daniel on 29/5/2018.
//

#ifndef LIBUI_LIBSCCC_SCREEN_ADAPTER_H
#define LIBUI_LIBSCCC_SCREEN_ADAPTER_H

/**
 * Adapts to libsccc::Lcd class
 */
#include <ui/adapters/module_adapters/screen_interface.h>
#include <libsc/lcd.h>

namespace adapters {
    class ScreenAdapter: public virtual ui::adapters::ScreenAdapterInterface {
    public:
        void setRegion(ui::graphics::Rectangle) override;
        ui::graphics::Rectangle getRegion() override;
        void clear() override;
        void fill(uint16_t) override;
        uint16_t getWidth() override;
        uint16_t getHeight() override;

        libsc::Lcd* lcd_ptr = nullptr;
    };
}


#endif //LIBUI_LIBSCCC_SCREEN_ADAPTER_H
