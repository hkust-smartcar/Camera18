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

    FlashStorage::load();


    //
    // Initialize Menu Group
    //

    ui::MenuGroup menu_group("Camera 18");
    menu_group.setHasBackArrow(false);


    //
    // Declare and initialize various Menu Items to be added to menu_group.
    //

    class RunAction: public ui::MenuAction {
    public:
        explicit RunAction(ui::MenuGroup* menu_group_ptr): menu_group_ptr(menu_group_ptr) {
            this->setName("Run");
        }
        int run() override {
            menu_group_ptr->exitMenu();
            return SUCCESS;
        }

    private:
        ui::MenuGroup* menu_group_ptr;
    } run_action(&menu_group);

    class CameraPreviewAction: public ui::MenuAction {
    public:
        CameraPreviewAction() {
            this->setName("Camera Preview");
        }
        int run() override {
            auto* screen_ptr = ui::Context::getScreen();

            //Change screen
            screen_ptr->setRegion(0, 0, screen_ptr->getWidth(), screen_ptr->getHeight());
            screen_ptr->fill(ui::Context::color_scheme.GRAY_DARKER);

            //Render loop
            Timer::TimerInt time = System::Time();

            bool is_exit = false;
            const Byte* buffer = camera->LockBuffer();
            uint16_t offset_x = 0;

            inline Byte getXY(uint16_t x, uint16_t y) {return buffer[(x) + (y) * 189];};

            std::function<void(ui::E&)> joystick_handler = [&](ui::E& e){
                if (e.JOYSTICK_STATE == ui::JoystickState::SELECT) {
                    is_exit = true;
                } else if (e.JOYSTICK_STATE == ui::JoystickState::LEFT) {
                    offset_x = (uint16_t) std::max(0, offset_x - 10);
                } else if (e.JOYSTICK_STATE == ui::JoystickState::RIGHT) {
                    offset_x = (uint16_t) std::min(189 - 160, offset_x + 10);
                }
                e.consume();
            };

            ui::Context::addEventListener(ui::Event::JOYSTICK_DOWN, &joystick_handler);

            camera->Start();

            while (!is_exit) {
                if (time != System::Time()) {
                    time = System::Time();
                    if (time % 200 == 0) {
                        //Updates image
                        buffer = camera->LockBuffer();

                        //Construct viewport array
                        uint8_t viewport[160*120];

                        uint16_t i = 0;

                        for (uint16_t y = 0; y < 120; y++) {
                            for (uint16_t x = 0; x < 160; x++) {
                                viewport[i] = getXY(x + offset_x, y);
                            }
                        }

                        lcd->SetRegion({0, 0, 160, 120});
                        lcd->FillGrayscalePixel(viewport, 160*120);
                    }
                }
            }

            ui::Context::removeEventListener(ui::Event::JOYSTICK_DOWN, &joystick_handler);
            camera->Stop();

            return SUCCESS;
        }
    } camera_preview_action;

    int servo_integer_test_angle = 0;

    class ServoInteger: public ui::MenuNumber<int> {
    public:
        explicit ServoInteger(int* i_ptr, libsc::Servo* servo_ptr): i_ptr(i_ptr), servo_ptr(servo_ptr) {
            this->setName("Turn Servo");
            this->setValue(0);
            this->setStep(5);
        }

        void onEnter() override {
            setValue(*i_ptr);
        }

        void onChange() override {
            servo_ptr->SetDegree((uint16_t) (900 + value));
            *i_ptr = value;
        }

    private:
        int* i_ptr;
        libsc::Servo* servo_ptr;
    } servo_integer(&servo_integer_test_angle, servo);


    //
    // Add items to menu_group
    //

    menu_group.addMenuActions({
                                      &run_action,
                                      &servo_integer,
                                      &camera_preview_action
    });

    //
    // Enter menu and its render loop.
    // The code will continue when the user hits the "Run" option, which exits the menu.
    //

    menu_group.run();
}
