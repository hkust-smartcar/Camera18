//
// Created by Daniel on 20/2/2018.
//

#include "ui/context.h"

namespace ui {
    const libsc::Lcd::Rect Context::full_screen(0, 0, 128, 160);
    libsc::Lcd* Context::lcd_ptr = nullptr;
    libsc::Joystick* Context::joystick_ptr = nullptr;
    libsc::BatteryMeter* Context::batteryMeter = nullptr;
    ColorScheme Context::color_scheme;
    libsc::Joystick::State Context::JOYSTICK_UP = libsc::Joystick::State::kUp;
    libsc::Joystick::State Context::JOYSTICK_DOWN = libsc::Joystick::State::kDown;
    libsc::Joystick::State Context::JOYSTICK_LEFT = libsc::Joystick::State::kLeft;
    libsc::Joystick::State Context::JOYSTICK_RIGHT = libsc::Joystick::State::kRight;
    libsc::Joystick::State Context::JOYSTICK_SELECT = libsc::Joystick::State::kSelect;

    libsc::Joystick::Config Context::joystick_config_base;

    std::map<std::pair<Event, std::function<void(E)>*>, std::function<void(E)>*> Context::listener_map;

    void Context::setColorScheme(ColorScheme colorScheme) {
        Context::color_scheme = colorScheme;
    }

    void Context::remap_joystick_directions(libsc::Joystick::State up, libsc::Joystick::State down,
                                                libsc::Joystick::State left, libsc::Joystick::State right) {
        JOYSTICK_UP = up;
        JOYSTICK_DOWN = down;
        JOYSTICK_LEFT = left;
        JOYSTICK_RIGHT = right;
    }

    void Context::setJoystickRotation(Context::RotationalChange rotationalChange) {
        if (rotationalChange == QUARTER_CCW) {
            remap_joystick_directions(
                    libsc::Joystick::State::kLeft,
                    libsc::Joystick::State::kRight,
                    libsc::Joystick::State::kDown,
                    libsc::Joystick::State::kUp
            );
        } else if (rotationalChange == QUARTER_CW) {
            remap_joystick_directions(
                    libsc::Joystick::State::kRight,
                    libsc::Joystick::State::kLeft,
                    libsc::Joystick::State::kUp,
                    libsc::Joystick::State::kDown
            );
        } else if (rotationalChange == HALF_TURN) {
            remap_joystick_directions(
                    libsc::Joystick::State::kDown,
                    libsc::Joystick::State::kUp,
                    libsc::Joystick::State::kRight,
                    libsc::Joystick::State::kLeft
            );
        }
    }

    void Context::addEventListener(Event event, std::function<void(E)>* cb_ptr) {
        listener_map[{event, cb_ptr}] = cb_ptr;
    }

    void Context::removeEventListener(Event event, std::function<void(E)>* cb_ptr) {
        std::pair<Event, std::function<void(E)>*> key = {event, cb_ptr};
        //Check if the function is already in map
        if ( listener_map.find(key) != listener_map.end() ) {
            listener_map.erase(key);
        }
    }

    void Context::triggerListeners(Event event, E event_object) {
        for (auto& pair: listener_map) {
            if (pair.first.first == event)
                (*pair.second)(event_object);
        }
    }

    void Context::prepareListenerEvents() {
        //Prepare joystick listener
        delete joystick_ptr;

        libsc::Joystick::Config joystick_config = joystick_config_base;
        joystick_config.listener_triggers[0] = libsc::Joystick::Config::Trigger::kBoth;
        joystick_config.listener_triggers[1] = libsc::Joystick::Config::Trigger::kBoth;
        joystick_config.listener_triggers[2] = libsc::Joystick::Config::Trigger::kBoth;
        joystick_config.listener_triggers[3] = libsc::Joystick::Config::Trigger::kBoth;
        joystick_config.listener_triggers[4] = libsc::Joystick::Config::Trigger::kBoth;
        joystick_config.dispatcher = joystick_listeners_dispatcher;

        joystick_ptr = new libsc::Joystick(joystick_config);

        //TODO btn listener setup
    }

    std::map<std::string, Font*> Context::font_repo;

    void Context::addFontToRepo(const std::string &name, Font* font_ptr) {
        font_repo[name] = font_ptr;
    }

    void Context::removeFontFromRepo(const std::string &name) {
        if ( font_repo.find(name) != font_repo.end() ) {
            font_repo.erase(name);
        }
    }

    void Context::clearFontRepo() {
        font_repo.clear();
    }

    libsc::Joystick::Listener Context::joystick_listeners_dispatcher = [](const uint8_t id, const libsc::Joystick::State which)->void{
        E::Config e;
        e.JOYSTICK_ID = id,
        e.JOYSTICK_STATE = which;

        libsc::System::DelayMs(300);

        //Check whether action is up or down
        if (joystick_ptr->GetState() == libsc::Joystick::State::kIdle) {
            //Up
            e.IS_JOYSTICK_DOWN = false;
            Context::triggerListeners(Event::JOYSTICK_UP, E(e));
        } else {
            //Down
            e.IS_JOYSTICK_DOWN = true;
            Context::triggerListeners(Event::JOYSTICK_DOWN, E(e));
        }
    };

}
