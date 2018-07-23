//
// Created by Daniel on 7/3/2018.
//

#ifndef LIBUI_E_H
#define LIBUI_E_H

#include <cstdint>

namespace ui {

    /**
     * Various events for usages in listener relays in Context
     *
     * @see ui::Context::addEventListener
     */
    enum struct EventType {
        JOYSTICK_DOWN,
        JOYSTICK_UP,
        BTN_DOWN,
        BTN_UP
    };

    enum struct JoystickState {
        UP,
        DOWN,
        LEFT,
        RIGHT,
        SELECT,
        IDLE
    };

    /**
     * Event Object for listener relay in Context to describe the event in detail.
     *
     * This is bundles and passed in all listeners in Context to describe the event. Data members are only accurate for
     * the chosen event, since there must always be some default values for non-pointer members.
     *
     * @see ui::Context::addEventListener
     */
    class E {
    public:
        const uint8_t JOYSTICK_ID;
        const JoystickState JOYSTICK_STATE;
        const bool IS_JOYSTICK_DOWN;
        const bool IS_BTN_DOWN;

        struct Config {
            uint8_t JOYSTICK_ID = 0;
            JoystickState JOYSTICK_STATE = JoystickState::IDLE;
            bool IS_JOYSTICK_DOWN = false;
            bool IS_BTN_DOWN = false;
        };

        explicit E(Config config):
                JOYSTICK_ID(config.JOYSTICK_ID),
                JOYSTICK_STATE(config.JOYSTICK_STATE),
                IS_JOYSTICK_DOWN(config.IS_JOYSTICK_DOWN),
                IS_BTN_DOWN(config.IS_BTN_DOWN) {}

        bool isConsumed() {
            return is_consumed;
        }

        void consume() {
            is_consumed = true;
        }

    private:
        bool is_consumed = false;
    };
}


#endif //LIBUI_E_H
