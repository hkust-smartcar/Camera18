//
// Created by Daniel on 17/2/2018.
//

#ifndef INNO14_D_2017_INNO_MENU_ACTION_H
#define INNO14_D_2017_INNO_MENU_ACTION_H

#include <functional>
#include <vector>
#include <libbase/misc_utils.h>
#include <ui/context.h>
#include <ui/ui_base.h>
#include <ui/text_block.h>
#include <ui/fonts/humanist.h>
#include <ui/events/life_cycle_events.h>
#include <cassert>

namespace ui {
    /**
     * Base class MenuAction for menu items.
     * Used for running routines in a menu. Extend this class to be used in a menu.
     *
     * @example
     * @code
     * //Anonymous class initialization
     * class: public ui::MenuAction {
     *     void onEnter() override {
     *         myFunction();
     *     }
     * } my_menu_action;
     *
     * //Reusable class initialization
     * class MyMenuAction: public ui::MenuAction {
     *     void onEnter() override {
     *         myFunction();
     *     }
     * };
     * MyMenuAction* my_action = new MyMenuAction;
     * @endcode
     *
     */
    class MenuAction: public virtual UIBase, public events::LifeCycleEvents {
    public:
        MenuAction();

        virtual int run();

        virtual void select();

        virtual void deselect();

        void render() override;

        virtual void setName(std::string name);

    protected:
        static const uint8_t ITEM_HEIGHT = 18;
        static const uint16_t PADDING = 18;
        static const uint16_t TEXT_OFFSET = 3;

        std::string name = "Untitled";
        bool is_selected = false;


        TextBlock textBlockName;
    };
}


#endif //INNO14_D_2017_INNO_MENU_ACTION_H
