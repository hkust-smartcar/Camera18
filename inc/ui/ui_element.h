//
// Created by Daniel on 19/2/2018.
//

#ifndef LIBUI_UI_ELEMENT_H
#define LIBUI_UI_ELEMENT_H

#include <cstdint>
#include <ui/ui_base.h>
#include <ui/context.h>

namespace ui {

    class UIElementGroup;

    enum Layout {
        WRAP_CONTENT,
        MATCH_PARENT
    };

    enum Opacity {
        TRANSPARENT,
        OPAQUE
    };

    /**
     * Abstract UIElement class. The class is used for organizing on-screen elements
     */
    class UIElement: public virtual UIBase {
    public:
        UIElementGroup* getParent();

        void setParent(UIElementGroup* ui_element_ptr);

        /**
         * Sets background color in RGB565
         * @param color
         */
        void setBackgroundColor(uint16_t color);

        /**
         * Retrieves background color in RGB565
         * @return color in RGB565
         */
        uint16_t getBackgroundColor();

    protected:
        UIElement() = default;

        UIElementGroup* parent_ptr = nullptr;
        Layout horizontal_layout = WRAP_CONTENT;
        Layout vertical_layout = WRAP_CONTENT;
        Opacity background_opacity = TRANSPARENT;
        uint16_t background_color = Context::color_scheme.BACKGROUND;
    };

    class UIElementGroup: public virtual UIElement {
    public:
        void render() override;

        void addElement(UIElement* ui_element_ptr);

        /**
         *
         * @param list initializer list of element pointers
         * @example
         * @code
         * group.addElements({ptr1, ptr2, ptr3});
         * @endcode
         */
        void addElements(std::initializer_list<UIElement*> list);

        std::vector<UIElement*> getElements();

    protected:
        /**
         * On screen elements
         */
        std::vector<UIElement*> ui_element_ptrs;

    };
}


#endif //LIBUI_UI_ELEMENT_H
