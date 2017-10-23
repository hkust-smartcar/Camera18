/*
 * algo.h
 *
 *  Created on: Oct 12, 2017
 *      Author: LeeChunHei
 */

#ifndef INC_ALGO_H_
#define INC_ALGO_H_

#include <../inc/global.h>

#define cam_height 120
#define cam_width 160
#define point_check(x,y) ((buff[y * cam_width / 8 + x / 8] >> (7 - (x % 8))) & 1)

void algo();



#endif /* INC_ALGO_H_ */
