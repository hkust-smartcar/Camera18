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

//Global Pointers
libbase::k60::Flash* flash;
libsc::RGBLed* led0;
libsc::RGBLed* led1;
libsc::RGBLed* led2;
libsc::RGBLed* led3;
libsc::St7735r* lcd;
libsc::LcdConsole* console;
libsc::BatteryMeter* battery_meter;
libsc::Servo* servo;
libsc::k60::JyMcuBt106* bt;
libsc::AlternateMotor* motor;
libsc::Joystick* joystick;
libsc::AbEncoder* encoder;


#endif /* INC_GLOBAL_H_ */
