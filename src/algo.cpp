/*
 * algo.cpp
 *
 *  Created on: Oct 12, 2017
 *      Author: LeeChunHei
 */

#include <../inc/algo.h>

//SUSAN corner detection variable
#define window_size 7
#define corner_min 40
#define corner_max 60

//Motor Control variable
#define kP 0;
#define kI 0;
#define kD 0;

//SUSAN corner detection
bool susan_corner_detection(uint8_t x, uint8_t y) {
	if (x - window_size > 0 || x + window_size < cam_width || y - window_size > 0 || y + window_size < cam_height) {
		uint16_t count = 0;
		for (uint8_t point_y = y - window_size; point_y <= y + window_size; ++point_y) {
			for (uint8_t point_x = x - window_size; point_x <= x + window_size; ++point_x) {
				count += point_check(point_x, point_y);
			}
		}
		return count < corner_max && count > corner_min;
	}
	return false;
}
//End of SUSAN corner detection

//Main car program
void algo() {
	//Motor Control variable
	int speed = 0;
	int count = 0;
	int curr = 0;
	int last_error = 0;
	int error_sum = 0;
	int output = 0;

	const uint8_t cam_mid = cam_width / 2;
	uint8_t left_edge[cam_height] = { };	//index 0 is the first edge point found
	uint8_t right_edge[cam_height] = { };	//index 0 is the first edge point found
	uint8_t left_edge_length = 0;
	uint8_t right_edge_length = 0;
	bool left_edge_completed = false;
	bool right_edge_completed = false;
	uint8_t path[cam_height] = { };			//index 0 is the first path point found
	uint8_t path_length = 0;
	bool left_edge_turn_left = false;
	bool right_edge_turn_right = false;
	coor left_corner;
	coor right_corner;
	bool found_left_corner = false;
	bool found_right_corner = false;

	//Camera Init
	libsc::k60::Ov7725::Config config_cam;
	config_cam.contrast = contrast;
	config_cam.fps = libsc::k60::Ov7725::Config::Fps::kHigh;
	config_cam.id = 0;
	config_cam.h = cam_height;
	config_cam.w = cam_width;
	libsc::k60::Ov7725 cam_(config_cam);
	cam = &cam_;
	cam->Start();

	libsc::Timer::TimerInt time = libsc::System::Time();
	while (1) {
		while (time != libsc::System::Time()) {
			time = libsc::System::Time();

			//Joystick state checking for enter the menu
			if (joystick->GetState() == libsc::Joystick::State::kSelect) {
				libutil::Menu menu(true, lcd, console, joystick, battery_meter, nullptr);
				menu.AddItem("Debug Mode", &debug, "on", "off", &menu.main_menu);
				menu.AddItem("contrast", &contrast, 1, &menu.main_menu);
				menu.EnterMenu(&menu.main_menu);
				cam->ChangeSecialDigitalEffect(0x00, contrast);
			}
			//End of checking enter menu request

			//Cycle runs in every 10ms
			if (time % 10 == 0) {
				left_edge_length = 0;
				right_edge_length = 0;
				left_edge_completed = true;
				right_edge_completed = true;
				path_length = 0;
				found_left_corner = false;
				found_right_corner = false;
				buff = cam->LockBuffer();

				//Debug program
				if (debug) {
					lcd->SetRegion(libsc::Lcd::Rect(0, 0, cam_width, cam_height));
					lcd->FillBits(libsc::Lcd::kWhite, libsc::Lcd::kBlue, buff, cam_width * cam_height);
				}
				//End of debug program

				//Camera algorithm
				uint8_t start_y = cam_height - 5;	//the vertical coordinate to start with
				//For loop to find the proper left edge or right edge
				for (; start_y > 0 && !(left_edge_completed && right_edge_completed); --start_y) {
					left_edge_length = 0;
					right_edge_length = 0;
					for (uint8_t x = cam_mid; x > 0; x--) {
						if (point_check(x, start_y)) {
							left_edge[left_edge_length++] = x;
							left_edge_completed = false;
							break;
						}
					}

					if (left_edge_completed)
						left_edge[left_edge_length++] = 1;

					for (uint8_t x = cam_mid; x < cam_width; x++) {
						if (point_check(x, start_y)) {
							right_edge[right_edge_length++] = x;
							right_edge_completed = false;
							break;
						}
					}

					if (right_edge_completed)
						right_edge[right_edge_length++] = cam_width - 1;
				}
				//End of finding starting left edge or right edge

				//Find the first path point
				if (left_edge_completed) {
					int16_t path_pos = right_edge[right_edge_length - 1] - start_y / 2;
					if (path_pos > 0) {
						path[path_length++] = path_pos;
					} else {
						path[path_length++] = 0;
						break;
					}
				} else if (right_edge_completed) {
					int16_t path_pos = left_edge[left_edge_length - 1] + start_y / 2;
					if (path_pos > cam_width) {
						path[path_length++] = cam_width;
						break;
					} else {
						path[path_length++] = path_pos;
					}
				} else {
					path[path_length++] = (right_edge[right_edge_length - 1] + left_edge[left_edge_length - 1]) / 2;
				}
				//End of finding the first path point

				//Loop for finding edge and path
				for (uint8_t y = start_y; y > 0; y--) {
					//Find left edge
					int x = left_edge[left_edge_length - 1];
					if (!left_edge_completed && !found_left_corner) {
						if (point_check(x, y)) {
							//Find if there is left corner
							if (left_edge_turn_left) {
								found_left_corner = susan_corner_detection(left_edge[left_edge_length - 1], y + 1);
							}
							left_edge_turn_left = false;
							//Find left edge point
							for (x++; x < cam_width; x++) {
								if (!point_check(x, y)) {
									left_edge[left_edge_length++] = x - 1;
									break;
								}
								if (x == cam_width - 1) {
									left_edge[left_edge_length++] = x;
									left_edge_completed = true;
								}
							}
						} else {
							//Find if there is left corner
							if (!left_edge_turn_left) {
								found_right_corner = susan_corner_detection(left_edge[left_edge_length - 1], y + 1);
							}
							left_edge_turn_left = true;
							//Find left edge point
							for (x--; x > 0; x++) {
								if (point_check(x, y)) {
									left_edge[left_edge_length++] = x;
									break;
								}
								if (!x) {
									left_edge[left_edge_length++] = x + 1;
									left_edge_completed = true;
								}
							}
						}
					} else if (x == 1) {
						if (point_check(x, y)) {
							for (x++; x < cam_width; x++) {
								if (!point_check(x, y)) {
									left_edge[left_edge_length++] = x - 1;
									left_edge_completed = false;
									break;
								}
								if (x == cam_width - 1) {
									left_edge[left_edge_length++] = x;
									left_edge_completed = true;
								}
							}
						}
					} else {
						left_edge[left_edge_length++] = left_edge[left_edge_length - 1];
					}
					//End of finding left edge

					//Find right edge
					x = right_edge[right_edge_length - 1];
					if (!right_edge_completed && !found_right_corner) {
						if (point_check(x, y)) {
							//Find if there is right corner
							if (right_edge_turn_right) {
								found_right_corner = susan_corner_detection(right_edge[right_edge_length - 1], y + 1);
							}
							right_edge_turn_right = false;
							//Find right edge point
							for (x--; x > 0; x--) {
								if (!point_check(x, y)) {
									right_edge[right_edge_length++] = x + 1;
									break;
								}
								if (!x) {
									right_edge[right_edge_length++] = x + 1;
									right_edge_completed = true;
								}
							}
						} else {
							//Find if there is right corner
							if (!right_edge_turn_right) {
								found_right_corner = susan_corner_detection(right_edge[right_edge_length - 1], y + 1);
							}
							right_edge_turn_right = true;
							//Find right edge point
							for (x++; x < cam_width; x++) {
								if (point_check(x, y)) {
									right_edge[right_edge_length++] = x;
									break;
								}
								if (x == cam_width - 1) {
									right_edge[right_edge_length++] = x;
									right_edge_completed = true;
								}
							}
						}
					} else if (x == cam_width - 1) {
						if (point_check(x, y)) {
							for (x--; x > 0; x--) {
								if (!point_check(x, y)) {
									right_edge[right_edge_length++] = x + 1;
									right_edge_completed = false;
									break;
								}
								if (!x) {
									right_edge[right_edge_length++] = x + 1;
									right_edge_completed = true;
								}
							}
						}
					} else {
						right_edge[right_edge_length++] = right_edge[right_edge_length - 1];
					}
					//End of finding right edge

					//Find path point
					if (left_edge_completed && right_edge_completed) {

					} else if (left_edge_completed) {
						int16_t path_pos = right_edge[right_edge_length - 1] - y / 2;
						if (path_pos > 0) {
							path[path_length++] = path_pos;
						} else {
							path[path_length++] = 0;
							break;
						}
					} else if (right_edge_completed) {
						int16_t path_pos = left_edge[left_edge_length - 1] + y / 2;
						if (path_pos > cam_width) {
							path[path_length++] = cam_width;
							break;
						} else {
							path[path_length++] = path_pos;
						}
					} else {
						path[path_length++] = (right_edge[right_edge_length - 1] + left_edge[left_edge_length - 1]) / 2;
					}
					//End of finding path point
				}
				//End of loop to find edge and path

				//Debug program
				if (debug) {
					//Print the edges(yellow), path(green), corners(red)
					for (int y = start_y; y > path_length + start_y; --y) {
						lcd->SetRegion(libsc::Lcd::Rect(left_edge[left_edge_length - 1], y, 1, 1));
						lcd->FillColor(libsc::Lcd::kYellow);
						lcd->SetRegion(libsc::Lcd::Rect(right_edge[right_edge_length - 1], y, 1, 1));
						lcd->FillColor(libsc::Lcd::kYellow);
						lcd->SetRegion(libsc::Lcd::Rect(path[path_length - 1], y, 1, 1));
						lcd->FillColor(libsc::Lcd::kGreen);
					}
					lcd->SetRegion(libsc::Lcd::Rect(left_corner.x, left_corner.y, 2, 2));
					lcd->FillColor(libsc::Lcd::kRed);
					lcd->SetRegion(libsc::Lcd::Rect(right_corner.x, right_corner.y, 2, 2));
					lcd->FillColor(libsc::Lcd::kRed);
				}
				//End of debug program

				//End of camera algorithm

				//Speed control program
				encoder->Update();
				curr = encoder->GetCount();
				count += curr;
				curr -= speed;
				error_sum += curr;
//				output = kP * curr + kI * error_sum + kD * (curr - last_error);
				last_error = curr;
				if (output < 0) {
					motor->SetClockwise(false);
					motor->SetPower(-output);
				} else
					motor->SetPower(output);
				if (debug)
					motor->SetPower(0);
				//End of speed control program

				cam->UnlockBuffer();
			}
			//End of cycle

		}
	}
	cam->Stop();
}
//End of car program
