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
#include<libsc/k60/ov7725.h>

//Servo Header File
#include<libsc/futaba_s3010.h>

//Motor Header File
#include<libsc/dir_motor.h>
#include<libsc/motor.h>

//Dir Encoder Header File
#include<libsc/dir_encoder.h>

//Bluetooth Header File
#include <libsc/k60/jy_mcu_bt_106.h>
#include <../inc/util/bluetooth.h>

//Battery Meter Header File
#include <libsc/battery_meter.h>

//Flash Header File
#include <libbase/k60/flash.h>

//Pit Header File
#include <libbase/k60/pit.h>

//Menu Header File
#include <../inc/util/menu.h>

//Util Header File
#include <../inc/util/util.h>

//Snake Game Header File
//#include <../inc/game/snake_game.h>

//namespace
using namespace libsc;
using namespace libsc::k60;
using namespace libbase::k60;
using namespace libutil;
using namespace std;

//Struct
struct coor {
	uint8_t x;
	uint8_t y;
};

//Global pointer
extern Led *led1;
extern Led *led2;
extern Led *led3;
extern Led *led4;
extern Joystick *joystick;
extern St7735r *lcd;
extern LcdConsole *console;
extern LcdTypewriter *type_writer;
extern BatteryMeter *battery_meter;
extern PassiveBuzzer *buzzer;
extern Ov7725 *cam;
extern FutabaS3010 *servo;
extern DirMotor *motor1;
extern DirMotor *motor1;
extern DirEncoder *encoder1;
extern DirEncoder *encoder2;
extern JyMcuBt106 *original_bt;
extern BTComm *bt;
extern Flash *flash;
extern Pit *pit;

//Global variable
extern uint8_t contrast;
extern const uint16_t cam_height;
extern const uint16_t cam_width;
extern const uint16_t servo_right_bound;
extern const uint16_t servo_center;
extern const uint16_t servo_left_bound;



#endif /* INC_GLOBAL_H_ */
