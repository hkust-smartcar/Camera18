//
// Created by Daniel on 7/3/2018.
//

#ifndef INNO14_D_2017_INNO_E_H
#define INNO14_D_2017_INNO_E_H

#include <libsc/joystick.h>

namespace ui {

    /**
     * Various events for usages in listener relays in Context
     *
     * @see ui::Context::addEventListener
     */
    enum struct Event {
        JOYSTICK_DOWN,
        JOYSTICK_UP,
        BTN_DOWN,
        BTN_UP
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
        const libsc::Joystick::State JOYSTICK_STATE;
        const bool IS_JOYSTICK_DOWN;
        const bool IS_BTN_DOWN;

        struct Config {
            uint8_t JOYSTICK_ID = 0;
            libsc::Joystick::State JOYSTICK_STATE = libsc::Joystick::State::kIdle;
            bool IS_JOYSTICK_DOWN = false;
            bool IS_BTN_DOWN = false;
        };

        explicit E(Config config):
                JOYSTICK_ID(config.JOYSTICK_ID),
                JOYSTICK_STATE(config.JOYSTICK_STATE),
                IS_JOYSTICK_DOWN(config.IS_JOYSTICK_DOWN),
                IS_BTN_DOWN(config.IS_BTN_DOWN) {}
    };
}


#endif //INNO14_D_2017_INNO_E_H
