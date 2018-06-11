//
// Created by Daniel on 29/5/2018.
//

#include <ui/graphics.h>
#include <ui/color_util.h>
#include "adapters/screen_adapter.h"

void adapters::ScreenAdapter::setRegion(ui::graphics::Rectangle rectangle) {
    lcd_ptr->SetRegion({rectangle.x, rectangle.y, rectangle.w, rectangle.h});
}

ui::graphics::Rectangle adapters::ScreenAdapter::getRegion() {
    libsc::Lcd::Rect rect = lcd_ptr->GetRegion();
    return {rect.x, rect.y, rect.w, rect.h};
}

void adapters::ScreenAdapter::clear() {
    lcd_ptr->Clear();
}

void adapters::ScreenAdapter::fill(uint16_t rgb565) {
    lcd_ptr->FillColor(rgb565);
}

uint16_t adapters::ScreenAdapter::getWidth() {
    return 128;
}

uint16_t adapters::ScreenAdapter::getHeight() {
    return 160;
}
