//
// Created by Daniel on 10/6/2018.
//

#include "adapters/application.h"

void Application::init() {
    //
    // Prepare UI
    //

    if (ui::Context::getContext() == nullptr) {
        context_adapter_ptr = new adapters::ContextAdapter;
        ui::Context::attachAdapter(context_adapter_ptr);

        screen_adapter_ptr = adapters::ContextAdapter::screen_adapter_ptr = new adapters::ScreenAdapter;
        battery_meter_adapter_ptr = adapters::ContextAdapter::battery_meter_adapter_ptr = new adapters::BatteryMeterAdapter;
        joystick_adapter_ptr = adapters::ContextAdapter::joystick_adapter_ptr = new adapters::JoystickAdapter;

        screen_adapter_ptr->lcd_ptr = new libsc::St7735r([](){
            libsc::St7735r::Config config;
            config.fps = 60;
            config.is_bgr = false;
            config.orientation = 2;
            return config;
        }());

        libsc::BatteryMeter::Config battery_meter_config{0.4};
        battery_meter_adapter_ptr->battery_meter_ptr = new libsc::BatteryMeter(battery_meter_config);

        adapters::ContextAdapter::joystick_config_base.id = 0;
        adapters::ContextAdapter::joystick_config_base.is_active_low = true;

        ui::Context::prepareListenerEvents();
        ui::Context::remapJoystickDirections(ui::JoystickState::LEFT, ui::JoystickState::UP, ui::JoystickState::RIGHT, ui::JoystickState::DOWN);
    }

    ui::Context::addFontToRepo("Humanist", &humanist);
    ui::Context::addFontToRepo("Blocky", &blocky);
}

void Application::done() {
    //
    // Clean removal of fonts to save memory
    //

    ui::Context::removeFontFromRepo("Humanist");
    ui::Context::removeFontFromRepo("Blocky");
}

void Application::run() {
    init();
    start();
    done();
}
