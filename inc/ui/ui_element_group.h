//
// Created by Daniel on 19/2/2018.
//

#ifndef INNO14_D_2017_INNO_SCREEN_H
#define INNO14_D_2017_INNO_SCREEN_H

#include <vector>
#include <cstdint>
#include "ui/ui_element.h"

namespace ui {

    class UIElementGroup: public UIElement {
    public:
        void render() override;

        void addElement(UIElement* ui_element) {
            ui_element_ptrs.push_back(ui_element);
        }

        std::vector<UIElement*> getElements() {
            return ui_element_ptrs;
        }
    private:
        /**
         * On screen elements
         */
        std::vector<UIElement*> ui_element_ptrs;

    };
}


#endif //INNO14_D_2017_INNO_SCREEN_H
