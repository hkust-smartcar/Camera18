//
// Created by Daniel on 30/5/2018.
//

#ifndef LIBUI_LIBSCCC_JOYSTICK_ADAPTER_H
#define LIBUI_LIBSCCC_JOYSTICK_ADAPTER_H

#include <libsc/joystick.h>
#include <ui/adapters/module_adapters/joystick_interface.h>

namespace adapters {
    class JoystickAdapter: public virtual ui::adapters::JoystickAdapterInterface {
    public:
        libsc::Joystick* joystick_ptr = nullptr;
    };
}


#endif //LIBUI_LIBSCCC_JOYSTICK_ADAPTER_H
