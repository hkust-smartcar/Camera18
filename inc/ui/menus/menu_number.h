//
// Created by Daniel on 17/2/2018.
//

#ifndef LIBUI_MENU_NUMBER_H
#define LIBUI_MENU_NUMBER_H


#include <ui/menus/menu_action.h>
#include <ui/icons.h>
#include <sstream>

namespace ui {
    template <class T>
    class MenuNumber: public virtual MenuAction {
    public:
        MenuNumber();

        void setValue(T value);

        T getValue();

        void setStep(T step);

        int run() override;

        void setName(std::string name) override;

        virtual void onChange();

    protected:
        T value;
        T step;

    private:
        static const uint32_t DIALOG_MARGIN = 12;
        const uint32_t DIALOG_WIDTH = Context::getScreen()->getWidth() - DIALOG_MARGIN * 2;
        static const uint32_t DIALOG_HEIGHT = 60;
        static const uint32_t DIALOG_TEXT_OFFSET = 5;
        static const uint32_t CARET_SIZE = 5;

        const graphics::Rectangle DIALOG_REGION {
                DIALOG_MARGIN,
                (Context::getScreen()->getHeight() - DIALOG_HEIGHT) / 2,
                DIALOG_WIDTH,
                DIALOG_HEIGHT
        };

        graphics::Rectangle text_region;

        TextBlock textBlockLabel;
        TextBlock textBlockValue;

        void printValue();
    };

    template<class T>
    MenuNumber<T>::MenuNumber() {
        //Style setup
        textBlockLabel.setFont(Context::font_repo["Humanist"]);
        textBlockLabel.setTextWrap(text::ELLIPSIS);
        textBlockLabel.setColor(Context::color_scheme.BODY);
        textBlockLabel.setRegion(DIALOG_REGION.x + DIALOG_TEXT_OFFSET, DIALOG_REGION.y + DIALOG_TEXT_OFFSET, DIALOG_WIDTH - DIALOG_TEXT_OFFSET * 2, 10);
        textBlockLabel.setText(this->name);

        text_region = graphics::Rectangle{DIALOG_REGION.x + DIALOG_TEXT_OFFSET * 2 + CARET_SIZE, DIALOG_REGION.y + 25, DIALOG_REGION.w - (DIALOG_TEXT_OFFSET * 2 + CARET_SIZE) * 2, 12};

        textBlockValue.setFont(Context::font_repo["Humanist"]);
        textBlockValue.setTextWrap(text::ELLIPSIS);
        textBlockValue.setColor(Context::color_scheme.BODY);
        textBlockValue.setRegion(text_region);
        textBlockValue.setText(this->name);
    }

    template<class T>
    int MenuNumber<T>::run() {
    	onEnter();
    	
    	adapters::ScreenAdapterInterface* screen_ptr = Context::getScreen();
    	
        //Draw dialog
        screen_ptr->setRegion(DIALOG_REGION);
        screen_ptr->fill(Context::color_scheme.BACKGROUND_LIGHT);

        //Draw border
        screen_ptr->setRegion(DIALOG_REGION.x, DIALOG_REGION.y, DIALOG_WIDTH, 1);
        screen_ptr->fill(Context::color_scheme.GRAY);
        screen_ptr->setRegion(DIALOG_REGION.x, DIALOG_REGION.y + DIALOG_HEIGHT - 1, DIALOG_WIDTH, 1);
        screen_ptr->fill(Context::color_scheme.GRAY);
        screen_ptr->setRegion(DIALOG_REGION.x, DIALOG_REGION.y, 1, DIALOG_HEIGHT);
        screen_ptr->fill(Context::color_scheme.GRAY);
        screen_ptr->setRegion(DIALOG_REGION.x + DIALOG_REGION.w - 1, DIALOG_REGION.y, 1, DIALOG_HEIGHT);
        screen_ptr->fill(Context::color_scheme.GRAY);

        //Draw name
        textBlockLabel.render();

        //Draw carets
        Icons::drawCaret(DIALOG_REGION.x + DIALOG_TEXT_OFFSET, DIALOG_REGION.y + 25 + 2, Context::color_scheme.GRAY, Direction::LEFT, CARET_SIZE);
        Icons::drawCaret(DIALOG_REGION.x + DIALOG_WIDTH - DIALOG_TEXT_OFFSET - CARET_SIZE, DIALOG_REGION.y + 25 + 2, Context::color_scheme.GRAY, Direction::RIGHT, CARET_SIZE);

        bool is_exit = false;

        std::function<void(E&)> joystick_handler = [&](E& e){
            if (e.JOYSTICK_STATE == JoystickState::LEFT) {
                //Decrease
                value -= step;
                printValue();
                onChange();
            } else if (e.JOYSTICK_STATE == JoystickState::RIGHT) {
                //Increase
                value += step;
                printValue();
                onChange();
            } else if (e.JOYSTICK_STATE == JoystickState::SELECT) {
                is_exit = true;
            }
            e.consume();
        };

        Context::addEventListener(Event::JOYSTICK_DOWN, &joystick_handler);

        printValue();

        while (!is_exit) {}

        Context::removeEventListener(Event::JOYSTICK_DOWN, &joystick_handler);

        onExit();

        return SUCCESS;
    }

    template<class T>
    void MenuNumber<T>::setStep(T step) {
        this->step = step;
    }

    template<class T>
    void MenuNumber<T>::setValue(T value) {
        this->value = value;
    }

    template<class T>
    T MenuNumber<T>::getValue() {
        return value;
    }

    template<class T>
    void MenuNumber<T>::setName(std::string name) {
        MenuAction::setName(name);
        textBlockLabel.setText(this->name);
    }

    template<class T>
    void MenuNumber<T>::printValue() {
        adapters::ScreenAdapterInterface* screen_ptr = Context::getScreen();
        
        //Re-render
        screen_ptr->setRegion(text_region);
        screen_ptr->fill(Context::color_scheme.BACKGROUND_LIGHT);

        std::ostringstream os;
        os.precision(3);
        os << std::fixed;
        os << value;

        textBlockValue.setText(os.str());
        textBlockValue.render();
    }

    template<class T>
    void MenuNumber<T>::onChange() {}


}


#endif //LIBUI_MENU_NUMBER_H
