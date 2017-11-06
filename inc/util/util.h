/*
 * util.h
 *
 *  Created on: Jul 18, 2017
 *      Author: LeeChunHei
 */

#ifndef INC_UTIL_UTIL_H_
#define INC_UTIL_UTIL_H_

#include <global.h>

namespace util {

void battery_test();
void led_test();
void servo_tune();
void camera_tune();
bool camera_point_check(const Byte* buff, uint16_t x, uint16_t y);
void PrintLogo();
void StartAction();
void EndAction(bool is_landscape);

}



#endif /* INC_UTIL_UTIL_H_ */
