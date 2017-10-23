/*
 * control.h
 *
 *  Created on: Oct 23, 2017
 *      Author: LeeChunHei
 */

#ifndef INC_UTIL_CONTROL_H_
#define INC_UTIL_CONTROL_H_

#include <cassert>
#include <cstring>
#include <string>
#include <libutil/misc.h>
#include <libbase/k60/mcg.h>
#include <libsc/system.h>
#include <libsc/futaba_s3010.h>
#include <libsc/dir_motor.h>
#include <libsc/motor.h>
#include <libsc/ab_encoder.h>
#include <libbase/k60/pit.h>

namespace util {

class Control {
public:
	Control(libsc::DirMotor* motor, libsc::AbEncoder* encoder, float kP, float kI, float kD);

	libbase::k60::Pit::Config GetPitConfig(int channel, int count);
	static void listener(libbase::k60::Pit* pit);

	void StartControl(bool flag);
	void SetSpeed(int speed);
	void SetkP(float kP);
	void SetkI(float kI);
	void SetkD(float kD);
	int GetCount() const;
	
private:
	libbase::k60::Pit pit;
	static libsc::DirMotor* motor;
	static libsc::AbEncoder* encoder;
	static float kP;
	static float kI;
	static float kD;
	static int speed;
	static int count;
	static int curr;
	static int last_error;
	static int error_sum;
	static int output;
};

}



#endif /* INC_UTIL_CONTROL_H_ */
