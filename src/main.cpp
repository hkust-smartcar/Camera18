/*
 * main.cpp
 *
 *  Created on: Jul 18, 2017
 *      Author: LeeChunHei
 */

#include <global.h>

//Required DON'T DELETE IT !!!
namespace libbase {
namespace k60 {
Mcg::Config Mcg::GetMcgConfig() {
	Mcg::Config config;
	config.external_oscillator_khz = 50000;
	config.core_clock_khz = 200000;
	config.flexbus_clock_khz = 200000;
	return config;
}
}
}

int main() {

	libsc::System::Init();

//Flash Init
	libbase::k60::Flash::Config flash_config;
	libbase::k60::Flash flash_(flash_config);
	flash = &flash_;

//Led Init
	libsc::Led::Config led_config;
	led_config.id = 0;
	led_config.is_active_low = true;
	libsc::Led led1_(led_config);
	led1 = &led1_;
	led_config.id = 1;
	libsc::Led led2_(led_config);
	led2 = &led2_;
	led_config.id = 2;
	libsc::Led led3_(led_config);
	led3 = &led3_;
	led_config.id = 3;
	libsc::Led led4_(led_config);
	led4 = &led4_;

//Lcd Init
	libsc::St7735r::Config lcd_config;
	lcd_config.fps = 60;
	lcd_config.is_bgr = false;
	lcd_config.is_revert = true;
	libsc::St7735r lcd_(lcd_config);
	lcd = &lcd_;
	
//Console Init
	libsc::LcdConsole::Config console_config;
	console_config.lcd = lcd;
	console_config.region = libsc::Lcd::Rect(0, 0, lcd->GetW(), lcd->GetH());
	libsc::LcdConsole console_(console_config);
	console = &console_;

//Battery Meter Init
	libsc::BatteryMeter::Config battery_meter_config;
	battery_meter_config.voltage_ratio = 0.4;
	libsc::BatteryMeter battery_meter_(battery_meter_config);
	battery_meter = &battery_meter_;

//Servo Init
	libsc::FutabaS3010::Config servo_config;
	servo_config.id = 0;
	libsc::FutabaS3010 servo_(servo_config);
	servo = &servo_;

//Bluetooth Init
	libsc::k60::JyMcuBt106::Config bt_config;
	bt_config.baud_rate = libbase::k60::Uart::Config::BaudRate::k115200;
	bt_config.id = 0;
	libsc::k60::JyMcuBt106 bt_(bt_config);
	bt = &bt_;

//Motor Init
	libsc::DirMotor::Config motor_config;
	motor_config.id = 0;
	libsc::DirMotor motor_(motor_config);
	motor = &motor_;

//Joystick Init
	libsc::Joystick::Config joystick_config;
	joystick_config.id = 0;
	joystick_config.is_active_low = true;
	libsc::Joystick joystick_(joystick_config);
	joystick = &joystick_;

//Encoder Init
	libsc::AbEncoder::Config encoder_config;
	encoder_config.id = 0;
	libsc::AbEncoder encoder_(encoder_config);
	encoder = &encoder_;

//Menu Init
	libutil::Menu menu(lcd, console, joystick, battery_meter, flash);
	menu.AddItem("Engine On!!!", std::bind(&algo), &menu.main_menu);
	menu.AddItem("Debug Mode", &debug, "on", "off", &menu.main_menu);
	menu.AddItem("contrast", &contrast, 1, &menu.main_menu);
	menu.AddItem("servo tune", std::bind(&util::servo_tune), &menu.main_menu);

	util::StartAction();

	menu.EnterMenu(&menu.main_menu);

	util::EndAction();
}
