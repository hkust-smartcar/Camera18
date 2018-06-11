//
// Created by Daniel on 17/2/2018.
//

#include "ui/menus/menu_group.h"

namespace ui {

    MenuGroup::MenuGroup(std::string menu_name) {
        this->setName(std::move(menu_name));
        this->setRegion(0, 0, Context::getScreen()->getWidth(), Context::getScreen()->getHeight());

        toolbar.setRegion(ui_region.x, ui_region.y, ui_region.w, TITLE_BAR_HEIGHT);
        toolbar.reserveHSpace(16);

        assert(Context::font_repo["Blocky"] != nullptr);
        textBlockBatteryVoltage.setFont(Context::font_repo["Blocky"]);
        textBlockBatteryVoltage.setTextWrap(text::NO_WRAP);
        textBlockBatteryVoltage.setRegion(Context::getScreen()->getWidth() - 17u, 6, 20, 5);
        textBlockBatteryVoltage.setText("Err");
    }

    int MenuGroup::run() {
        /**
         * Remember to set font pointer in Context.
         *
         * @see ui::Context::font_repo
         */

        drawBaseUI();

        //Draw items
        drawPage();

        //Rendering loop
        uint32_t time = Context::getSystemTime();

        std::function<void(E&)> joystick_handler = [&](E& e){
            if (e.JOYSTICK_STATE == JoystickState::UP) {
                //Select prev item
                selectPrevAction();
                e.consume();
            } else if (e.JOYSTICK_STATE == JoystickState::DOWN) {
                //Select next item
                selectNextAction();
                e.consume();
            } else if (e.JOYSTICK_STATE == JoystickState::SELECT) {
                runAction();
                e.consume();
            }
        };

        Context::addEventListener(Event::JOYSTICK_DOWN, &joystick_handler);

        while (!is_exit) {
            if (time != Context::getSystemTime()) {
                time = Context::getSystemTime();

                //When an action is queued for running
                if (run_action != nullptr) {
                    run_action->run();
                    run_action = nullptr;

                    if (is_exit)
                        break;

                    drawBaseUI();
                    drawPage();
                }

                if (time % 1000 == 1) {
                    drawBatteryMeter();
                }
            }
        }

        Context::removeEventListener(Event::JOYSTICK_DOWN, &joystick_handler);

        //Exit preparation

        return 0;
    }

    void MenuGroup::setHasBackArrow(bool has_back_arrow) {
        toolbar.setHasBackArrow(has_back_arrow);
    }

    void MenuGroup::drawBatteryMeter() {
        float voltage = Context::getBatteryMeter()->getVoltage();
        uint16_t &battery_color = voltage > 7.4 ?
                                  Context::color_scheme.SUCCESS :
                                  Context::color_scheme.DANGER;

        adapters::ScreenAdapterInterface* screen_ptr = Context::getScreen();

        Icons::drawBatteryGauge(screen_ptr->getWidth() - 19u, 4, battery_color, 16, 9);
        screen_ptr->setRegion(screen_ptr->getWidth() - 18u, 5, 13, 7);
        screen_ptr->fill(Context::color_scheme.BACKGROUND_LIGHT);
        std::ostringstream os;
        os << (roundf(voltage * 10) / 10);
        if (os.str().back() == '.')
            os << '0';
        textBlockBatteryVoltage.setText(os.str());
        textBlockBatteryVoltage.setColor(battery_color);
        textBlockBatteryVoltage.render();
    }

    void MenuGroup::addMenuAction(MenuAction* menu_action_ptr) {
        menu_actions.push_back(menu_action_ptr);
    }

    void MenuGroup::addMenuActions(std::initializer_list<MenuAction *> menu_action_ptrs) {
        for (auto* menu_action_ptr: menu_action_ptrs) {
            addMenuAction(menu_action_ptr);
        }
    }

    void MenuGroup::render() {
        adapters::ScreenAdapterInterface* screen_ptr = Context::getScreen();

        screen_ptr->setRegion(ui_region);
        screen_ptr->fill(is_selected ? Context::color_scheme.PRIMARY_LIGHTER : Context::color_scheme.BACKGROUND);
        textBlockName.setRegion(ui_region.x + PADDING, ui_region.y + TEXT_OFFSET, ui_region.w - PADDING * 2, ITEM_HEIGHT);
        textBlockName.setText(name);
        textBlockName.render();

        //Draw arrow
        Icons::drawCaret(
                screen_ptr->getWidth() - PADDING,
                CARET_OFFSET,
                is_selected ? Context::color_scheme.PRIMARY : Context::color_scheme.GRAY,
                graphics::RIGHT, 5
        );

    }

    void MenuGroup::drawBaseUI() {
        adapters::ScreenAdapterInterface* screen_ptr = Context::getScreen();
        
        toolbar.render();

        //Draw battery meter
        drawBatteryMeter();

        //Draw scrollbar
        screen_ptr->setRegion(
                screen_ptr->getWidth() - 2u,
                TITLE_BAR_HEIGHT + 1,
                SCROLLBAR_WIDTH,
                screen_ptr->getHeight() - TITLE_BAR_HEIGHT
        );
        screen_ptr->fill(Context::color_scheme.BACKGROUND_LIGHT);
    }

    void MenuGroup::drawPage() {
        adapters::ScreenAdapterInterface* screen_ptr = Context::getScreen();
        
        //Draw background
        screen_ptr->setRegion(0, TITLE_BAR_HEIGHT, screen_ptr->getWidth(), screen_ptr->getHeight() - TITLE_BAR_HEIGHT);
        screen_ptr->fill(Context::color_scheme.BACKGROUND);

        std::deque<MenuAction*> current_page_actions;

        uint8_t i = 0;
        auto menu_actions_it = menu_actions.begin() + (getCurrentPageIndex() * getItemsPerPage());
        //show one more item
        auto menu_actions_stop = std::min(menu_actions_it + getItemsPerPage(), menu_actions.end());

        while (menu_actions_it < menu_actions_stop) {
            MenuAction* menu_action = *menu_actions_it;

            menu_action->setRegion(
                    this->ui_region.x,
                    this->ui_region.y + TITLE_BAR_HEIGHT + ITEM_HEIGHT * i,
                    this->ui_region.w - SCROLLBAR_WIDTH,
                    ITEM_HEIGHT
            );

            current_page_actions.push_back(menu_action);

            if (i == selected_index % getItemsPerPage()) {
                menu_action->select();
            } else {
                menu_action->deselect();
            }

            menu_action->render();
            i++;
            menu_actions_it++;
        }

        drawScrollBar();
    }

    uint8_t MenuGroup::getItemsPerPage() {
        return uint8_t ((Context::getScreen()->getHeight() - TITLE_BAR_HEIGHT) / ITEM_HEIGHT);
    }

    uint8_t MenuGroup::getTotalPages() {
        return (uint8_t) (menu_actions.size() / getItemsPerPage());
    }

    uint8_t MenuGroup::getCurrentPageIndex() {
        return selected_index / getItemsPerPage();
    }

    uint8_t MenuGroup::getPageIndexByItemIndex(uint8_t item_index) {
        return item_index / getItemsPerPage();
    }

    void MenuGroup::selectNewActionByIndex(int16_t new_index) {
        if (is_loop_back) {
            new_index = new_index % (int16_t) menu_actions.size();
            if (new_index < 0)
            	new_index += (int16_t) menu_actions.size();
        } else
            new_index = (int16_t) std::min(std::max((int) new_index, 0), (int) (menu_actions.size() - 1));

        if (new_index == selected_index)
            return;

        MenuAction* current_action_ptr = menu_actions[selected_index];
        MenuAction* new_action_ptr = menu_actions[new_index];
        current_action_ptr->deselect();
        new_action_ptr->select();
        if (isIndexInPage((uint8_t) new_index)) {
            //New index is in the same page. Alter current and last item
            current_action_ptr->render();
            new_action_ptr->render();
            selected_index = (uint8_t) new_index;
        } else {
            //New index is on a new page. Need to re-render entire page
            selected_index = (uint8_t) new_index;
            drawPage();
        }
    }

    void MenuGroup::selectPrevAction() {
        selectNewActionByIndex(selected_index - (uint8_t) 1);
    }

    void MenuGroup::selectNextAction() {
        selectNewActionByIndex(selected_index + (uint8_t) 1);
    }

    void MenuGroup::runAction() {
        run_action = menu_actions[selected_index];
    }

    bool MenuGroup::isIndexInPage(uint8_t i) {
        uint8_t start_of_current_page_index = getCurrentPageIndex() * getItemsPerPage();
        return i >= start_of_current_page_index && i < start_of_current_page_index + getItemsPerPage();
    }

    void MenuGroup::drawScrollBar() {
        adapters::ScreenAdapterInterface* screen_ptr = Context::getScreen();

        if (getTotalPages() > 1) {
            //draw scroll bar
            uint8_t scroll_bar_height = (uint8_t) (ui_region.h - TITLE_BAR_HEIGHT) / getTotalPages();
            uint8_t scroll_bar_offset = (getCurrentPageIndex() / (getTotalPages() - (uint8_t) 1)) * scroll_bar_height;

            screen_ptr->setRegion(
                    ui_region.x + ui_region.w - SCROLLBAR_WIDTH,
                    TITLE_BAR_HEIGHT + scroll_bar_offset,
                    SCROLLBAR_WIDTH,
                    scroll_bar_height
            );

            screen_ptr->fill(Context::color_scheme.PRIMARY);

        }
    }

    /**
     * Sets state of the menu group to have the menu exit in the next loop
     */
    void MenuGroup::exitMenu() {
        is_exit = true;
    }

    void MenuGroup::setName(std::string name) {
        MenuAction::setName(name);
        toolbar.setName(name);
    }

    void MenuGroup::setLoopBack(bool is_loop_back) {
        this->is_loop_back = is_loop_back;
    }
}
