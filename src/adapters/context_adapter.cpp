//
// Created by Daniel on 29/5/2018.
//

#include "adapters/context_adapter.h"

namespace adapters {
    adapters::ScreenAdapter* ContextAdapter::screen_adapter_ptr = nullptr;
    adapters::JoystickAdapter* ContextAdapter::joystick_adapter_ptr = nullptr;
    adapters::BatteryMeterAdapter* ContextAdapter::battery_meter_adapter_ptr = nullptr;

    libsc::Joystick::Config ContextAdapter::joystick_config_base;

    libsc::Joystick::State ContextAdapter::JOYSTICK_UP;
    libsc::Joystick::State ContextAdapter::JOYSTICK_DOWN;
    libsc::Joystick::State ContextAdapter::JOYSTICK_LEFT;
    libsc::Joystick::State ContextAdapter::JOYSTICK_RIGHT;
    libsc::Joystick::State ContextAdapter::JOYSTICK_SELECT;
    libsc::Joystick::State ContextAdapter::JOYSTICK_IDLE;

    libsc::Joystick::State toJoystickState(ui::JoystickState state) {
        switch (state) {
            case ui::JoystickState::UP:
                return libsc::Joystick::State::kUp;
            case ui::JoystickState::DOWN:
                return libsc::Joystick::State::kDown;
            case ui::JoystickState::LEFT:
                return libsc::Joystick::State::kLeft;
            case ui::JoystickState::RIGHT:
                return libsc::Joystick::State::kRight;
            case ui::JoystickState::SELECT:
                return libsc::Joystick::State::kSelect;
            case ui::JoystickState::IDLE:
                return libsc::Joystick::State::kIdle;
        }
    }

    void ContextAdapter::remapJoystickDirections(ui::JoystickState up, ui::JoystickState down, ui::JoystickState left,
                                                 ui::JoystickState right, ui::JoystickState select,
                                                 ui::JoystickState idle) {
        JOYSTICK_UP = toJoystickState(up);
        JOYSTICK_DOWN = toJoystickState(down);
        JOYSTICK_LEFT = toJoystickState(left);
        JOYSTICK_RIGHT = toJoystickState(right);
        JOYSTICK_SELECT = toJoystickState(select);
        JOYSTICK_IDLE = toJoystickState(idle);
    }

    void ContextAdapter::remapJoystickDirections(libsc::Joystick::State up, libsc::Joystick::State down,
                                                 libsc::Joystick::State left, libsc::Joystick::State right) {
        JOYSTICK_UP = up;
        JOYSTICK_DOWN = down;
        JOYSTICK_LEFT = left;
        JOYSTICK_RIGHT = right;
    }

    void ContextAdapter::setJoystickRotation(ui::adapters::ContextAdapterInterface::RotationalChange rotationalChange) {
        if (rotationalChange == QUARTER_CCW) {
            remapJoystickDirections(
                    libsc::Joystick::State::kLeft,
                    libsc::Joystick::State::kRight,
                    libsc::Joystick::State::kDown,
                    libsc::Joystick::State::kUp
            );
        } else if (rotationalChange == QUARTER_CW) {
            remapJoystickDirections(
                    libsc::Joystick::State::kRight,
                    libsc::Joystick::State::kLeft,
                    libsc::Joystick::State::kUp,
                    libsc::Joystick::State::kDown
            );
        } else if (rotationalChange == HALF_TURN) {
            remapJoystickDirections(
                    libsc::Joystick::State::kDown,
                    libsc::Joystick::State::kUp,
                    libsc::Joystick::State::kRight,
                    libsc::Joystick::State::kLeft
            );
        }
    }

    void ContextAdapter::prepareListenerEvents() {
        //Prepare joystick listener
        delete joystick_adapter_ptr->joystick_ptr;

        libsc::Joystick::Config joystick_config = joystick_config_base;
        joystick_config.listener_triggers[0] = libsc::Joystick::Config::Trigger::kBoth;
        joystick_config.listener_triggers[1] = libsc::Joystick::Config::Trigger::kBoth;
        joystick_config.listener_triggers[2] = libsc::Joystick::Config::Trigger::kBoth;
        joystick_config.listener_triggers[3] = libsc::Joystick::Config::Trigger::kBoth;
        joystick_config.listener_triggers[4] = libsc::Joystick::Config::Trigger::kBoth;
        joystick_config.dispatcher = joystick_listeners_dispatcher;

        joystick_adapter_ptr->joystick_ptr = new libsc::Joystick(joystick_config);

        //TODO btn listener setup
    }

    libsc::Joystick::Listener ContextAdapter::joystick_listeners_dispatcher = [](const uint8_t id, const libsc::Joystick::State which) {
        ui::E::Config e;
        e.JOYSTICK_ID = id;
        if (which == JOYSTICK_UP) {
            e.JOYSTICK_STATE = ui::JoystickState::UP;
        } else if (which == JOYSTICK_DOWN) {
            e.JOYSTICK_STATE = ui::JoystickState::DOWN;
        } else if (which == JOYSTICK_LEFT) {
            e.JOYSTICK_STATE = ui::JoystickState::LEFT;
        } else if (which == JOYSTICK_RIGHT) {
            e.JOYSTICK_STATE = ui::JoystickState::RIGHT;
        } else {
            switch(which) {
                case libsc::Joystick::State::kSelect:
                    e.JOYSTICK_STATE = ui::JoystickState::SELECT;
                    break;
                default:
                    e.JOYSTICK_STATE = ui::JoystickState::IDLE;
            }
        }

        //Check whether action is up or down
        if (joystick_adapter_ptr->joystick_ptr->GetState() == libsc::Joystick::State::kIdle) {
            //Up
            e.IS_JOYSTICK_DOWN = false;
            ui::Context::triggerListeners(ui::Event::JOYSTICK_UP, ui::E(e));
        } else {
            //Down
            e.IS_JOYSTICK_DOWN = true;
            ui::Context::triggerListeners(ui::Event::JOYSTICK_DOWN, ui::E(e));
        }
    };

    ui::adapters::ScreenAdapterInterface* ContextAdapter::getScreen() {
        return screen_adapter_ptr;
    }

    ui::adapters::JoystickAdapterInterface* ContextAdapter::getJoystick() {
        return joystick_adapter_ptr;
    }

    ui::adapters::BatteryMeterAdapterInterface *ContextAdapter::getBatteryMeter() {
        return battery_meter_adapter_ptr;
    }

    void ContextAdapter::setGUIRotation(ui::adapters::ContextAdapterInterface::RotationalChange rotational_change) {
        setJoystickRotation(rotational_change);
    }

    uint32_t ContextAdapter::getSystemTime() {
        return libsc::System::Time();
    }
}
