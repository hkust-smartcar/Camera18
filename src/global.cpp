/*
 * global.cpp
 *
 *  Created on: May 30, 2018
 *      Author: LeeChunHei
 */

#include <global.h>

libbase::k60::Flash* flash = nullptr;
libsc::RGBLed* led0 = nullptr;
libsc::RGBLed* led1 = nullptr;
libsc::RGBLed* led2 = nullptr;
libsc::RGBLed* led3 = nullptr;
libsc::PassiveBuzzer* buzzer = nullptr;
libsc::St7735r* lcd = nullptr;
libsc::LcdConsole* console = nullptr;
libsc::BatteryMeter* battery_meter = nullptr;
libsc::Servo* servo = nullptr;
libsc::k60::JyMcuBt106* bt = nullptr;
libsc::AlternateMotor* motor = nullptr;
libsc::Joystick* joystick = nullptr;
libsc::AbEncoder* encoder = nullptr;

int edge_threshold = 150;
