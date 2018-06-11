//
// Created by Daniel on 21/2/2018.
//

#include <ui/graphics.h>
#include "ui/ui_base.h"

void ui::UIBase::setRegion(ui::graphics::Rectangle rect) {
    ui_region = rect;
}

void ui::UIBase::setRegion(uint32_t x, uint32_t y, uint32_t w, uint32_t h) {
    ui_region = graphics::Rectangle{x, y, w, h};
}

ui::graphics::Rectangle ui::UIBase::getRegion() {
    return ui_region;
}

void ui::UIBase::setVisibility(Visibility visibility) {
    this->visibility = visibility;
}

ui::Visibility ui::UIBase::getVisibility() {
    return visibility;
}

void ui::UIBase::render() {

}
