/*
 * control.cpp
 *
 *  Created on: Oct 23, 2017
 *      Author: LeeChunHei
 */

#include <../inc/util/control.h>

namespace util {

libsc::DirMotor* motor = nullptr;
libsc::AbEncoder* encoder = nullptr;
float kP = 0;
float kI = 0;
float kD = 0;
int speed = 0;
int count = 0;
int curr = 0;
int last_error = 0;
int error_sum = 0;
int output = 0;

Control::Control(libsc::DirMotor* motor, libsc::AbEncoder* encoder, float kP, float kI, float kD) :
		pit(GetPitConfig(2, 37500 * 10)) {
	this->motor = motor;
	this->encoder = encoder;
	this->kP = kP;
	this->kI = kI;
	this->kD = kD;
	speed = 0;
	count = 0;
	curr = 0;
	last_error = 0;
	error_sum = 0;
	output = 0;
}

libbase::k60::Pit::Config Control::GetPitConfig(int channel, int count) {
	libbase::k60::Pit::Config config;
	config.channel = channel;
	config.count = count;
	config.is_enable = false;
	config.isr = &listener;
	return config;
}

void Control::listener(libbase::k60::Pit* pit) {
	encoder->Update();
	curr = encoder->GetCount();
	count += curr;
	curr -= speed;
	error_sum += curr;
	output = kP * curr + kI * error_sum + kD * (curr - last_error);
	last_error = curr;
	if (output < 0) {
		motor->SetClockwise(false);
		motor->SetPower(-output);
	} else
		motor->SetPower(output);
}

void Control::StartControl(bool flag) {
	pit.SetEnable(flag);
}

void Control::SetSpeed(int speed) {
	this->speed = speed;
}

void Control::SetkP(float kP) {
	this->kP = kP;
}

void Control::SetkI(float kI) {
	this->kI = kI;
}

void Control::SetkD(float kD) {
	this->kD = kD;
}

int Control::GetCount() const {
	return count;
}

}


