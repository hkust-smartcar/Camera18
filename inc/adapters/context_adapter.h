//
// Created by Daniel on 29/5/2018.
//

#ifndef LIBUI_LIBSCCC_CONTEXT_ADAPTER_H
#define LIBUI_LIBSCCC_CONTEXT_ADAPTER_H


#include <ui/adapters/context_adapters/context_adapter_interface.h>

#include <ui/context.h>
#include <ui/color_schemes/color_scheme.h>
#include <ui/ui_base.h>
#include <ui/e.h>
#include <ui/font.h>
#include <libsc/lcd.h>
#include <libsc/joystick.h>
#include <libsc/battery_meter.h>
#include <libsc/system.h>
#include <map>
#include <functional>
#include "screen_adapter.h"
#include "battery_meter_adapter.h"
#include "joystick_adapter.h"

namespace adapters {
    class ContextAdapter: public virtual ui::adapters::ContextAdapterInterface {
    public:
        ui::adapters::ScreenAdapterInterface* getScreen() override;
        ui::adapters::JoystickAdapterInterface* getJoystick() override;
        ui::adapters::BatteryMeterAdapterInterface* getBatteryMeter() override;
        void setGUIRotation(RotationalChange rotational_change) override;

        static adapters::ScreenAdapter* screen_adapter_ptr;
        static adapters::BatteryMeterAdapter* battery_meter_adapter_ptr;
        static adapters::JoystickAdapter* joystick_adapter_ptr;

        static libsc::Joystick::Config joystick_config_base;

        static libsc::Joystick::State JOYSTICK_UP;
        static libsc::Joystick::State JOYSTICK_DOWN;
        static libsc::Joystick::State JOYSTICK_LEFT;
        static libsc::Joystick::State JOYSTICK_RIGHT;
        static libsc::Joystick::State JOYSTICK_SELECT;
        static libsc::Joystick::State JOYSTICK_IDLE;

        void remapJoystickDirections(ui::JoystickState up,  ui::JoystickState down,  ui::JoystickState left,  ui::JoystickState right, ui::JoystickState select, ui::JoystickState idle) override;
        static void remapJoystickDirections(libsc::Joystick::State up,  libsc::Joystick::State down,  libsc::Joystick::State left,  libsc::Joystick::State right);

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
        void prepareListenerEvents() override;

        uint32_t getSystemTime() override;

        static libsc::Joystick::Listener joystick_listeners_dispatcher;
    };
}


#endif //LIBUI_LIBSCCC_CONTEXT_ADAPTER_H
