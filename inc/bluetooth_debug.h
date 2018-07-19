/*
 * bluetooth_debug.h
 *
 *  Created on: May 30, 2018
 *      Author: LeeChunHei
 */

#ifndef INC_BLUETOOTH_DEBUG_H_
#define INC_BLUETOOTH_DEBUG_H_

#include <global.h>

//Call this function before doing any following operation
void InitBluetoothDebug(bool* stop);

//Parameter format ("var_name\n", (Byte*)&var)
void PushTuneUint8(const char* name, Byte* var);
void PushTuneInt8(const char* name, Byte* var);
void PushTuneUInt16(const char* name, Byte* var);
void PushTuneInt16(const char* name, Byte* var);
void PushTuneUInt32(const char* name, Byte* var);
void PushTuneInt32(const char* name, Byte* var);
void PushTuneFloat(const char* name, Byte* var);
void PushTuneDouble(const char* name, Byte* var);

//Call this function after you had pushed all the variables that needed to be tune
void InitValueDebug();

//Send the camera related information out
bool SendData(std::vector<coor>& left_edge, std::vector<coor>& right_edge, int servo_angle, int mode, bool left_corner_valid, bool right_corner_valid, int car_speed, int left_edge_end_point, int right_edge_end_point, coor left_corner, coor right_corner);


#endif /* INC_BLUETOOTH_DEBUG_H_ */
