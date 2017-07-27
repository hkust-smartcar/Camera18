/*
 * global.cpp
 *
 *  Created on: Jul 20, 2017
 *      Author: LeeChunHei
 */

#include <global.h>

//Global pointer
Led *led1 = nullptr;
Led *led2 = nullptr;
Led *led3 = nullptr;
Led *led4 = nullptr;
Joystick *joystick = nullptr;
St7735r *lcd = nullptr;
LcdConsole *console = nullptr;
LcdTypewriter *type_writer = nullptr;
BatteryMeter *battery_meter = nullptr;
PassiveBuzzer *buzzer = nullptr;
Ov7725 *cam = nullptr;
FutabaS3010 *servo = nullptr;
DirMotor *motor1 = nullptr;
DirMotor *motor2 = nullptr;
DirEncoder *encoder1 = nullptr;
DirEncoder *encoder2 = nullptr;
JyMcuBt106 *original_bt = nullptr;
BTComm *bt = nullptr;
Flash *flash = nullptr;
Pit *pit = nullptr;

//Global variable
uint8_t contrast = 0x40;
const uint16_t cam_height = 60;
const uint16_t cam_width = 80;
const uint16_t servo_right_bound = 1380;
const uint16_t servo_center = 1060;
const uint16_t servo_left_bound = 750;
