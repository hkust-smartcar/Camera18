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
#include <ui/reactive/reactive.h>

namespace adapters {
    class ScreenAdapter: public virtual ui::adapters::ScreenAdapterInterface {
    public:
        ui::reactive::Reactive<uint16_t> width = ui::reactive::Reactive<uint16_t>(128);
        ui::reactive::Reactive<uint16_t> height = ui::reactive::Reactive<uint16_t>(160);

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
