/*
 * global.cpp
 *
 *  Created on: Jul 20, 2017
 *      Author: LeeChunHei
 */

#include <global.h>

//Global pointer
libsc::Led *led1 = nullptr;
libsc::Led *led2 = nullptr;
libsc::Led *led3 = nullptr;
libsc::Led *led4 = nullptr;
libsc::St7735r* lcd = nullptr;
libsc::LcdConsole* console = nullptr;
libsc::BatteryMeter *battery_meter = nullptr;
libsc::k60::Ov7725 *cam = nullptr;
libsc::Servo *servo = nullptr;
libsc::DirMotor *motor = nullptr;
libsc::AbEncoder *encoder = nullptr;
libsc::k60::JyMcuBt106 *bt = nullptr;
//util::BTComm *bt = nullptr;
libbase::k60::Flash *flash = nullptr;
//Pit *pit = nullptr;
libsc::Joystick* joystick = nullptr;

//Global variable
const Byte* buff = nullptr;
bool debug = false;
uint8_t contrast = 0x40;
uint8_t corner_window_size = 11;
uint8_t corner_min = 0;
uint8_t corner_max = 0;
