//
// Created by Daniel on 29/5/2018.
//

#ifndef LIBUI_CONTEXT_ADAPTER_INTERFACE_H
#define LIBUI_CONTEXT_ADAPTER_INTERFACE_H

#include <ui/e.h>
#include <ui/adapters/module_adapters/screen_interface.h>
#include <ui/adapters/module_adapters/joystick_interface.h>
#include <ui/adapters/module_adapters/battery_meter_interface.h>

namespace ui {
    namespace adapters {
        class ContextAdapterInterface {
        public:
            enum RotationalChange {
                NO_TURN,
                QUARTER_CCW,
                QUARTER_CW,
                HALF_TURN
            };

            virtual ScreenAdapterInterface* getScreen() = 0;
            virtual JoystickAdapterInterface* getJoystick() = 0;
            virtual BatteryMeterAdapterInterface* getBatteryMeter() = 0;
            virtual void setGUIRotation(RotationalChange) = 0;
            virtual void remapJoystickDirections(ui::JoystickState up,  ui::JoystickState down,  ui::JoystickState left,  ui::JoystickState right, ui::JoystickState select, ui::JoystickState idle) = 0;
            virtual void prepareListenerEvents() {};
            virtual uint32_t getSystemTime() = 0;
        };
    }
}


#endif //LIBUI_CONTEXT_ADAPTER_INTERFACE_H
