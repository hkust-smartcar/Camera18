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

bool bluetoothListener(const Byte* data, const size_t size) {
	if (data[0] == 255 && !tuned_var_coming) {
		received_ack = true;
		send_buffer.clear();
	} else if (data[0] == 254 && !tuned_var_coming) {
		tuned_var_coming = true;
		tuned_var_count = 0;
		tuned_byte_count = 0;
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

void InitBluetoothDebug() {
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
