/*
 * main.cpp
 *
 *  Created on: May 28, 2018
 *      Author: LeeChunHei
 */

#include <global.h>
#include <camera_menu.h>

//Required DON'T DELETE IT !!!
namespace libbase {
namespace k60 {
Mcg::Config Mcg::GetMcgConfig() {
	Mcg::Config config;
	config.external_oscillator_khz = 50000;
	config.core_clock_khz = 200000;
	return config;
}
}
}

int main() {

	libsc::System::Init();

	//Camera Init
	libsc::k60::MT9V034::Config camera_config;
	camera_config.h_binning = camera_config.k4;
	camera_config.w_binning = camera_config.k4;
	libsc::k60::MT9V034 camera_(camera_config);
	camera = &camera_;

	//Flash Init
	libbase::k60::Flash::Config flash_config;
	libbase::k60::Flash flash_(flash_config);
	flash = &flash_;

	//RGBLed Init
	libsc::RGBLed::Config led_config;
	led_config.id = 0;
	led_config.is_active_low = true;
	libsc::RGBLed led0_(led_config);
	led0 = &led0_;
	led_config.id = 1;
	libsc::RGBLed led1_(led_config);
	led1 = &led1_;
	led_config.id = 2;
	libsc::RGBLed led2_(led_config);
	led2 = &led2_;
	led_config.id = 3;
	libsc::RGBLed led3_(led_config);
	led3 = &led3_;

	//Buzzer Init
	libsc::PassiveBuzzer::Config buzzer_config;
	libsc::PassiveBuzzer buzzer_(buzzer_config);
	buzzer = &buzzer_;

	//Lcd Init
	libsc::St7735r::Config lcd_config;
	lcd_config.fps = 60;
	lcd_config.is_bgr = false;
	lcd_config.orientation = 1;
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
	libsc::Servo::Config servo_config;
	servo_config.id = 0;
	servo_config.period = 3333;
	servo_config.max_pos_width = 2000;
	servo_config.min_pos_width = 1000;
	libsc::Servo servo_(servo_config);
	servo = &servo_;

	//Bluetooth Init
	libsc::k60::JyMcuBt106::Config bt_config;
	bt_config.baud_rate = libbase::k60::Uart::Config::BaudRate::k115200;
	bt_config.id = 0;
	libsc::k60::JyMcuBt106 bt_(bt_config);
	bt = &bt_;

	//Motor Init
	libsc::AlternateMotor::Config motor_config;
	motor_config.id = 0;
	libsc::AlternateMotor motor_(motor_config);
	motor = &motor_;

	//Joystick Init
//	libsc::Joystick::Config joystick_config;
//	joystick_config.id = 0;
//	joystick_config.is_active_low = true;
//	libsc::Joystick joystick_(joystick_config);
//	joystick = &joystick_;

	//Encoder Init
	libsc::AbEncoder::Config encoder_config;
	encoder_config.id = 0;
	libsc::AbEncoder encoder_(encoder_config);
	encoder = &encoder_;

	//initialize lcd typewriter
	libsc::LcdTypewriter::Config typeconfig;
	typeconfig.lcd = &lcd_;
	libsc::LcdTypewriter writer(typeconfig);
	writerP = &writer;

	//MPU6050 Init
//	libsc::Mpu6050::Config mpu_config;
//	mpu_config.accel_range = mpu_config.Range::kSmall;
//	mpu_config.gyro_range = mpu_config.Range::kSmall;
//	mpu_config.cal_drift = true;
//	libsc::Mpu6050 mpu_(mpu_config);
//	mpu = &mpu_;

    CameraMenu::show();

	algo();
}
