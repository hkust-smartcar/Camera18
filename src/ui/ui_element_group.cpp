//
// Created by Daniel on 19/2/2018.
//

#include "ui/ui_element_group.h"

void ui::UIElementGroup::render() {
    for(UIElement* ui_element_ptr: ui_element_ptrs)
        ui_element_ptr->render();
}
