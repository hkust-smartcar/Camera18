/*
 * util.cpp
 *
 *  Created on: Jul 18, 2017
 *      Author: LeeChunHei
 */

#include <../inc/util/util.h>

namespace util {

void battery_test() {

	lcd->Clear();

	//Battery Check
	float voltage = 0;
	char buf[10] = {};
    do {
		voltage = battery_meter->GetVoltage();

		console->SetTextColor(voltage <= 7.4 ? Lcd::kRed : Lcd::kGreen);

		sprintf(buf, " Voltage: %.2fV", voltage);
		console->Clear(true);
		console->SetCursorRow(0);
		console->WriteString(buf);

		System::DelayMs(10);
	} while (voltage <= 7.4 && joystick->GetState() == Joystick::State::kIdle);
	System::DelayMs(400);
}

void led_test(){
	while (joystick->GetState() == Joystick::State::kIdle) {
		led1->SetEnable(false);
		led2->SetEnable(false);
		led3->SetEnable(false);
		led4->SetEnable(false);
		led1->Switch();
		System::DelayMs(500);
		led2->Switch();
		System::DelayMs(500);
		led3->Switch();
		System::DelayMs(500);
		led4->Switch();
		System::DelayMs(500);
		led1->SetEnable(true);
		led2->SetEnable(true);
		led3->SetEnable(true);
		led4->SetEnable(true);
		System::DelayMs(500);
		for (int i = 0; i < 100; i++) {
			led1->Switch();
			led2->Switch();
			led3->Switch();
			led4->Switch();
			System::DelayMs(50);
		}
	}
}

void servo_tune() {
	lcd->Clear();
	console->Clear(true);
	console->SetCursorRow(0);
	console->WriteString("Servo Angle:");
	char buf[5] = { };
	uint16_t angle = servo_center;
	servo->SetDegree(angle);
	console->SetCursorRow(1);
	sprintf(buf, "%d", angle);
	console->WriteString(buf);
	Timer::TimerInt last_trigger_time = System::Time();
	while (true) {
		if (joystick->GetState() == Joystick::State::kLeft) {
			angle--;
			servo->SetDegree(angle);
			sprintf(buf, "%d", angle);
			console->SetCursorRow(1);
			console->WriteString("              ");
			console->SetCursorRow(1);
			console->WriteString(buf);
			if (System::Time() - last_trigger_time < 210) {
				last_trigger_time = System::Time();
				System::DelayMs(20);
			} else {
				last_trigger_time = System::Time();
				System::DelayMs(200);
			}
		} else if (joystick->GetState() == Joystick::State::kRight) {
			angle++;
			servo->SetDegree(angle);
			sprintf(buf, "%d", angle);
			console->SetCursorRow(1);
			console->WriteString("              ");
			console->SetCursorRow(1);
			console->WriteString(buf);
			if (System::Time() - last_trigger_time < 210) {
				last_trigger_time = System::Time();
				System::DelayMs(20);
			} else {
				last_trigger_time = System::Time();
				System::DelayMs(200);
			}
		} else if (joystick->GetState() == Joystick::State::kSelect) {
			return;
		}
	}
}

void camera_tune() {
	cam->Start();
	while (true) {
		if (!System::Time() % 500) {

		}
	}
}

void PrintLogo() {
	for (uint8_t y = 0; y < 33; y++) {
		for (uint8_t x = 0; x < 120; x++) {
			lcd->SetRegion(libsc::Lcd::Rect(x + 3, y, 1, 1));
			switch (logo_data[y][x]) {
			case 0:
				lcd->FillColor(0x0000);
				break;
			case 1:
				lcd->FillColor(0x8000);
				break;
			case 7:
				lcd->FillColor(0x8410);
				break;
			case 8:
				lcd->FillColor(0xC618);
				break;
			case 9:
				lcd->FillColor(0xF800);
				break;
			case 15:
				lcd->FillColor(0xFFFF);
				break;
			default:
				break;
			}
		}
	}
}

}


