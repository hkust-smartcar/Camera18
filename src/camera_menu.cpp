//
// Created by Daniel on 15/3/2018.
//

#include "camera_menu.h"

void CameraMenu::init() {
    //
    // Prepare UI
    //

    if (ui::Context::getContext() == nullptr) {
        context_adapter_ptr = new adapters::ContextAdapter;
        ui::Context::attachAdapter(context_adapter_ptr);

        screen_adapter_ptr = adapters::ContextAdapter::screen_adapter_ptr = new adapters::ScreenAdapter;
        battery_meter_adapter_ptr = adapters::ContextAdapter::battery_meter_adapter_ptr = new adapters::BatteryMeterAdapter;
        joystick_adapter_ptr = adapters::ContextAdapter::joystick_adapter_ptr = new adapters::JoystickAdapter;

        screen_adapter_ptr->lcd_ptr = lcd;
        battery_meter_adapter_ptr->battery_meter_ptr = battery_meter;
        joystick_adapter_ptr->joystick_ptr = joystick;

        //Do not re-init joystick outside
//        adapters::ContextAdapter::joystick_config_base.id = 0;
//        adapters::ContextAdapter::joystick_config_base.is_active_low = true;
//
//        ui::Context::prepareListenerEvents();
//        ui::Context::remapJoystickDirections(ui::JoystickState::LEFT, ui::JoystickState::UP, ui::JoystickState::RIGHT, ui::JoystickState::DOWN);
    }

    ui::Context::addFontToRepo("Humanist", &(Application::humanist));
    ui::Context::addFontToRepo("Blocky", &(Application::blocky));
}

void CameraMenu::start() {
    //
    // Initialize other components
    //

    if (FlashStorage::flash_ptr == nullptr) {
        FlashStorage::flash_ptr = flash;
    }

    volatile bool is_exit = false;

    ui::TextBlock tb;
    tb.setColor(0xFFFF);
    tb.setFont(&humanist);

    //Constant Vs Dynamic
    volatile uint8_t choice;

    while (!is_exit) {
        screen_adapter_ptr->clear();

        FlashStorage::load();

        is_using_constant_mode = FlashStorage::data.is_using_constant_mode;

        choice = 0;

        {
            tb.setText(std::string("Run (S) - ") + (is_using_constant_mode ? "C" : "D"));
            tb.setRegion(0, 0, 128, 14);
            tb.render();

            tb.setText("Constant speed (U)");
            tb.setRegion(0, 15, 128, 14);
            tb.render();

            tb.setText("Dynamic speed (L)");
            tb.setRegion(0, 30, 128, 14);
            tb.render();

            tb.setText("Reset flash (R)");
            tb.setRegion(0, 45, 128, 14);
            tb.render();

            while (choice == 0) {
                const auto state = joystick_adapter_ptr->joystick_ptr->GetState();

                switch (state) {
                    case libsc::Joystick::State::kUp:
                        choice = 1;
                        break;
                    case libsc::Joystick::State::kDown:
                        choice = 2;
                        break;
                    case libsc::Joystick::State::kRight:
                        choice = 3;
                        break;
                    case libsc::Joystick::State::kSelect:
                        choice = 4;
                        break;
                    default:
                        {};
                }
            }
        };



        // =====
        if (choice == 1) {
            changeValue<uint16_t>("Constant speed", &FlashStorage::data.constant_speed, 10, "%d");
            is_using_constant_mode = true;
            FlashStorage::data.is_using_constant_mode = true;

            FlashStorage::save();

            is_exit = true;

        } else if (choice == 2) {
            changeValue<uint16_t>("Max speed", &FlashStorage::data.max_speed, 10, "%d");
            changeValue<uint16_t>("Min speed", &FlashStorage::data.min_speed, 10, "%d");
            changeValue<float>("Slope param", &FlashStorage::data.slope_param, .1, "%.2f");
            changeValue<float>("X shift", &FlashStorage::data.x_shift, .1, "%.2f");
            FlashStorage::data.is_using_constant_mode = false;

            FlashStorage::save();

            showPlot();

            while (joystick_adapter_ptr->joystick_ptr->GetState() == libsc::Joystick::State::kIdle) {}

            is_exit = true;

        } else if (choice == 3) {
            screen_adapter_ptr->clear();

            tb.setText("Select to reset flash!");
            tb.setTextWrap(ui::text::WRAP);
            tb.setRegion(0, 0, 128, 50);
            tb.render();

            while (joystick_adapter_ptr->joystick_ptr->GetState() == libsc::Joystick::State::kIdle) {}

            FlashStorage::data = FlashStorage::Data{};
            FlashStorage::save();
        } else if (choice == 4) {
            is_using_constant_mode = FlashStorage::data.is_using_constant_mode;
            is_exit = true;
        }
    }
}
