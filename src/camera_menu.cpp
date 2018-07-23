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

        //Do not re-init joystick outside
        adapters::ContextAdapter::joystick_config_base.id = 0;
        adapters::ContextAdapter::joystick_config_base.is_active_low = true;

        ui::Context::prepareListenerEvents();
        ui::Context::remapJoystickDirections(ui::JoystickState::LEFT, ui::JoystickState::UP, ui::JoystickState::RIGHT, ui::JoystickState::DOWN);
    }

    ui::Context::addFontToRepo("Humanist", &(Application::humanist));
    ui::Context::addFontToRepo("Blocky", &(Application::blocky));
}

void CameraMenu::start() {
    //
    // Initialize other components
    //

    if (FlashStorage::flash_ptr == nullptr) {
        FlashStorage::flash_ptr = new libbase::k60::Flash([]() {
            libbase::k60::Flash::Config config{};
            return config;
        }());
    }


    ui::TextBlock tb;
    tb.setColor(0xFFFF);
    tb.setFont(&humanist);

    //Constant Vs Dynamic
    volatile uint8_t choice;

    start:
    screen_adapter_ptr->clear();

    FlashStorage::load();

    choice = 0;

    {
        std::function<void(ui::E&)> constant_dynamic_handler = [&](ui::E& e){
            if (e.JOYSTICK_STATE == ui::JoystickState::UP) {
                //Constant
                choice = 1;
                e.consume();
            } else if (e.JOYSTICK_STATE == ui::JoystickState::DOWN) {
                //Dynamic
                choice = 2;
                e.consume();
            } else if (e.JOYSTICK_STATE == ui::JoystickState::RIGHT) {
                //Reset
                choice = 3;
                e.consume();
            } else if (e.JOYSTICK_STATE == ui::JoystickState::SELECT) {
                //Run
                choice = 4;
                e.consume();
            }
        };

        tb.setText("Run (S)");
        tb.setRegion(0, 0, 128, 14);
        tb.render();

        tb.setText("Constant speed (U)");
        tb.setRegion(0, 15, 128, 14);
        tb.render();

        tb.setText("Dynamic speed (D)");
        tb.setRegion(0, 30, 128, 14);
        tb.render();

        tb.setText("Reset flash (R)");
        tb.setRegion(0, 45, 128, 14);
        tb.render();

        ui::Context::addEventListener(ui::EventType::JOYSTICK_DOWN, &constant_dynamic_handler);

        while (choice == 0) {}

        ui::Context::removeEventListener(ui::EventType::JOYSTICK_DOWN, &constant_dynamic_handler);
    };

    if (choice == 1) goto constant;
    else if (choice == 2) goto dynamic;
    else if (choice == 3) goto reset;
    else if (choice == 4) goto exit;


    // =====

    constant:

    changeValue<uint16_t>("Constant speed", &FlashStorage::data.constant_speed, 10, "%d");
    is_using_constant_mode = true;

    FlashStorage::save();

    goto exit;
    dynamic:

    {
        changeValue<uint16_t>("Max speed", &FlashStorage::data.max_speed, 10, "%d");
        changeValue<uint16_t>("Min speed", &FlashStorage::data.min_speed, 10, "%d");
        changeValue<float>("Slope param", &FlashStorage::data.slope_param, .1, "%.2f");
        changeValue<float>("X shift", &FlashStorage::data.x_shift, .1, "%.2f");

        FlashStorage::save();

        showPlot();

        volatile bool is_continue = false;

        std::function<void(ui::E&)> dynamic_handler = [&](ui::E& e){
            is_continue = true;
            e.consume();
        };

        ui::Context::addEventListener(ui::EventType::JOYSTICK_DOWN, &dynamic_handler);

        while (!is_continue) {}

        ui::Context::removeEventListener(ui::EventType::JOYSTICK_DOWN, &dynamic_handler);
    }

    goto exit;
    reset:
    screen_adapter_ptr->clear();

    {
        tb.setText("Select to reset flash!");
        tb.setTextWrap(ui::text::WRAP);
        tb.setRegion(0, 0, 128, 50);
        tb.render();

        volatile bool is_continue = false;

        std::function<void(ui::E&)> reset_handler = [&](ui::E& e){
            if (e.JOYSTICK_STATE == ui::JoystickState::SELECT) {
                is_continue = true;
                e.consume();
            }
        };

        ui::Context::addEventListener(ui::EventType::JOYSTICK_DOWN, &reset_handler);

        while (!is_continue) {}

        FlashStorage::data = FlashStorage::Data{};
        FlashStorage::save();

        ui::Context::removeEventListener(ui::EventType::JOYSTICK_DOWN, &reset_handler);

        goto start;
    }

    exit: {};
}
