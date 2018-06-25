/*
 * global.h
 *
 *  Created on: May 28, 2018
 *      Author: LeeChunHei
 */

#ifndef INC_GLOBAL_H_
#define INC_GLOBAL_H_

//Essential Header
#include <libbase/k60/mcg.h>
#include <cassert>
#include <cstring>
#include <string>
#include <libsc/system.h>
#include <functional>
#include <vector>
#include <math.h>


//RGB LED Header File
#include <libsc/rgb_led.h>

//Joystick Header File
#include <libsc/joystick.h>

//LCD Header File
#include <libsc/st7735r.h>
#include <libsc/lcd_console.h>
#include <libsc/lcd_typewriter.h>

//Buzzer Header File
#include <libsc/passive_buzzer.h>
#include <libutil/notes.h>

//Camera Header File
#include<libsc/k60/Mt9V034.h>

//Servo Header File
#include<libsc/servo.h>

//Motor Header File
#include<libsc/alternate_motor.h>

//AB Encoder Header File
#include<libsc/AB_encoder.h>

//Bluetooth Header File
#include <libsc/k60/jy_mcu_bt_106.h>

//Flash Header File
#include <libbase/k60/flash.h>

//Battery Meter Header File
#include <libsc/battery_meter.h>

//MPU6050 Header File
#include <libsc/mpu6050.h>

//Algo Header File
#include <algo.h>

//Global Pointers
extern libbase::k60::Flash* flash;
extern libsc::RGBLed* led0;
extern libsc::RGBLed* led1;
extern libsc::RGBLed* led2;
extern libsc::RGBLed* led3;
extern libsc::PassiveBuzzer* buzzer;
extern libsc::St7735r* lcd;
extern libsc::LcdConsole* console;
extern libsc::BatteryMeter* battery_meter;
extern libsc::Servo* servo;
extern libsc::k60::JyMcuBt106* bt;
extern libsc::AlternateMotor* motor;
extern libsc::Joystick* joystick;
extern libsc::AbEncoder* encoder;
extern libsc::k60::MT9V034* camera;
extern libsc::LcdTypewriter* writerP;
extern libsc::Mpu6050* mpu;

//Global Variables
extern int edge_threshold;
extern const int img2world[189][120][2];

struct coor {
	int16_t x;
	int16_t y;
};

#endif /* INC_GLOBAL_H_ */
