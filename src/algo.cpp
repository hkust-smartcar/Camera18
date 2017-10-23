/*
 * algo.cpp
 *
 *  Created on: Oct 12, 2017
 *      Author: LeeChunHei
 */

#include <../inc/algo.h>

void algo() {
	//Camera Init
	libsc::k60::Ov7725::Config config_cam;
	config_cam.contrast = contrast;
	config_cam.fps = libsc::k60::Ov7725::Config::Fps::kHigh;
	config_cam.id = 0;
	config_cam.h = 120;
	config_cam.w = 160;
	libsc::k60::Ov7725 cam_(config_cam);
	cam = &cam_;
	cam->Start();

	const uint8_t cam_mid = cam_width / 2;
	uint8_t left_edge[cam_height] = { };	//index 0 is the first edge point found
	uint8_t right_edge[cam_height] = { };	//index 0 is the first edge point found
	uint8_t left_edge_length = 0;
	uint8_t right_edge_length = 0;
	bool left_edge_completed = false;
	bool right_edge_completed = false;
	uint8_t path[cam_height] = { };			//index 0 is the first path point found
	uint8_t path_length = 0;
	coor left_corner;
	coor right_corner;
	bool found_left_corner = false;
	bool found_right_corner = false;
	libsc::Timer::TimerInt time = libsc::System::Time();
	while (1) {
		while (time != libsc::System::Time()) {
			time = libsc::System::Time();
			if (joystick->GetState() == libsc::Joystick::State::kSelect) {
				libutil::Menu menu(lcd, console, joystick, battery_meter, nullptr);
				menu.AddItem("Debug Mode", &debug, "on", "off", &menu.main_menu);
				menu.AddItem("contrast", &contrast, 1, &menu.main_menu);
				menu.EnterMenu(&menu.main_menu);
				cam->ChangeSecialDigitalEffect(0x00, contrast);
			}
			if (!time % 10) {
				left_edge_length = 0;
				right_edge_length = 0;
				left_edge_completed = false;
				right_edge_completed = false;
				path_length = 0;
				found_left_corner = false;
				found_right_corner = false;
				buff = cam->LockBuffer();
				for (uint8_t x = cam_mid; x >= 0; x--) {
					if (point_check(x, cam_height-5)) {
						left_edge[left_edge_length++] = x;
						break;
					}
				}
				for (uint8_t x = cam_mid; x < cam_width; x++) {
					if (point_check(x, cam_height-5)) {
						right_edge[right_edge_length++] = x;
						break;
					}
				}
				for (uint8_t y = cam_height - 6; y >= 0; y--) {
					int x;
					if (!left_edge_completed) {
						x = left_edge[left_edge_length - 1];
						if (point_check(x, y)) {
							for (x++; x < cam_width; x++) {
								if (!point_check(x, y)) {
									left_edge[left_edge_length++] = x - 1;
									break;
								}
								if (x == cam_width - 1) {
									left_edge_completed = true;
								}
							}
						} else {
							for (x--; x >= 0; x++) {
								if (point_check(x, y)) {
									left_edge[left_edge_length++] = x;
									break;
								}
								if (!x) {
									left_edge_completed = true;
								}
							}
						}
					}
					if (!right_edge_completed) {
						x = right_edge[right_edge_length - 1];
						if (point_check(x, y)) {
							for (x--; x >= 0; x--) {
								if (!point_check(x, y)) {
									right_edge[right_edge_length++] = x + 1;
									break;
								}
								if (!x) {
									right_edge_completed = true;
								}
							}
						} else {
							for (x++; x < cam_width; x++) {
								if (point_check(x, y)) {
									right_edge[right_edge_length++] = x;
									break;
								}
								if (x == cam_width - 1) {
									right_edge_completed = true;
								}
							}
						}	
					}
				}
				cam->UnlockBuffer();
			}
		}
	}
	cam->Stop();
}



