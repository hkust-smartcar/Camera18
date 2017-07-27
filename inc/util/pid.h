/*
 * pid.h
 *
 *  Created on: Jul 24, 2017
 *      Author: LeeChunHei
 */

#ifndef INC_UTIL_PID_H_
#define INC_UTIL_PID_H_

#include <../inc/global.h>

namespace util {

class pid {
public:
	pid(float kP = 0, float kI = 0, float kD = 0) {
		this->kP = kP;
		this->kI = kI;
		this->kD = kD;
		last_trigger_time = System::Time();
	}

	int16_t PowerOutput(int16_t target_speed, int16_t curr_speed) {
		curr_error = target_speed - curr_speed;
		int16_t time_diff = System::Time() - last_trigger_time;
		last_trigger_time = System::Time();
		cumulative_error += curr_error * (time_diff);
		int16_t output = kP * curr_error + kI * cumulative_error + kD * (curr_error - last_error) / time_diff;
		last_error = curr_error;
		return output;
	}

	void setkP(float kP) {
		this->kP = kP;
	}

	void setkPI(float kPI) {
		this->kI = kI;
	}

	void setkD(float kD) {
		this->kD = kD;
	}

private:
	float kP=0;
	float kI = 0;
	float kD = 0;
	int16_t cumulative_error = 0;
	Timer::TimerInt last_trigger_time = 0;
	int16_t curr_error = 0;
	int16_t last_error = 0;
};

}



#endif /* INC_UTIL_PID_H_ */
