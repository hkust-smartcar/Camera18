//
// Created by Daniel on 20/2/2018.
//

#ifndef INNO14_D_2017_INNO_PERIPHERALS_H
#define INNO14_D_2017_INNO_PERIPHERALS_H


#include <libsc/lcd.h>
#include <libsc/joystick.h>
#include <libsc/battery_meter.h>
#include <libsc/system.h>
#include <ui/color_schemes/color_scheme.h>
#include <ui/ui_base.h>
#include <ui/e.h>
#include <map>
#include <functional>
#include "font.h"

namespace ui {
    class Context {
    public:
        static libsc::Lcd* lcd_ptr;
        static libsc::Joystick* joystick_ptr;
        static libsc::BatteryMeter* batteryMeter;
        static ColorScheme color_scheme;

        static void setColorScheme(ColorScheme colorScheme);

        static const libsc::Lcd::Rect full_screen;

        static libsc::Joystick::State JOYSTICK_UP;
        static libsc::Joystick::State JOYSTICK_DOWN;
        static libsc::Joystick::State JOYSTICK_LEFT;
        static libsc::Joystick::State JOYSTICK_RIGHT;
        static libsc::Joystick::State JOYSTICK_SELECT;

        static libsc::Joystick::Config joystick_config_base;

        static void remap_joystick_directions(
                libsc::Joystick::State up,
                libsc::Joystick::State down,
                libsc::Joystick::State left,
                libsc::Joystick::State right
        );

        /**
         * Relays all UI listeners to libsccc, so that removing listeners is possible
         */
        static std::map<std::pair<Event, std::function<void(E event_obj)>*>, std::function<void(E event_obj)>*>
                listener_map;

        enum RotationalChange {
            NO_TURN,
            QUARTER_CCW,
            QUARTER_CW,
            HALF_TURN
        };

        /**
         * Sets rotation of the virtual joystick states. References to the states later on must refer to the states in
         * Context, e.g. Context::JOYSTICK_UP.
         *
         * @param rotationalChange An item of the enum RotationalChange.
         *
         * @example This example will change and retrieve the virtual state of the joystick.
         * @code
         * Context::setJoystickRotation(QUARTER_CCW);
         * bool isRight = Context::joystick_ptr->GetState() == Context::JOYSTICK_RIGHT;
         * @endcode
         *
         * @see ui::Context::RotationalChange
         */
        static void setJoystickRotation(RotationalChange rotationalChange);

        /**
         * Prepares listener relays.
         *
         * Caution! This will delete and re-instantiate various peripheral pointers.
         */
        static void prepareListenerEvents();

        /**
         * Adds an event listener
         * @param event
         * @param cb_ptr
         */
        static void addEventListener(Event event, std::function<void(E event_obj)>* cb_ptr);

        static void removeEventListener(Event event, std::function<void(E event_obj)>* cb_ptr);

        static void triggerListeners(Event event, E event_object);

        static std::map<std::string, Font*> font_repo;

        static void addFontToRepo(const std::string &name, Font* font_ptr);

        static void removeFontFromRepo(const std::string &name);

        static void clearFontRepo();

        static libsc::Joystick::Listener joystick_listeners_dispatcher;
    };
}



#endif //INNO14_D_2017_INNO_PERIPHERALS_H
