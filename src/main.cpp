/*
 * main.cpp
 *
 *  Created on: Jul 18, 2017
 *      Author: LeeChunHei
 */

#include <global.h>

//Required DON'T DELETE IT !!!
namespace libbase
{
	namespace k60
	{
		Mcg::Config Mcg::GetMcgConfig()
		{
			Mcg::Config config;
			config.external_oscillator_khz = 50000;
			config.core_clock_khz = 150000;
			return config;
		}
	}
}

int main() {
	System::Init();

	//Led Init
	Led::Config config_led;
	config_led.id = 0;
	config_led.is_active_low = true;
	Led led1_(config_led);
	led1 = &led1_;
	config_led.id = 1;
	Led led2_(config_led);
	led2 = &led2_;
	config_led.id = 2;
	Led led3_(config_led);
	led3 = &led3_;
	config_led.id = 3;
	Led led4_(config_led);
	led4 = &led4_;

	//Joystick Init
	Joystick::Config config_joystick;
	config_joystick.id = 0;
	config_joystick.is_active_low = true;
	Joystick joystick_(config_joystick);
	joystick = &joystick_;

	//LCD Init
	St7735r::Config config_lcd;
	config_lcd.is_revert = true;
	config_lcd.is_bgr = false;
	config_lcd.fps = 60;
	St7735r lcd_(config_lcd);
	lcd = &lcd_;

	//Console Init
    LcdConsole::Config config_console;
	config_console.lcd = &lcd_;
	config_console.region = Lcd::Rect(5, 0, 123, 160);
    LcdConsole console_(config_console);
    console = &console_;

	//Type Writer Init
	LcdTypewriter::Config config_type_writer;
	config_type_writer.lcd = &lcd_;
	LcdTypewriter type_writer_(config_type_writer);
	type_writer = &type_writer_;

	//Battery Meter Init
	BatteryMeter::Config config_battery_meter;
	config_battery_meter.voltage_ratio = 0.4;
	BatteryMeter battery_meter_(config_battery_meter);
	battery_meter = &battery_meter_;

	//Passive Buzzer Init
	PassiveBuzzer::Config config_passive_buzzer;
	PassiveBuzzer buzzer_(config_passive_buzzer);
	buzzer = &buzzer_;

	//Servo Init
	FutabaS3010::Config config_servo;
	config_servo.id = 0;
	FutabaS3010 servo_(config_servo);
	servo = &servo_;

	//Bluetooth Init
	libsc::k60::JyMcuBt106::Config bt_config;
	bt_config.baud_rate = libbase::k60::Uart::Config::BaudRate::k115200;
	bt_config.id = 0;
	libsc::k60::JyMcuBt106 bt_(bt_config);
	bt = &bt_;

	//Flash Init
	Flash::Config flash_config;
	Flash flash_(flash_config);
	flash = &flash_;

	//Motor Init
	libsc::DirMotor::Config motor_config;
	motor_config.id = 0;
	libsc::DirMotor left_motor_(motor_config);
	left_motor = &left_motor_;
	motor_config.id = 1;
	libsc::DirMotor right_motor_(motor_config);
	right_motor = &right_motor_;

	//Encoder Init
	libsc::DirEncoder::Config encoder_config;
	encoder_config.id = 0;
	libsc::DirEncoder left_encoder_(encoder_config);
	left_encoder = &left_encoder_;
	encoder_config.id = 1;
	libsc::DirEncoder right_encoder_(encoder_config);
	right_encoder = &right_encoder_;

	//Control Init
	util::Control car_(servo, left_encoder, right_encoder, left_motor, right_motor, 0, 0, 0, servo_left_bound, servo_right_bound);
	car = &car_;

	util::battery_test();

//	util::led_test();

//	util::servo_tune(); //left bound 750 center 1060 right bound 1380

	lcd->SetRegion(Lcd::Rect(0, 0, 128, 160));
	lcd->Clear(true);

	//Start up action
	console->Clear(true);
	util::PrintLogo();
	console->SetTextColor(Lcd::kGreen);
	console->SetCursorRow(2);
	console->WriteString("Camera 2018");
	console->SetCursorRow(3);
	console->WriteString("version 1.0");
	console->SetCursorRow(7);
	console->WriteString("Created by");
	console->SetCursorRow(8);
	console->WriteString("Lee Chun Hei");
	console->SetCursorRow(9);
	console->WriteString("(Leslie)");
	System::DelayMs(1500);

	Ov7725::Config config_cam;
	config_cam.contrast = contrast;
	config_cam.fps = Ov7725::Config::Fps::kHigh;
	config_cam.id = 0;
	config_cam.h = 480;
	config_cam.w = 128;
	Ov7725 cam_(config_cam);
	cam = &cam_;

	util::Menu menu(lcd, console, joystick, flash);
	menu.AddItem("contrast", &contrast, 1, &menu.main_menu);
	menu.AddItem("battery level", std::bind(&util::battery_test), &menu.main_menu);
	menu.AddItem("servo tune", std::bind(&util::servo_tune), &menu.main_menu);
	menu.AddItem("snake game", std::bind(&snake_game, joystick, lcd, console), &menu.main_menu);
	menu.EnterMenu(&menu.main_menu);

	//Program ended action
	while (true) {
		console->Clear(true);
		console->SetTextColor(Lcd::kRed);
		console->SetCursorRow(0);
		console->WriteString("Program Ended");
		System::DelayMs(250);
	}
}
