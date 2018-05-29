//
// Created by Daniel on 19/2/2018.
//

#include "ui/ui_element.h"

void ui::UIElement::setParent(ui::UIElement *ui_element_ptr) {
    this->parent_ptr = ui_element_ptr;
}

ui::UIElement *ui::UIElement::getParent() {
    return parent_ptr;
}

void ui::UIElement::setBackgroundColor(uint16_t color) {
    this->background_color = color;
}

uint16_t ui::UIElement::getBackgroundColor() {
    return background_color;
}
