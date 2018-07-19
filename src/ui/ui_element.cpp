//
// Created by Daniel on 19/2/2018.
//

#include "ui/ui_element.h"

void ui::UIElement::setParent(ui::UIElementGroup *ui_element_ptr) {
    this->parent_ptr = ui_element_ptr;
}

ui::UIElementGroup *ui::UIElement::getParent() {
    return parent_ptr;
}

void ui::UIElement::setBackgroundColor(uint16_t color) {
    this->background_color = color;
}

uint16_t ui::UIElement::getBackgroundColor() {
    return background_color;
}

void ui::UIElementGroup::render() {
    for(UIElement* ui_element_ptr: ui_element_ptrs)
        ui_element_ptr->render();
}

void ui::UIElementGroup::addElement(UIElement* ui_element_ptr) {
    ui_element_ptr->setParent(this);
    ui_element_ptrs.push_back(ui_element_ptr);
}

void ui::UIElementGroup::addElements(std::initializer_list<UIElement*> list) {
    for (UIElement* ui_element_ptr: list) {
        ui_element_ptr->setParent(this);
        ui_element_ptrs.push_back(ui_element_ptr);
    }
}

std::vector<ui::UIElement *> ui::UIElementGroup::getElements() {
    return ui_element_ptrs;
}

