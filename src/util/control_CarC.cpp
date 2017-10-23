///*
// * speed_control.cpp
// *
// *  Created on: Jul 28, 2017
// *      Author: LeeChunHei
// */
//#include "../../inc/util/control_carC.h"
//namespace util {
//
//libsc::DirEncoder *Control::left_encoder = nullptr;
//libsc::DirEncoder *Control::right_encoder = nullptr;
//libsc::AlternateMotor *Control::left_motor = nullptr;
//libsc::AlternateMotor *Control::right_motor = nullptr;
//int16_t Control::left_speed = 0;
//int16_t Control::right_speed = 0;
//float Control::kP = 0;
//float Control::kI = 0;
//float Control::kD = 0;
//int16_t Control::left_error_sum = 0;
//int16_t Control::right_error_sum = 0;
//int16_t Control::left_last_error = 0;
//int16_t Control::right_last_error = 0;
//int16_t Control::left_encoder_count = 0;
//int16_t Control::right_encoder_count = 0;
//
//Control::Control(libsc::FutabaS3010 *servo, libsc::DirEncoder *left_encoder, libsc::DirEncoder *right_encoder, libsc::AlternateMotor *left_motor, libsc::AlternateMotor *right_motor, uint16_t kP, uint16_t kI, uint16_t kD, uint16_t servo_left_bound, uint16_t servo_right_bound) {
//	this->servo = servo;
//	this->left_encoder = left_encoder;
//	this->right_encoder = right_encoder;
//	this->left_motor = left_motor;
//	this->right_motor = right_motor;
//	this->kP = kP;
//	this->kI = kI;
//	this->kD = kD;
//	this->servo_left_bound = servo_left_bound;
//	this->servo_right_bound = servo_right_bound;
//	libbase::k60::Pit::Config config;
//	config.channel = 2;
//	config.count = 37500 * 10; //5ms
//	config.is_enable = true;
//	config.isr = &listener;
//	pit = new libbase::k60::Pit(config);
//}
//
//void Control::listener(libbase::k60::Pit *pit) {
//	left_encoder->Update();
//	right_encoder->Update();
//	int16_t left_curr = left_encoder->GetCount();
//	int16_t right_curr = -right_encoder->GetCount();
//	left_encoder_count += left_curr;
//	right_encoder_count += right_curr;
//	left_curr = left_curr - left_speed;
//	right_curr = right_curr - right_speed;
//	left_error_sum += left_curr;
//	right_error_sum += right_curr;
//	int16_t left_output = kP * left_curr + kI * left_error_sum + kD * (left_curr - left_last_error);
//	int16_t right_output = kP * right_curr + kI * right_error_sum + kD * (right_curr - right_last_error);
//	left_last_error = left_curr;
//	right_last_error = right_curr;
//	if (left_output > 0) {
//		left_motor->SetClockwise(false);
//	} else {
//		left_motor->SetClockwise(true);
//		left_output = -left_output;
//	}
//	if (right_output > 0) {
//		right_motor->SetClockwise(true);
//	} else {
//		right_motor->SetClockwise(false);
//		right_output = -right_output;
//	}
//	left_motor->SetPower(left_output);
//	right_motor->SetPower(right_output);
//}
//
//}
//
//
//
//
