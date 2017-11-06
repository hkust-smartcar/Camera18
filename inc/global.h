/*
 * global.h
 *
 *  Created on: Jul 18, 2017
 *      Author: LeeChunHei
 */

#ifndef INC_GLOBAL_H_
#define INC_GLOBAL_H_

#include <cassert>
#include <cstring>
#include <string>
#include <libbase/k60/mcg.h>
#include <libsc/system.h>
#include <functional>
#include <vector>

//led Header File
#include <libsc/led.h>

//Joystick Header File
#include <libsc/joystick.h>

//LCD Header File
#include <libsc/st7735r.h>
#include <libsc/lcd_console.h>
#include <libsc/lcd_typewriter.h>

//Buzzer Header File
#include <libsc/passive_buzzer.h>

//Camera Header File
#include <libsc/k60/ov7725.h>

//Servo Header File
#include <libsc/servo.h>

//Motor Header File
#include <libsc/dir_motor.h>
#include <libsc/motor.h>

//AB Encoder Header File
#include <libsc/ab_encoder.h>

//Bluetooth Header File
#include <libsc/k60/jy_mcu_bt_106.h>
//#include <../inc/util/bluetooth.h>

//Battery Meter Header File
#include <libsc/battery_meter.h>

//Flash Header File
#include <libbase/k60/flash.h>

//Speed Control Header File
#include <../inc/util/control.h>

//Menu Header File
#include <libutil/normal_menu.h>

//Util Header File
#include <../inc/util/util.h>

//Algo Header File
#include <../inc/algo.h>
#include "util/control_carC.h"

//namespace
using namespace std;

//Struct
struct coor {
	uint8_t x;
	uint8_t y;
};

//Global pointer
extern libsc::Led *led1;
extern libsc::Led *led2;
extern libsc::Led *led3;
extern libsc::Led *led4;
extern libsc::St7735r* lcd;
extern libsc::LcdConsole* console;
extern libsc::BatteryMeter *battery_meter;
extern libsc::k60::Ov7725 *cam;
extern libsc::Servo *servo;
extern libsc::DirMotor *motor;
extern libsc::AbEncoder *encoder;
extern libsc::k60::JyMcuBt106 *bt;
//extern util::BTComm *bt;
extern libbase::k60::Flash *flash;
extern libsc::Joystick* joystick;

//Global variable
extern const Byte* buff;
extern bool debug;
extern uint8_t contrast;
extern uint8_t corner_window_size;
extern uint8_t corner_min;
extern uint8_t corner_max;
#define cam_height 120
#define cam_width 160
#define servo_right_bound 1380
#define servo_center 1060
#define servo_left_bound 750



#endif /* INC_GLOBAL_H_ */
