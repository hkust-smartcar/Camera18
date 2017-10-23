///*
// * control.h
// *
// *  Created on: Jul 25, 2017
// *      Author: LeeChunHei
// */
//
//#ifndef INC_UTIL_CONTROL_CARC_H_
//#define INC_UTIL_CONTROL_CARC_H_
//
//#include <cassert>
//#include <cstring>
//#include <string>
//#include <libutil/misc.h>
//#include <libbase/k60/mcg.h>
//#include <libsc/system.h>
//#include <libsc/futaba_s3010.h>
//#include <libsc/alternate_motor.h>
//#include <libsc/motor.h>
//#include <libsc/dir_encoder.h>
//#include <libbase/k60/pit.h>
//
//
//namespace util {
//
//class Control {
//public:
//	Control(libsc::FutabaS3010 *servo, libsc::DirEncoder *left_encoder, libsc::DirEncoder *right_encoder, libsc::AlternateMotor *left_motor, libsc::AlternateMotor *right_motor, uint16_t kP, uint16_t kI, uint16_t kD, uint16_t servo_left_bound, uint16_t servo_right_bound);
//
//	void SetSpeed(uint16_t speed) {
//		this->speed = speed;
//		this->left_speed = speed;
//		this->right_speed = speed;
//	}
//
//	void SetAngle(uint16_t angle) {
//		this->angle = libutil::Clamp(this->servo_left_bound, angle, this->servo_right_bound);
//		servo->SetDegree(this->angle);
//	}
//
//	void ResetErrorSum() {
//		left_error_sum = 0;
//		right_error_sum = 0;
//	}
//
//	int16_t GetLeftCount() {
//		int16_t count = left_encoder_count;
//		left_encoder_count = 0;
//		return count;
//	}
//
//	int16_t GetRightCount() {
//		int16_t count = right_encoder_count;
//		right_encoder_count = 0;
//		return count;
//	}
//
//private:
//	libbase::k60::Pit *pit = nullptr;
//	libsc::FutabaS3010 *servo = nullptr;
//	static libsc::DirEncoder *left_encoder;
//	static libsc::DirEncoder *right_encoder;
//	static libsc::AlternateMotor *left_motor;
//	static libsc::AlternateMotor *right_motor;
//	int16_t speed = 0;
//	uint16_t angle = 0;
//	uint16_t servo_left_bound = 0;
//	uint16_t servo_right_bound = 0;
//	static int16_t left_speed;
//	static int16_t right_speed;
//	static float kP;
//	static float kI;
//	static float kD;
//	static int16_t left_error_sum;
//	static int16_t right_error_sum;
//	static int16_t left_last_error;
//	static int16_t right_last_error;
//	static int16_t left_encoder_count;
//	static int16_t right_encoder_count;
//	static void listener(libbase::k60::Pit *pit);
//};
//
//}
//
//
//
//#endif /* INC_UTIL_CONTROL_CARC_H_ */
