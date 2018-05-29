//
// Created by Daniel on 17/2/2018.
//

#ifndef INNO14_D_2017_INNO_MENU_GROUP_H
#define INNO14_D_2017_INNO_MENU_GROUP_H


#include <utility>
#include <sstream>
#include <libsc/system.h>
#include <string>
#include <libsc/lcd.h>
#include <ui/context.h>
#include <ui/color_util.h>
#include <ui/text_block.h>
#include <ui/fonts/humanist.h>
#include <ui/icons.h>
#include <ui/fonts/blocky.h>
#include <ui/menus/menu_action.h>
#include <ui/toolbar.h>
#include <cassert>

namespace ui {
    class MenuGroup: public virtual MenuAction {
    public:
        explicit MenuGroup(std::string menu_name = "Untitled");

        int run() override;

        void setHasBackArrow(bool has_back_arrow);

        void addMenuAction(MenuAction* menu_action);

        void render() override;

        void selectPrevAction();

        void selectNextAction();

        void runAction();

        bool isIndexInPage(uint8_t i);

        void selectNewActionByIndex(int16_t i);

        void exitMenu();

        void setName(std::string name) override;

        void setLoopBack(bool is_loop_back);

    private:
        static const uint8_t TITLE_BAR_HEIGHT = 18;
        static const uint16_t SCROLLBAR_WIDTH = 2;
        static const uint16_t CARET_OFFSET = TEXT_OFFSET + 1;

        Toolbar toolbar;
        TextBlock textBlockBatteryVoltage;

        std::vector<MenuAction*> menu_actions;
        uint8_t selected_index = 0;

        uint8_t getItemsPerPage();
        uint8_t getTotalPages();
        uint8_t getCurrentPageIndex();
        uint8_t getPageIndexByItemIndex(uint8_t item_index);

        bool is_loop_back = true;

        MenuAction* run_action = nullptr;

        void drawBatteryMeter();

        void drawBaseUI();

        void drawPage();

        void drawScrollBar();

        bool is_exit = false;
    };
}


#endif //INNO14_D_2017_INNO_MENU_GROUP_H
