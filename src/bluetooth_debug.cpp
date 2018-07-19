/*
 * bluetooth_debug.cpp
 *
 *  Created on: May 30, 2018
 *      Author: LeeChunHei
 */

#include <bluetooth_debug.h>

bool received_ack;
Byte camera_data_start_byte = 255;
std::vector<Byte> send_buffer;
std::vector<uint8_t> var_type;
std::vector<Byte*> var_value;
std::vector<const char*> var_name;
bool tuned_var_coming;
int tuned_var_count;
int tuned_byte_count;
bool* car_stop;

bool bluetoothListener(const Byte* data, const size_t size) {
	if (data[0] == 255 && !tuned_var_coming) {
		received_ack = true;
		send_buffer.clear();
	} else if (data[0] == 254 && !tuned_var_coming) {
		tuned_var_coming = true;
		tuned_var_count = 0;
		tuned_byte_count = 0;
	} else if (data[0] == 0 && !tuned_var_coming) {
		motor->SetPower(0);
		*car_stop = !(*car_stop);
	} else if (tuned_var_coming) {
		*(var_value[tuned_var_count] + tuned_byte_count++) = data[0];
		switch (var_type[tuned_var_count]) {
		case 0:
			if (tuned_byte_count == 1) {
				tuned_var_count++;
				tuned_byte_count = 0;
			}
			break;
		case 1:
			if (tuned_byte_count == 1) {
				tuned_var_count++;
				tuned_byte_count = 0;
			}
			break;
		case 2:
			if (tuned_byte_count == 2) {
				tuned_var_count++;
				tuned_byte_count = 0;
			}
			break;
		case 3:
			if (tuned_byte_count == 2) {
				tuned_var_count++;
				tuned_byte_count = 0;
			}
			break;
		case 4:
			if (tuned_byte_count == 4) {
				tuned_var_count++;
				tuned_byte_count = 0;
			}
			break;
		case 5:
			if (tuned_byte_count == 4) {
				tuned_var_count++;
				tuned_byte_count = 0;
			}
			break;
		case 6:
			if (tuned_byte_count == 4) {
				tuned_var_count++;
				tuned_byte_count = 0;
			}
			break;
		case 7:
			if (tuned_byte_count == 4) {
				tuned_var_count++;
				tuned_byte_count = 0;
			}
			break;
		}
		if (tuned_var_count == var_type.size()) {
			tuned_var_coming = false;
		}
	}
}


void PushTuneUint8(const char* name, Byte* var) {
	var_type.push_back(0);
	var_value.push_back(var);
	var_name.push_back(name);
}

void PushTuneInt8(const char* name, Byte* var) {
	var_type.push_back(1);
	var_value.push_back(var);
	var_name.push_back(name);
}

void PushTuneUInt16(const char* name, Byte* var) {
	var_type.push_back(2);
	var_value.push_back(var);
	var_name.push_back(name);
}

void PushTuneInt16(const char* name, Byte* var) {
	var_type.push_back(3);
	var_value.push_back(var);
	var_name.push_back(name);
}

void PushTuneUInt32(const char* name, Byte* var) {
	var_type.push_back(4);
	var_value.push_back(var);
	var_name.push_back(name);
}

void PushTuneInt32(const char* name, Byte* var) {
	var_type.push_back(5);
	var_value.push_back(var);
	var_name.push_back(name);
}

void PushTuneFloat(const char* name, Byte* var) {
	var_type.push_back(6);
	var_value.push_back(var);
	var_name.push_back(name);
}

void PushTuneDouble(const char* name, Byte* var) {
	var_type.push_back(7);
	var_value.push_back(var);
	var_name.push_back(name);
}

void InitBluetoothDebug(bool* stop) {
	car_stop = stop;
	received_ack = true;
	send_buffer.clear();
	var_type.clear();
	var_value.clear();
	bt->SetRxIsr(&bluetoothListener);
}

void InitValueDebug() {
	tuned_var_coming = false;
	Byte start_byte = 254;
	bt->SendBuffer(&start_byte, 1);
	Byte size = var_type.size();
	bt->SendBuffer(&size, 1);
	for (int i = 0; i < var_type.size(); i++) {
		bt->SendBuffer(&var_type[i], 1);
	}
	for (int i = 0; i < var_type.size(); i++) {
		bt->SendStr(var_name[i]);
		switch (var_type[i]) {
		case 0:
			bt->SendBuffer(var_value[i], 1);
			break;
		case 1:
			bt->SendBuffer(var_value[i], 1);
			break;
		case 2:
			bt->SendBuffer(var_value[i], 2);
			break;
		case 3:
			bt->SendBuffer(var_value[i], 2);
			break;
		case 4:
			bt->SendBuffer(var_value[i], 4);
			break;
		case 5:
			bt->SendBuffer(var_value[i], 4);
			break;
		case 6:
			bt->SendBuffer(var_value[i], 4);
			break;
		case 7:
			bt->SendBuffer(var_value[i], 4);
			break;
		}
	}
	libsc::System::DelayMs(3000);
	var_name.clear();
}

bool SendData(std::vector<coor>& left_edge, std::vector<coor>& right_edge, int servo_angle, int mode, bool left_corner_valid, bool right_corner_valid, int car_speed, int left_edge_end_point, int right_edge_end_point, coor left_corner, coor right_corner) {
	if (!received_ack)
		return false;
	received_ack = false;
	bt->SendBuffer(&camera_data_start_byte, 1);
//	send_buffer.clear();
	send_buffer.push_back(6);
	send_buffer.push_back(left_edge.size());
	send_buffer.push_back(right_edge.size());
	send_buffer.push_back(servo_angle + 127);
	send_buffer.push_back(car_speed && 0b11111111);
	send_buffer.push_back(0 | (mode << 12) | (left_corner_valid << 3) | (right_corner_valid << 2) | (car_speed && 0b1100000000));
	if (left_edge.size()) {
		send_buffer.push_back(left_edge_end_point);
	}
	if (right_edge.size()) {
		send_buffer.push_back(right_edge_end_point);
	}
	if (left_edge.size()) {
		for (int i = 0; i < left_edge.size(); i++) {
			send_buffer.push_back(left_edge[i].x);
			send_buffer.push_back(left_edge[i].y);
		}
	}
	if (right_edge.size()) {
		for (int i = 0; i < right_edge.size(); i++) {
			send_buffer.push_back(right_edge[i].x);
			send_buffer.push_back(right_edge[i].y);
		}
	}
	if (left_corner_valid) {
		send_buffer.push_back(left_corner.x);
		send_buffer.push_back(left_corner.y);
	}
	if (right_corner_valid) {
		send_buffer.push_back(right_corner.x);
		send_buffer.push_back(right_corner.y);
	}
	bt->SendBuffer(send_buffer);
	return true;
}
