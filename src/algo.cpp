/*
 * algo.cpp
 *
 *  Created on: May 30, 2018
 *      Author: Amrutavarsh S Kinagi
 *      	LeeChunHei
 */

#include <algo.h>

struct coor {
	int16_t x;
	int16_t y;
};

const Byte* buffer;

bool debug = true;

//inline Byte GetPoint(uint8_t x, uint8_t y) {
//	return buffer[x + y * 189];
//}
//
//inline int16_t SobelEdgeDetection(uint8_t x, uint8_t y) {
//	return std::abs(-GetPoint((x) - 1, (y) - 1) - 2 * GetPoint((x) - 1, (y)) - GetPoint((x) - 1, (y) + 1) + GetPoint((x) + 1, (y) - 1) + 2 * GetPoint((x) + 1, (y)) + GetPoint((x) + 1, (y) + 1)) + std::abs(-GetPoint((x) - 1, (y) - 1) - 2 * GetPoint((x), (y) - 1) - GetPoint((x) + 1, (y) - 1) + GetPoint((x) - 1, (y) + 1) + 2 * GetPoint((x), (y) + 1) + GetPoint((x) + 1, (y) + 1));
//}

#define GetPoint(x, y) buffer[(x) + (y) * 189]
#define SobelEdgeDetection(x, y) std::abs(-GetPoint((x) - 1, (y)-1) - 2 * GetPoint((x) - 1, (y)) - GetPoint((x) - 1, (y)+1) + GetPoint((x) + 1, (y)-1) + 2 * GetPoint((x) + 1, (y)) + GetPoint((x) + 1, (y)+1)) + std::abs(-GetPoint((x) - 1, (y)-1) - 2 * GetPoint((x), (y)-1) - GetPoint((x) + 1, (y)-1) + GetPoint((x) - 1, (y)+1) + 2 * GetPoint((x), (y)+1) + GetPoint((x) + 1, (y)+1))

float search_distance = 400;
float search_m;
float search_c;
float search_slope;
//img2world[91][119] is the bottom mid point of the image, may need to tune later
float search_origin_x = img2world[97][119][0];
float search_origin_y = img2world[97][119][1];

coor left_end_point;
coor right_end_point;
bool left_end_point_found;
bool right_end_point_found;

inline void InitSearchDistance(float mpu_angle) {
	mpu_angle *= 0.00008726646;	//angle = angular_speed * 5ms * PI / 180
	search_m = -std::tan(mpu_angle);
	search_c = (search_distance * std::cos(mpu_angle) - search_m * (search_distance * std::sin(mpu_angle) - search_origin_x));
	left_end_point_found = false;
	right_end_point_found = false;
}

inline bool FindEndPoint(int x, int y) {
	return (img2world[x][y][1] - search_origin_y) > (search_m * (img2world[x][y][0] - search_origin_x) + search_c);
}

std::vector<coor> left_edge;
std::vector<coor> right_edge;
std::vector<int> left_edge_corner;
std::vector<int> right_edge_corner;
#define up 0
#define up_left 1
#define left 2
#define down_left 3
#define down 4
#define down_right 5
#define right 6
#define up_right 7
#define width 189
#define height 120
#define dist_threshold 202500
#define fast_threshold 8

int left_edge_prev_dir;
int right_edge_prev_dir;

bool FindLeftEdge(int& edge_prev_dir) {
	int size = left_edge.size();
	if (size > 100 || !size)
		return false;
	coor edge_coor = left_edge.back();
	int x = edge_coor.x;
	int y = edge_coor.y;
	if (x < 4 || x > 184 || y < 4 || y > 116)
		return false;
	if (!left_end_point_found && size % 5 == 0) {
		if (FindEndPoint(x, y)) {
			left_end_point_found = true;
			left_end_point= {x,y};
			if (debug) {
				lcd->SetRegion(libsc::Lcd::Rect(x - 1, y - 1, 3, 3));
				lcd->FillColor(libsc::Lcd::kBlue);
			}
		}
	}
	edge_prev_dir += 2;
	for (int i = 0; i < 5; i++) {
		edge_prev_dir = edge_prev_dir < 0 ? 8 + edge_prev_dir : edge_prev_dir > up_right ? edge_prev_dir - 8 : edge_prev_dir;
		switch (edge_prev_dir) {
		case up:
			if (SobelEdgeDetection(x, y - 1) < edge_threshold) {
				edge_coor.y = y - 1;
				left_edge.push_back(edge_coor);
				return true;
			}
			break;
		case up_left:
			if (SobelEdgeDetection(x - 1, y - 1) < edge_threshold) {
				edge_coor.x = x - 1;
				edge_coor.y = y - 1;
				left_edge.push_back(edge_coor);
				return true;
			}
			break;
		case left:
			if (SobelEdgeDetection(x - 1, y) < edge_threshold) {
				edge_coor.x = x - 1;
				left_edge.push_back(edge_coor);
				return true;
			}
			break;
		case down_left:
			if (SobelEdgeDetection(x - 1, y + 1) < edge_threshold) {
				edge_coor.x = x - 1;
				edge_coor.y = y + 1;
				left_edge.push_back(edge_coor);
				return true;
			}
			break;
		case down:
			if (SobelEdgeDetection(x, y + 1) < edge_threshold) {
				edge_coor.y = y + 1;
				left_edge.push_back(edge_coor);
				return true;
			}
			break;
		case down_right:
			if (SobelEdgeDetection(x + 1, y + 1) < edge_threshold) {
				edge_coor.x = x + 1;
				edge_coor.y = y + 1;
				left_edge.push_back(edge_coor);
				return true;
			}
			break;
		case right:
			if (SobelEdgeDetection(x + 1, y) < edge_threshold) {
				edge_coor.x = x + 1;
				left_edge.push_back(edge_coor);
				return true;
			}
			break;
		case up_right:
			if (SobelEdgeDetection(x + 1, y - 1) < edge_threshold) {
				edge_coor.x = x + 1;
				edge_coor.y = y - 1;
				left_edge.push_back(edge_coor);
				return true;
			}
			break;
		}
		edge_prev_dir--;
	}
	return false;
}

bool FindRightEdge(int& edge_prev_dir) {
	int size = right_edge.size();
	if (size > 100 || !size)
		return false;
	coor edge_coor = right_edge.back();
	int x = edge_coor.x;
	int y = edge_coor.y;
	if (x < 4 || x > 184 || y < 4 || y > 116)
		return false;
	if (!right_end_point_found && size % 5 == 0) {
		if (FindEndPoint(x, y)) {
			right_end_point_found = true;
			right_end_point= {x,y};
			if (debug) {
				lcd->SetRegion(libsc::Lcd::Rect(x - 1, y - 1, 3, 3));
				lcd->FillColor(libsc::Lcd::kBlue);
			}
		}
	}
	edge_prev_dir -= 2;
	for (int i = 0; i < 5; i++) {
		edge_prev_dir = edge_prev_dir < 0 ? 8 + edge_prev_dir : edge_prev_dir > up_right ? edge_prev_dir - 8 : edge_prev_dir;
		switch (edge_prev_dir) {
		case up:
			if (SobelEdgeDetection(x, y - 1) < edge_threshold) {
				edge_coor.y = y - 1;
				right_edge.push_back(edge_coor);
				return true;
			}
			break;
		case up_left:
			if (SobelEdgeDetection(x - 1, y - 1) < edge_threshold) {
				edge_coor.x = x - 1;
				edge_coor.y = y - 1;
				right_edge.push_back(edge_coor);
				return true;
			}
			break;
		case left:
			if (SobelEdgeDetection(x - 1, y) < edge_threshold) {
				edge_coor.x = x - 1;
				right_edge.push_back(edge_coor);
				return true;
			}
			break;
		case down_left:
			if (SobelEdgeDetection(x - 1, y + 1) < edge_threshold) {
				edge_coor.x = x - 1;
				edge_coor.y = y + 1;
				right_edge.push_back(edge_coor);
				return true;
			}
			break;
		case down:
			if (SobelEdgeDetection(x, y + 1) < edge_threshold) {
				edge_coor.y = y + 1;
				right_edge.push_back(edge_coor);
				return true;
			}
			break;
		case down_right:
			if (SobelEdgeDetection(x + 1, y + 1) < edge_threshold) {
				edge_coor.x = x + 1;
				edge_coor.y = y + 1;
				right_edge.push_back(edge_coor);
				return true;
			}
			break;
		case right:
			if (SobelEdgeDetection(x + 1, y) < edge_threshold) {
				edge_coor.x = x + 1;
				right_edge.push_back(edge_coor);
				return true;
			}
			break;
		case up_right:
			if (SobelEdgeDetection(x + 1, y - 1) < edge_threshold) {
				edge_coor.x = x + 1;
				edge_coor.y = y - 1;
				right_edge.push_back(edge_coor);
				return true;
			}
			break;
		}
		edge_prev_dir++;
	}
	return false;
}

bool FAST(int x, int y) {
	if (x < 4 || x > 184 || y < 4 || y > 116)
		return false;
	Byte center = GetPoint(x, y)- fast_threshold;
	Byte point;
	int temp = 0;
	point = GetPoint(x, y - 3);
	if (point < center) {
		temp++;
	}
	point = GetPoint(x + 1, y - 3);
	if (point < center) {
		temp++;
	}
	point = GetPoint(x + 2, y - 2);
	if (point < center) {
		temp++;
	}
	point = GetPoint(x + 3, y - 1);
	if (point < center) {
		temp++;
	}
	point = GetPoint(x + 3, y);
	if (point < center) {
		temp++;
	}
	point = GetPoint(x + 3, y + 1);
	if (point < center) {
		temp++;
	}
	point = GetPoint(x + 2, y + 2);
	if (point < center) {
		temp++;
	}
	point = GetPoint(x + 1, y + 3);
	if (point < center) {
		temp++;
	}
	point = GetPoint(x, y + 3);
	if (point < center) {
		temp++;
	}
	point = GetPoint(x - 1, y + 3);
	if (point < center) {
		temp++;
	}
	point = GetPoint(x - 2, y + 2);
	if (point < center) {
		temp++;
	}
	point = GetPoint(x - 3, y + 1);
	if (point < center) {
		temp++;
	}
	point = GetPoint(x - 3, y);
	if (point < center) {
		temp++;
	}
	point = GetPoint(x - 3, y - 1);
	if (point < center) {
		temp++;
	}
	point = GetPoint(x - 2, y - 2);
	if (point < center) {
		temp++;
	}
	point = GetPoint(x - 1, y - 3);
	if (point < center) {
		temp++;
	}
	if (temp < 5 && temp > 0)
		return true;
	return false;
}

bool dist_corners(coor m, coor n) {
	int x1 = img2world[m.x][m.y][0];
	int y1 = img2world[m.x][m.y][1];
	int x2 = img2world[n.x][n.y][0];
	int y2 = img2world[n.x][n.y][1];
	return ((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2) <= dist_threshold);
}

bool check_corner(coor pivot, coor m, coor n) {
	int c2 = (m.x - n.x) * (m.x - n.x) + (m.y - n.y) * (m.y - n.y);
	if (c2 > 170)
		return false;
	int a2 = (pivot.x - m.x) * (pivot.x - m.x) + (pivot.y - m.y) * (pivot.y - m.y);
	int b2 = (pivot.x - n.x) * (pivot.x - n.x) + (pivot.y - n.y) * (pivot.y - n.y);
	int pythagoras = a2 + b2 - c2;
	if (pythagoras >= 0)
		return true;
	float value = (float) (pythagoras * pythagoras) / (4 * a2 * b2);
	if (value < 0.07)
		return true;
	return false;
}

//bool check_corner(coor pivot, coor m, coor n) {
//	int a2 = (img2world[pivot.x][pivot.y][0] - img2world[m.x][m.y][0]) * (img2world[pivot.x][pivot.y][0] - img2world[m.x][m.y][0]) + (img2world[pivot.x][pivot.y][1] - img2world[m.x][m.y][1]) * (img2world[pivot.x][pivot.y][1] - img2world[m.x][m.y][1]);
////	a2/=100;
//	int b2 = (img2world[pivot.x][pivot.y][0] - img2world[n.x][n.y][0]) * (img2world[pivot.x][pivot.y][0] - img2world[n.x][n.y][0]) + (img2world[pivot.x][pivot.y][1] - img2world[n.x][n.y][1]) * (img2world[pivot.x][pivot.y][1] - img2world[n.x][n.y][1]);
////	b2/=100;
//	int c2 = (img2world[m.x][m.y][0] - img2world[n.x][n.y][0]) * (img2world[m.x][m.y][0] - img2world[n.x][n.y][0]) + (img2world[m.x][m.y][1] - img2world[n.x][n.y][1]) * (img2world[m.x][m.y][1] - img2world[n.x][n.y][1]);
////	c2/=100;
//	int pythagoras = a2 + b2 - c2;
//	if (pythagoras >= 0)
//		return true;
//	pythagoras = std::abs(pythagoras);
//	uint64_t pythagoras_uint = (uint64_t) pythagoras;
//	float value = ((((float) ((uint64_t) (pythagoras_uint * pythagoras_uint)) / 4) / a2) / b2);
////	float angle = std::acos(std::sqrt(value)) * 57.2957795;
//	if (value < 0.17)
//		return true;
//	return false;
//}

bool jump(coor point1, coor point2, coor& new_start) { //jump from the corner to new edge. returns true if a edge is found.
	float slope = (float) (point1.x - point2.x) / (point1.y - point2.y);
	float constant = point2.x - slope * point2.y;
	new_start.y = point2.y - 3;
	new_start.x = (int16_t) (new_start.y * slope + constant);
	while (new_start.x < width - 4 && new_start.x > 3 && new_start.y < height - 4 && new_start.y > 3 && SobelEdgeDetection(new_start.x, new_start.y) < edge_threshold) {
		new_start.y--;
		new_start.x = (new_start.y * slope + constant);
	}
	if (new_start.x < width - 4 && new_start.x > 3 && new_start.y < height - 4 && new_start.y > 3) {
		return true;
	}
	return false;
}

void algo() {
	motor->SetPower(250);
	motor->SetClockwise(false);
	libsc::System::DelayMs(100);
	libsc::Timer::TimerInt time_now = 0;
	camera->Start();
	coor midpoint = { 97, 115 };

	coor left_start = { 0, 0 };
	coor right_start = { 0, 0 };
	coor final_point = { 0, 0 };

	coor left_corner_coor;
	coor right_corner_coor;

	int left_corner_index;
	int right_corner_index;

	coor left_corner_end_coor;
	coor right_corner_end_coor;

	int mode = 0;	//0: normal 1: crossroad 2: left circle 3: right circle

	while (1) {
		if (camera->IsAvailable()) {
			buffer = camera->LockBuffer();
			InitSearchDistance(0);
			if (debug) {
				for (uint16_t i = 0; i < height; i++) {
					lcd->SetRegion(libsc::Lcd::Rect(0, i, 160, 1));
					lcd->FillGrayscalePixel(buffer + camera->GetW() * i, 160);
				}
			}
//			for (int y = 4; y < 120; y++) {
//				for (int x = 4; x < 184; x++) {
//					if (SobelEdgeDetection(x,y) > edge_threshold) {
//						lcd->SetRegion(libsc::Lcd::Rect(x, y, 1, 1));
//						lcd->FillColor(lcd->kRed);
//					}
//				}
//			}
//			camera->UnlockBuffer();
//			continue;
			time_now = libsc::System::Time();
			left_edge.clear();
			right_edge.clear();
			bool left_corner_found = false;
			bool right_corner_found = false;
			if (mode == 1) {
				if (debug) {
					lcd->SetRegion(libsc::Lcd::Rect(midpoint.x - 2, midpoint.y - 2, 5, 5));
					lcd->FillColor(lcd->kGreen);
				}
				for (; left_start.y > 4; left_start.y--) {
					for (int x = left_start.x; x > 4 && x > left_start.x - 20; x--) {
						if (SobelEdgeDetection(x,left_start.y) > edge_threshold) {
							left_start= {x,left_start.y};
							right_edge.push_back(left_start);
							break;
						}
					}
					if(right_edge.size()) {
						break;
					}
				}
				for (; right_start.y > 4; right_start.y--) {
					for (int x = right_start.x; x < 184 && x < right_start.x + 20; x++) {
						if (SobelEdgeDetection(x,right_start.y) > edge_threshold) {
							right_start= {x,right_start.y};
							left_edge.push_back(right_start);
							break;
						}
					}
					if(left_edge.size()) {
						break;
					}
				}
				int prev_left_dir = down;
				int prev_right_dir = down;
				while (FindRightEdge(prev_left_dir) && right_edge.size() < 10) {

				}
				left_start = right_edge.back();
				right_edge.clear();
				while (FindLeftEdge(prev_right_dir) && left_edge.size() < 10) {

				}
				right_start = left_edge.back();
				left_edge.clear();
				left_edge.push_back(left_start);
				right_edge.push_back(right_start);
				prev_left_dir = up;
				prev_right_dir = up;
				while (FindLeftEdge(prev_left_dir)) {
					if (left_edge.size() > 15 && left_edge[left_edge.size() - 6].y > 30 && check_corner(left_edge[left_edge.size() - 11], left_edge[left_edge.size() - 1], left_edge[left_edge.size() - 16])) {
						left_start = left_edge[left_edge.size() - 11];
						left_start.x += 7;
						if (debug) {
							lcd->SetRegion(libsc::Lcd::Rect(left_start.x - 2, left_start.y - 2, 5, 5));
							lcd->FillColor(lcd->kRed);
						}
						break;
					}
				}
				while (FindRightEdge(prev_right_dir)) {
					if (right_edge.size() > 15 && right_edge[right_edge.size() - 6].y > 30 && check_corner(right_edge[right_edge.size() - 11], right_edge[right_edge.size() - 1], right_edge[right_edge.size() - 16])) {
						right_start = right_edge[right_edge.size() - 11];
						right_start.x -= 7;
						if (debug) {
							lcd->SetRegion(libsc::Lcd::Rect(right_start.x - 2, right_start.y - 2, 5, 5));
							lcd->FillColor(lcd->kRed);
						}
						break;
					}
				}
				midpoint= {(left_start.x+right_start.x)/2,(left_start.y+right_start.y)/2};
				if (debug) {
					lcd->SetRegion(libsc::Lcd::Rect((left_start.x + right_start.x) / 2 - 2, (left_start.y + right_start.y) / 2 - 2, 5, 5));
					lcd->FillColor(lcd->kGreen);
				}
				if (midpoint.y > 114) {
					midpoint= {97,115};
					mode=0;
				} else {
					if (FindEndPoint(left_start.x, left_start.y) && FindEndPoint(right_start.x, right_start.y)) {
						left_end_point_found=true;
						right_end_point_found=true;
						left_end_point=midpoint;
						right_end_point=midpoint;
					}
				}
			} else if (mode == 0) {
				for (int x = midpoint.x; x > 4; x--) {
					if (SobelEdgeDetection(x, midpoint.y) > edge_threshold) {
						left_start= {x,midpoint.y};
						left_edge.push_back(left_start);
						break;
					}
				}
				for (int x = midpoint.x; x < 184; x++) {
					if (SobelEdgeDetection(x, midpoint.y) > edge_threshold) {
						right_start= {x,midpoint.y};
						right_edge.push_back(right_start);
						break;
					}
				}
				if (left_edge.size() && right_edge.size()) {
					midpoint.x = (left_start.x + right_start.x) / 2;
				}
			}
			left_edge_prev_dir = up;
			right_edge_prev_dir = up;
			float prev_corner_value;
			if (left_edge.size()) {
				while (FindLeftEdge(left_edge_prev_dir)) {
					if (debug) {
						lcd->SetRegion(libsc::Lcd::Rect(left_edge.back().x, left_edge.back().y, 1, 1));
						lcd->FillColor(lcd->kRed);
					}
					if (left_edge.size() > 15 && left_edge[left_edge.size() - 6].y > 30 && check_corner(left_edge[left_edge.size() - 11], left_edge[left_edge.size() - 1], left_edge[left_edge.size() - 16])) { //&& FAST(left_edge.back().x, left_edge.back().y)) {	//
//						while (FindLeftEdge(left_edge_prev_dir) && check_corner(left_edge[left_edge.size() - 11], left_edge[left_edge.size() - 1], left_edge[left_edge.size() - 16], corner_value)) {
//							if (corner_value < prev_corner_value) {
//								prev_corner_value = corner_value;
//								left_corner_coor = left_edge[left_edge.size() - 11];
//								left_corner_index = left_edge.size() - 11;
//							}
//						}
//						left_corner_end_coor = left_edge[left_edge.size() - 12];
						left_corner_coor = left_edge[left_edge.size() - 11];
						left_corner_found = true;
						if (debug) {
							lcd->SetRegion(libsc::Lcd::Rect(left_corner_coor.x - 2, left_corner_coor.y - 2, 5, 5));
							lcd->FillColor(lcd->kGreen);
						}
						break;
					}
				}
			}
			if (right_edge.size()) {
				while (FindRightEdge(right_edge_prev_dir)) {
					if (debug) {
						lcd->SetRegion(libsc::Lcd::Rect(right_edge.back().x, right_edge.back().y, 1, 1));
						lcd->FillColor(lcd->kRed);
					}
					if (right_edge.size() > 15 && right_edge[right_edge.size() - 6].y > 30 && check_corner(right_edge[right_edge.size() - 11], right_edge[right_edge.size() - 1], right_edge[right_edge.size() - 16])) {  //&& FAST(right_edge.back().x, right_edge.back().y)) {	//
//						while (FindRightEdge(right_edge_prev_dir) && check_corner(right_edge[right_edge.size() - 11], right_edge[right_edge.size() - 1], right_edge[right_edge.size() - 16], corner_value)) {
//							if (corner_value < prev_corner_value) {
//								prev_corner_value = corner_value;
//								right_corner_coor = right_edge[right_edge.size() - 11];
//								right_corner_index = right_edge.size() - 11;
//							}
//						}
//						right_corner_end_coor = right_edge[right_edge.size() - 12];
						right_corner_found = true;
						right_corner_coor = right_edge[right_edge.size() - 11];
						if (debug) {
							lcd->SetRegion(libsc::Lcd::Rect(right_corner_coor.x - 2, right_corner_coor.y - 2, 5, 5));
							lcd->FillColor(lcd->kGreen);
						}
						break;
					}
				}
			}
			if (mode == 0 && left_corner_found && right_corner_found && !FindEndPoint((left_corner_coor.x + right_corner_coor.x) / 2, (left_corner_coor.y + right_corner_coor.y) / 2)) {
				left_start = {0, 0};
				right_start = {0, 0};
				float l_slope = (float) (left_corner_coor.y - left_edge[left_edge.size() - 16].y) / (left_corner_coor.x - left_edge[left_edge.size() - 16].x);
				float r_slope = (float) (right_corner_coor.y - right_edge[right_edge.size() - 16].y) / (right_corner_coor.x - right_edge[right_edge.size() - 16].x);
				for (int y = left_corner_coor.y - 10; y > 30; y--) {
					int start_x = ((y - left_corner_coor.y) / l_slope) + left_corner_coor.x + 7;
					for (int x = start_x; x > start_x - 15 && x > 4; x--) {
						if (SobelEdgeDetection(x,y) > edge_threshold) {
							left_start= {start_x,y};
							break;
						}
						if(debug) {
							lcd->SetRegion(libsc::Lcd::Rect(x, y, 1, 1));
							lcd->FillColor(lcd->kBlue);
						}
					}
					if (left_start.x) {
						break;
					}
					left_start= {0,0};
				}
				for (int y = right_corner_coor.y - 10; y > 30; y--) {
					int start_x = ((y - right_corner_coor.y) / r_slope) + right_corner_coor.x - 7;
					for (int x = start_x; x < start_x + 15 && x < 184; x++) {
						if (SobelEdgeDetection(x,y) > edge_threshold) {
							right_start= {start_x,y};
							break;
						}
						if(debug) {
							lcd->SetRegion(libsc::Lcd::Rect(x, y, 1, 1));
							lcd->FillColor(lcd->kBlue);
						}
					}
					if (right_start.x) {
						break;
					}
					right_start= {0,0};
				}
				if (left_start.x && right_start.x) {
					midpoint= {(left_start.x+right_start.x)/2,(left_start.y+right_start.y)/2};
					left_end_point_found=true;
					right_end_point_found=true;
					left_end_point=midpoint;
					right_end_point=midpoint;
					mode = 1;
					if(debug) {
						lcd->SetRegion(libsc::Lcd::Rect((left_start.x + right_start.x) / 2 - 2, (left_start.y + right_start.y) / 2 - 2, 5, 5));
						lcd->FillColor(lcd->kGreen);
						lcd->SetRegion(libsc::Lcd::Rect(left_start.x - 2, left_start.y - 2, 5, 5));
						lcd->FillColor(lcd->kRed);
						lcd->SetRegion(libsc::Lcd::Rect(right_start.x - 2, right_start.y - 2, 5, 5));
						lcd->FillColor(lcd->kRed);
					}
				}
//				float slope = -(float) (left_corner_coor.x - right_corner_coor.x) / (left_corner_coor.y - right_corner_coor.y);
//				coor corner_midpoint = { (left_corner_coor.x + right_corner_coor.x) / 2, (left_corner_coor.y + right_corner_coor.y) / 2 };
//				for (int y = left_corner_coor.y < right_corner_coor.y ? left_corner_coor.y - 5 : right_corner_coor.y - 5; y > 30; y--) {
//					int start_x = ((y - corner_midpoint.y) / slope) + corner_midpoint.x;
//					if (SobelEdgeDetection(start_x,midpoint.y) > edge_threshold) {
//						if (GetPoint(start_x-3,midpoint.y)>GetPoint(start_x+3,midpoint.y)) {
//							start_x-=3;
//							for(int x=start_x;x>4;x--) {
//								if(SobelEdgeDetection(x,midpoint.y)<edge_threshold)
//								break;
//							}
//						} else {
//							start_x+=3;
//							for(int x=start_x;x<184;x++) {
//								if(SobelEdgeDetection(x,midpoint.y)<edge_threshold)
//								break;
//							}
//						}
//					}
//					for (int x = start_x; x > 30 && x > (start_x - (right_corner_coor.x + left_corner_coor.x) / 5); x--) {
//						if (SobelEdgeDetection(x,y) > edge_threshold) {
//							left_new= {x,y};
//						}
//					}
//					for (int x = start_x; x < 158 && x < (start_x + (right_corner_coor.x + left_corner_coor.x) / 5); x++) {
//						if (SobelEdgeDetection(x,y) > edge_threshold) {
//							right_new= {x,y};
//						}
//					}
//					if (left_new.x && right_new.x) {
////						midpoint= {(left_new.x + right_new.x) / 2,(left_new.y + right_new.y) / 2};
////						left_end_point_found=true;
////						right_end_point_found=true;
////						left_end_point=midpoint;
////						right_end_point=midpoint;
////						mode = 1;
//						if (debug) {
//							lcd->SetRegion(libsc::Lcd::Rect((left_new.x + right_new.x) / 2 - 2, (left_new.y + right_new.y) / 2 - 2, 5, 5));
//							lcd->FillColor(lcd->kGreen);
//						}
//						break;
//					}
//					left_new= {0,0};
//					right_new= {0,0};
//				}
//				coor left_new;
//				coor right_new;
//				coor left_point;
//				coor right_point;
//				if (left_corner_index > 10)
//					left_point = left_edge[left_corner_index - 10];
//				else
//					left_point = left_corner_coor;
//				if (right_corner_index > 10)
//					right_point = right_edge[right_corner_index - 10];
//				else
//					right_point = right_corner_coor;
//				if (jump(left_point, left_corner_end_coor, left_new) && jump(right_point, right_corner_end_coor, right_new)) {
//					float down_m = -1.0 * (left_corner_coor.x - right_corner_coor.x) / (left_corner_coor.y - right_corner_coor.y);
//					float up_m = -1.0 * (left_new.y - right_new.y) / (left_new.x - right_new.x);
//					float x = (down_m * ((float) (left_corner_coor.x + right_corner_coor.x) / 2) - (float) (left_corner_coor.y + right_corner_coor.y) / 2 + up_m * left_new.x + left_new.y) / (down_m + up_m);
//					float y = down_m * (x - (float) (left_corner_coor.x + right_corner_coor.x) / 2) + (float) (left_corner_coor.y + right_corner_coor.y) / 2;
//					if (debug) {
//						lcd->SetRegion(libsc::Lcd::Rect(left_new.x - 2, left_new.y - 2, 5, 5));
//						lcd->FillColor(lcd->kRed);
//						lcd->SetRegion(libsc::Lcd::Rect(right_new.x - 2, right_new.y - 2, 5, 5));
//						lcd->FillColor(lcd->kRed);
//						lcd->SetRegion(libsc::Lcd::Rect(x - 2, y - 2, 5, 5));
//						lcd->FillColor(lcd->kBlue);
//					}
//					if (x > 4 && x < 184 && y > 4 && y < 116) {
//						midpoint= {x,y};
//						left_end_point_found=true;
//						right_end_point_found=true;
//						left_end_point=midpoint;
//						right_end_point=midpoint;
//						mode = 1;
//						if(debug) {
//							lcd->SetRegion(libsc::Lcd::Rect(x - 2, y - 2, 5, 5));
//							lcd->FillColor(lcd->kGreen);
//						}
//					}
//				}
			} else if (left_corner_found) {

			} else if (right_corner_found) {

			} else {

			}

			//Control
			coor destination;
			int servo_angle, prev_servo_angle = servo->GetDegree();
			if (!mode || mode == 1) {
				if (left_end_point_found && right_end_point_found) {
					destination= {(left_end_point.x+right_end_point.x)/2,(left_end_point.y+right_end_point.y)/2};
					servo_angle = 1120 + std::atan(1.0*(img2world[destination.x][destination.y][0] - img2world[97][118][0]) / (img2world[destination.x][destination.y][1] - img2world[97][118][1])) * 1800 / 3.14;
					servo_angle += 0.4 * (servo_angle - prev_servo_angle);
				} else if (left_end_point_found) {
					destination=left_end_point;
					servo_angle = 1120 + std::atan(1.0*(img2world[destination.x][destination.y][0] - img2world[46][118][0]) / (img2world[destination.x][destination.y][1] - img2world[46][118][1])) * 1800 / 3.14;
					servo_angle += 0.4 * (servo_angle - prev_servo_angle);
				} else if (right_end_point_found) {
					destination=right_end_point;
					servo_angle = 1120 + std::atan(1.0*(img2world[destination.x][destination.y][0] - img2world[149][118][0]) / (img2world[destination.x][destination.y][1] - img2world[149][118][1])) * 1800 / 3.14;
					servo_angle += 0.4 * (servo_angle - prev_servo_angle);
					servo->SetDegree(servo_angle);
				} else if(left_corner_found&&right_corner_found) {
					destination= {(left_corner_coor.x+right_corner_coor.x)/2,(left_corner_coor.y+right_corner_coor.y)/2};
					servo_angle = 1120 + std::atan(1.0*(img2world[destination.x][destination.y][0] - img2world[97][118][0]) / (img2world[destination.x][destination.y][1] - img2world[97][118][1])) * 1800 / 3.14;
					servo_angle += 0.4 * (servo_angle - prev_servo_angle);
				} else if(left_corner_found) {
					destination=left_corner_coor;
					servo_angle = 1120 + std::atan(1.0*(img2world[destination.x][destination.y][0] - img2world[46][118][0]) / (img2world[destination.x][destination.y][1] - img2world[46][118][1])) * 1800 / 3.14;
					servo_angle += 0.4 * (servo_angle - prev_servo_angle);
				} else if(right_corner_found) {
					destination=right_corner_coor;
					servo_angle = 1120 + std::atan(1.0*(img2world[destination.x][destination.y][0] - img2world[149][118][0]) / (img2world[destination.x][destination.y][1] - img2world[149][118][1])) * 1800 / 3.14;
					servo_angle += 0.4 * (servo_angle - prev_servo_angle);
				} else if (left_edge.size() > right_edge.size()) {
					destination=left_edge.back();
					servo_angle = 1120 + std::atan(1.0*(img2world[destination.x][destination.y][0] - img2world[46][118][0]) / (img2world[destination.x][destination.y][1] - img2world[46][118][1])) * 1800 / 3.14;
					servo_angle += 0.4 * (servo_angle - prev_servo_angle);
				} else if(right_edge.size()) {
					destination=right_edge.back();
					servo_angle = 1120 + std::atan(1.0*(img2world[destination.x][destination.y][0] - img2world[149][118][0]) / (img2world[destination.x][destination.y][1] - img2world[149][118][1])) * 1800 / 3.14;
					servo_angle += 0.4 * (servo_angle - prev_servo_angle);
				}
			}
			servo->SetDegree(libutil::Clamp(700, servo_angle, 1500));
			time_now = libsc::System::Time() - time_now;
			camera->UnlockBuffer();
		}
		/*
		 if (camera->IsAvailable()) {
		 buffer = camera->LockBuffer(); //Use GetPoint(x,y) to get the gradient of the point
		 camera->UnlockBuffer();
		 }
		 for (uint16_t i = 0; i < height; i++) {
		 lcd->SetRegion(libsc::Lcd::Rect(0, i, 160, 1));
		 lcd->FillGrayscalePixel(buffer + camera->GetW() * i, 160);
		 }
		 motor->SetPower(200);
		 motor->SetClockwise(false);

		 InitSearchDistance(0);

		 empty_left();
		 empty_right();

		 coor leftmostP = { 188, 120 };
		 coor rightmostP = { 0, 120 };

		 if (track_state == Normal) {

		 left_edge_prev_dir = up;
		 right_edge_prev_dir = up;
		 if (left_start_point(midpoint, left_start, edge_threshold))
		 LeftEdge(left_start, left_edge_prev_dir, false);
		 if (right_start_point(midpoint, right_start, edge_threshold))
		 RightEdge(right_start, right_edge_prev_dir, false);

		 if (left_edge.size() && right_edge.size()) {
		 if (left_edge_corner.size() == 1 && right_edge_corner.size() == 1) {
		 if (dist_corners(left_edge[left_edge_corner[0]], right_edge[right_edge_corner[0]])) {
		 if ((right_edge[right_edge_corner[0]].y > 85) || left_edge[left_edge_corner[0]].y > 85)
		 track_state = Crossroad;
		 else {
		 final_point = {(left_edge[left_edge_corner[0]].x+right_edge[right_edge_corner[0]].x)/2,
		 (left_edge[left_edge_corner[0]].y+right_edge[right_edge_corner[0]].y)/2};
		 align = center_align;
		 midpoint= {(left_start.x+right_start.x)/2,115};
		 }
		 } else if (left_edge[left_edge_corner[0]].y > right_edge[right_edge_corner[0]].y) {
		 if(left_edge[left_edge_corner[0]].y>85)
		 normal_left_corner_fsm(track_state, final_point, midpoint, left_start, right_start);
		 else {
		 final_point = left_end_point.y>left_edge[left_edge_corner[0]].y ? left_edge[left_edge_corner[0]] : left_end_point;
		 align = left_align;
		 midpoint= {(left_start.x+right_start.x)/2,115};
		 }
		 } else if (left_edge[left_edge_corner[0]].y < right_edge[right_edge_corner[0]].y) {
		 if(right_edge[right_edge_corner[0]].y > 85)
		 normal_right_corner_fsm(track_state, final_point, midpoint, left_start, right_start);
		 else {
		 final_point = right_end_point.y>right_edge[right_edge_corner[0]].y ? right_edge[right_edge_corner[0]] : right_end_point;
		 align = right_align;
		 midpoint= {(left_start.x+right_start.x)/2,115};
		 }
		 }
		 } else if (left_edge_corner.size() == 1 && right_edge_corner.size() == 0) {
		 if(left_edge[left_edge_corner[0]].y>85)
		 normal_left_corner_fsm(track_state, final_point, midpoint, left_start, right_start);
		 else {
		 final_point = left_end_point.y>left_edge[left_edge_corner[0]].y ? left_edge[left_edge_corner[0]] : left_end_point;
		 align = left_align;
		 midpoint= {(left_start.x+right_start.x)/2,115};
		 }
		 } else if (left_edge_corner.size() == 0 && right_edge_corner.size() == 1) {
		 if(right_edge[right_edge_corner[0]].y > 85)
		 normal_right_corner_fsm(track_state, final_point, midpoint, left_start, right_start);
		 else {
		 final_point = right_end_point.y>right_edge[right_edge_corner[0]].y ? right_edge[right_edge_corner[0]] : right_end_point;
		 align = right_align;
		 midpoint= {(left_start.x+right_start.x)/2,115};
		 }
		 } else {
		 align = center_align;
		 final_point = {(left_end_point.x + right_end_point.x)/2,
		 (left_end_point.y + right_end_point.y)/2};
		 midpoint= {(left_start.x+right_start.x)/2,115};
		 }
		 }
		 else if(right_edge.size()) {
		 if(right_edge_corner.size() == 1) {
		 normal_right_corner_fsm(track_state,final_point,midpoint,left_start,right_start);
		 }
		 else if(right_edge_corner.size() == 0) {
		 align = right_align;
		 final_point = right_end_point;
		 midpoint= {(left_start.x+right_start.x)/2,115};
		 }
		 }
		 else if(left_edge.size()) {
		 if(left_edge_corner.size() == 1) {
		 normal_left_corner_fsm(track_state,final_point,midpoint,left_start,right_start);
		 }
		 else if(left_edge_corner.size() == 0) {
		 align = left_align;
		 final_point = left_end_point;
		 midpoint= {(left_start.x+right_start.x)/2,115};
		 }
		 }
		 }
		 if (track_state == Crossroad) {
		 if (crossroad_state == Detected) {
		 if (prev_track_state != Normal) {
		 left_edge_prev_dir = up;
		 right_edge_prev_dir = up;
		 if (left_start_point(midpoint, left_start, edge_threshold))
		 LeftEdge(left_start, left_edge_prev_dir, false);
		 if (right_start_point(midpoint, right_start, edge_threshold))
		 RightEdge(right_start, right_edge_prev_dir, false);
		 }
		 if (left_edge.size() && right_edge.size()) {
		 if (left_edge[left_edge_corner[0]].y > 85 || right_edge[right_edge_corner[0]].y > 85) {
		 crossroad_state = Inside;
		 coor point1;
		 if (right_edge_corner[0] > 10)
		 point1 = right_edge[right_edge_corner[0] - 10];
		 else
		 point1 = right_edge[0];
		 coor new_start = { 0, 0 };
		 jump(point1, right_edge[right_edge_corner[0] + 2], new_start, edge_threshold, true);
		 right_edge_prev_dir = right;
		 right_edge.push_back(new_start);
		 FindJumpStart(right_edge_prev_dir);
		 RightEdge(right_edge[right_edge.size() - 1], right_edge_prev_dir, true);

		 if (left_edge_corner[0] > 10)
		 point1 = left_edge[left_edge_corner[0] - 10];
		 else
		 point1 = left_edge[0];
		 jump(point1, left_edge[left_edge_corner[0] + 2], new_start, edge_threshold, false);
		 left_edge_prev_dir = left;
		 left_edge.push_back(new_start);
		 FindJumpStart(left_edge_prev_dir);
		 LeftEdge(left_edge[left_edge.size() - 1], left_edge_prev_dir, true);
		 if (left_edge_corner.size() == 2 && right_edge_corner.size() == 2) {
		 align = center_align;
		 final_point= {(left_edge[left_edge_corner[1]].x + right_edge[right_edge_corner[1]].x) / 2,
		 (left_edge[left_edge_corner[1]].y + right_edge[right_edge_corner[1]].y) / 2};
		 midpoint= {(right_edge[right_edge.size()-1].x + left_edge[left_edge.size()-1].x)/2,
		 (right_edge[right_edge.size()-1].y + left_edge[left_edge.size()-1].x)/2};
		 }
		 else if(left_edge_corner.size() == 2) {
		 final_point = left_edge[left_edge_corner[1]];
		 align = left_align;
		 midpoint= {left_edge[left_edge.size() - 1].x + 25, left_edge[left_edge.size() - 1].y};
		 }
		 else if(right_edge_corner.size() == 2) {
		 final_point = right_edge[right_edge_corner[1]];
		 align = right_align;
		 midpoint= {right_edge[right_edge.size()-1].x - 10,right_edge[right_edge.size()-1].y};
		 }
		 } else {
		 align = center_align;
		 crossroad_state = Detected;
		 final_point = {(left_edge[left_edge_corner[0]].x + right_edge[right_edge_corner[0]].x) / 2,
		 (left_edge[left_edge_corner[0]].y + right_edge[right_edge_corner[0]].y) / 2};
		 midpoint= {(left_start.x+right_start.x)/2,115};
		 }
		 }
		 else if(left_edge.size()) {
		 align = left_align;
		 if (left_edge[left_edge_corner[0]].y > 85) {
		 crossroad_state = Inside;
		 if (prev_track_state != Normal) {
		 coor point1;
		 if (left_edge_corner[0] > 10)
		 point1 = left_edge[left_edge_corner[0] - 10];
		 else
		 point1 = left_edge[0];
		 coor new_start = {0, 0};
		 jump(point1, left_edge[left_edge_corner[0] + 2], new_start, edge_threshold, false);
		 left_edge_prev_dir = left;
		 left_edge.push_back(new_start);
		 FindJumpStart(left_edge_prev_dir);
		 LeftEdge(left_edge[left_edge.size()-1], left_edge_prev_dir, true);
		 }
		 final_point = left_edge[left_edge_corner[1]];
		 midpoint= {left_edge[left_edge.size() - 1].x + 10, left_edge[left_edge.size() - 1].y};
		 }
		 else {
		 crossroad_state = Detected;
		 final_point = left_edge[left_edge_corner[0]];
		 midpoint= {(left_start.x+right_start.x)/2,115};
		 }
		 }
		 else if(right_edge.size()) {
		 align = right_align;
		 if (right_edge[right_edge_corner[0]].y > 85) {
		 crossroad_state = Inside;
		 if(prev_track_state != Normal) {
		 coor point1;
		 if (right_edge_corner[0] > 10)
		 point1 = right_edge[right_edge_corner[0] - 10];
		 else
		 point1 = right_edge[0];
		 coor new_start = {0, 0};
		 jump(point1, right_edge[right_edge_corner[0] + 2], new_start, edge_threshold, true);
		 right_edge_prev_dir = right;
		 right_edge.push_back(new_start);
		 FindJumpStart(right_edge_prev_dir);
		 RightEdge(right_edge[right_edge.size()-1], right_edge_prev_dir, true);
		 }
		 final_point = right_edge[right_edge_corner[1]];
		 midpoint= {right_edge[right_edge.size()-1].x - 10,right_edge[right_edge.size()-1].y};
		 }
		 else {
		 crossroad_state = Detected;
		 final_point = right_edge[right_edge_corner[0]];
		 midpoint= {(left_start.x+right_start.x)/2,115};
		 }
		 }
		 }
		 else if(crossroad_state == Inside) {
		 left_edge_prev_dir = down;
		 right_edge_prev_dir = down;
		 left_jump = true;
		 right_jump = true;
		 if(left_start_point(midpoint,left_start,edge_threshold))
		 RightEdge(left_start,right_edge_prev_dir,false);
		 if(right_start_point(midpoint,right_start,edge_threshold))
		 LeftEdge(right_start,left_edge_prev_dir,false);

		 if(left_edge[left_edge.size()-1].y<113 && right_edge[right_edge.size()-1].y<113) {
		 if(left_edge_corner.size() == 1 && right_edge_corner.size() == 1) {
		 align = center_align;
		 final_point = {(right_edge[right_edge_corner[0]].x+left_edge[left_edge_corner[0]].x)/2,
		 (right_edge[right_edge_corner[0]].y+left_edge[left_edge_corner[0]].y)/2};
		 midpoint = {final_point.x,final_point.y-10};
		 }
		 else if(left_edge_corner.size() == 1) {
		 align = right_align;
		 final_point = left_edge[left_edge_corner[0]];
		 midpoint = {final_point.x-25,final_point.y-10};
		 }
		 else if(right_edge_corner.size() == 1) {
		 align = left_align;
		 final_point = right_edge[right_edge_corner[0]];
		 midpoint = {final_point.x+25,final_point.y-10};
		 }
		 else {
		 align = center_align;
		 final_point = midpoint;
		 }
		 }
		 else {
		 left_jump = false;
		 right_jump = false;
		 track_state = Normal;
		 crossroad_state = Detected;
		 left_edge_prev_dir = up;
		 right_edge_prev_dir = up;
		 LeftEdge(left_start,left_edge_prev_dir,false);
		 RightEdge(right_start,right_edge_prev_dir,false);
		 if(left_edge.size() && right_edge.size()) {
		 align=center_align;
		 final_point = {(left_end_point.x + right_end_point.x)/2,
		 (left_end_point.y + right_end_point.y)/2};
		 }
		 else if(left_edge.size()) {
		 align = left_align;
		 final_point = left_end_point;
		 }
		 else if(right_edge.size()) {
		 align = right_align;
		 final_point = right_end_point;
		 }
		 midpoint= {(left_start.x+right_start.x)/2,115};
		 }
		 }

		 }
		 if (track_state == RightLoop) {
		 //follow the right edge when Entering
		 //right align
		 if (loop_state == Entering) {
		 if (prev_track_state == Normal) {
		 for (int i = right_edge.size() - 1; i > right_edge_corner[0] + 10; i--) {

		 if (right_edge[i].x < leftmostP.x)
		 leftmostP = right_edge[i];
		 }
		 final_point = {leftmostP.x,leftmostP.y};
		 align = right_align;
		 midpoint = {leftmostP.x-10,leftmostP.y};

		 //					right_edge_prev_dir = up;
		 //					if (right_start_point(midpoint, right_start, edge_threshold))
		 //						RightEdge(right_start, right_edge_prev_dir, edge_threshold, false);

		 //set control
		 right_end_point = leftmostP;
		 right_end_point_found = true;
		 align = right_align;
		 } else if (prev_track_state == RightLoop) {
		 left_edge_prev_dir = down;
		 right_edge_prev_dir = down;
		 if (left_start_point(midpoint, left_start, edge_threshold)) {
		 //as the direction is down, use rightedge function
		 //and the left edge will be stored in right_edge
		 RightEdge(left_start, left_edge_prev_dir, false);
		 //LeftEdge(left_start, left_edge_prev_dir, false);
		 }
		 if (right_start_point(midpoint, right_start, edge_threshold)) {
		 //opposite as well
		 LeftLoopEdgeR(right_start, right_edge_prev_dir, leftmostP, false);
		 //	RightLoopEdgeR(right_start, right_edge_prev_dir, leftmostP, false);
		 }
		 midpoint = {leftmostP.x-10,leftmostP.y};
		 align = right_align;
		 final_point = {leftmostP.x,leftmostP.y};

		 //					left_edge = right_edge;
		 //					left_edge_corner = right_edge_corner;

		 //					right_edge_prev_dir = up;
		 //					if (right_start_point(midpoint, right_start, edge_threshold))
		 //						RightEdge(right_start, right_edge_prev_dir,  false);
		 //					if(right_edge_corner.size()==1){
		 //						char buffer[50];
		 //						sprintf(buffer, "%d", right_edge[right_edge_corner[0]].y);
		 //						lcd->SetRegion(libsc::Lcd::Rect(0, 80, 160, 40));
		 //						writerP->WriteString(buffer);
		 //					}

		 if (right_edge_corner.size() == 1 && right_edge[right_edge_corner[0]].y>80) {
		 loop_state = In;
		 midpoint = {(left_start.x+right_start.x)/2,100};
		 }
		 else {
		 if(leftmostP.y>90) {
		 midpoint = {midpoint.x-10,90};
		 right_edge_prev_dir = up;
		 if (right_start_point(midpoint, right_start, edge_threshold))
		 RightEdge(right_start, right_edge_prev_dir, false);

		 //set control
		 //if the bottom exist, then we find the farthest point as end point
		 if(right_edge.size()>0) {
		 right_end_point = right_edge[right_edge.size()-1];
		 right_end_point_found = true;
		 }
		 else {
		 right_end_point_found = false;
		 }
		 align = right_align;
		 }
		 else {
		 //set control
		 //if the bottom still not exist, then the car is just entering the loop
		 //then we find the leftmostP on the inner loop
		 right_end_point = leftmostP;
		 right_end_point_found = true;
		 align = right_align;
		 }
		 }

		 }
		 }
		 //follow the normal way when In
		 else if(loop_state == In) {
		 left_edge_prev_dir = up;
		 right_edge_prev_dir = up;
		 if(left_start_point(midpoint,left_start,edge_threshold))
		 LeftEdge(left_start,left_edge_prev_dir,false);
		 if(right_start_point(midpoint,right_start,edge_threshold))
		 RightEdge(right_start,right_edge_prev_dir,false);
		 midpoint = {(left_start.x+right_start.x)/2,(left_start.y+right_start.y)/2};
		 if(left_edge.size() && right_edge.size())
		 align = center_align;
		 else if(left_edge.size())
		 align = left_align;
		 else if(right_edge.size())
		 align = right_align;
		 //&& left_edge[left_edge_corner[0]].y>100
		 if(left_edge_corner.size() == 1 && right_edge_corner.size() == 0  && left_edge[left_edge_corner[0]].y>90) {
		 loop_state = Leaving;
		 }

		 //set control
		 //TODO
		 //as normal way
		 }
		 //follow the right edges when Leaving
		 else if(loop_state == Leaving) {
		 midpoint = {(left_start.x+right_start.x)/2,90};
		 left_edge_prev_dir = up;
		 right_edge_prev_dir = up;
		 if(left_start_point(midpoint,left_start,edge_threshold))
		 LeftEdge(left_start,left_edge_prev_dir,false);
		 if(right_start_point(midpoint,right_start,edge_threshold))
		 RightEdge(right_start,right_edge_prev_dir,false);
		 midpoint = {(left_start.x+right_start.x)/2,90};
		 char buffer[50];
		 sprintf(buffer, "%d", left_edge.size());
		 lcd->SetRegion(libsc::Lcd::Rect(0, 80, 160, 40));
		 writerP->WriteString(buffer);
		 if(left_edge.size()>110 && left_edge_corner.size()==0) {
		 loop_state = Finished;
		 }
		 else {
		 //set control
		 right_end_point_found = true;
		 if(right_edge.size()>0)
		 right_end_point = right_edge[right_edge.size()-1];
		 else
		 right_end_point_found = false;//follow previous direction
		 align = right_align;
		 }
		 }
		 //follow the left edges when Finished
		 else if(loop_state == Finished) {
		 //increase the midpoint height for detecting the corner?
		 midpoint = {midpoint.x,90};
		 left_edge_prev_dir = up;
		 right_edge_prev_dir = down;
		 std::vector<coor> temp_edge;
		 std::vector<int> temp_edge_corner;
		 if(left_start_point(midpoint,left_start,edge_threshold)) {
		 LeftEdge(left_start,left_edge_prev_dir,false);
		 temp_edge = left_edge;
		 temp_edge_corner = left_edge_corner;
		 }
		 if(right_start_point(midpoint,right_start,edge_threshold))
		 LeftEdge(right_start,right_edge_prev_dir,false);
		 else{

		 left_edge_corner.clear();
		 left_edge.clear();
		 }

		 right_edge = left_edge;
		 right_edge_corner = left_edge_corner;
		 left_edge = temp_edge;
		 left_edge_corner = temp_edge_corner;
		 midpoint = {(left_start.x+right_start.x)/2,(left_start.y+right_start.y)/2};
		 //				char buffer[50];
		 //				sprintf(buffer, "%d %d", right_edge_corner.size(),left_edge_corner.size());
		 //				lcd->SetRegion(libsc::Lcd::Rect(0, 80, 160, 40));
		 //				writerP->WriteString(buffer);
		 if(right_edge_corner.size()==1) {
		 track_state = Normal;
		 loop_state = Entering;
		 midpoint = {midpoint.x,115};
		 }
		 else {
		 //set control
		 left_end_point_found = true;
		 if(left_edge.size()>11)
		 left_end_point = left_edge[left_edge.size()-11];
		 else if(left_edge.size()>0)
		 left_end_point = left_edge[left_edge.size()-1];
		 else
		 left_end_point_found = false;
		 align = left_align;

		 }
		 }
		 }
		 if (track_state == LeftLoop) {
		 //follow the right edge when Entering
		 if (loop_state == Entering) {
		 if (prev_track_state == Normal) {
		 for (int i = 0; i < left_edge.size(); i++) {
		 if (left_edge[i].x < rightmostP.x)
		 rightmostP = left_edge[i];
		 }
		 midpoint = {rightmostP.x-10,rightmostP.y};
		 left_edge_prev_dir = up;
		 if (left_start_point(midpoint, left_start, edge_threshold))
		 LeftEdge(left_start, left_edge_prev_dir, false);
		 } else if (prev_track_state == LeftLoop) {
		 right_edge_prev_dir = down;
		 left_edge_prev_dir = down_left;
		 if (right_start_point(midpoint, right_start, edge_threshold)) {
		 //as the direction is down, use rightedge function
		 //and the left edge will be stored in right_edge
		 LeftEdge(right_start, right_edge_prev_dir, false);
		 }
		 if (left_start_point(midpoint, left_start, edge_threshold)) {
		 //opposite as well
		 RightLoopEdgeL(right_start, right_edge_prev_dir, rightmostP, true);
		 }
		 midpoint = {rightmostP.x-10,rightmostP.y};

		 right_edge = left_edge;
		 right_edge_corner = left_edge_corner;

		 left_edge_prev_dir = up;
		 if (left_start_point(midpoint, left_start, edge_threshold))
		 LeftEdge(left_start, left_edge_prev_dir, false);

		 if (right_edge_corner.size() == 1) {
		 loop_state = In;
		 midpoint = {(left_start.x+right_start.x)/2,115};
		 }
		 }
		 }
		 //follow the normal way when In
		 else if(loop_state == In) {
		 left_edge_prev_dir = up;
		 right_edge_prev_dir = up;
		 if(left_start_point(midpoint,left_start,edge_threshold))
		 LeftEdge(left_start,left_edge_prev_dir,false);
		 if(right_start_point(midpoint,right_start,edge_threshold))
		 RightEdge(right_start,right_edge_prev_dir,false);
		 midpoint = {(left_start.x+right_start.x)/2,(left_start.y+right_start.y)/2};
		 if(right_edge_corner.size() == 1 && left_edge_corner.size() == 0) {
		 loop_state = Leaving;
		 }
		 }
		 //follow the right edges when Leaving
		 else if(loop_state == Leaving) {
		 left_edge_prev_dir = up;
		 right_edge_prev_dir = up;
		 if(left_start_point(midpoint,left_start,edge_threshold))
		 LeftEdge(left_start,left_edge_prev_dir,false);
		 if(right_start_point(midpoint,right_start,edge_threshold))
		 RightEdge(right_start,right_edge_prev_dir,false);
		 midpoint = {(left_start.x+right_start.x)/2,(left_start.y+right_start.y)/2};
		 if(right_edge.size()>0 && right_edge_corner.size()==0) {
		 loop_state = Finished;
		 }
		 }
		 //follow the left edges when Finished
		 else if(loop_state == Finished) {
		 //increase the midpoint height for detecting the corner?
		 midpoint = {midpoint.x,100};
		 right_edge_prev_dir = up;
		 left_edge_prev_dir = down;
		 std::vector<coor> temp_edge;
		 std::vector<int> temp_edge_corner;
		 if(right_start_point(midpoint,right_start,edge_threshold)) {
		 RightEdge(right_start,right_edge_prev_dir,false);
		 temp_edge = right_edge;
		 temp_edge_corner = right_edge_corner;
		 }
		 if(left_start_point(midpoint,left_start,edge_threshold))
		 RightEdge(left_start,left_edge_prev_dir,false);
		 left_edge = right_edge;
		 left_edge_corner = right_edge_corner;
		 right_edge = temp_edge;
		 right_edge_corner = temp_edge_corner;
		 midpoint = {(left_start.x+right_start.x)/2,(left_start.y+right_start.y)/2};
		 if(left_edge_corner.size()==1) {
		 track_state = Normal;
		 loop_state = Entering;
		 }
		 }
		 }

		 for (int i = 0; i < left_edge.size(); i++) {

		 lcd->SetRegion(libsc::St7735r::Lcd::Rect(left_edge[i].x, left_edge[i].y, 1, 1));
		 lcd->FillColor(lcd->kPurple);
		 }
		 for (int i = 0; i < right_edge.size(); i++) {
		 lcd->SetRegion(libsc::St7735r::Lcd::Rect(right_edge[i].x, right_edge[i].y, 1, 1));
		 lcd->FillColor(lcd->kRed);
		 }

		 for (int i = 0; i < left_edge_corner.size(); i++) {
		 lcd->SetRegion(libsc::St7735r::Lcd::Rect(left_edge[left_edge_corner[i]].x, left_edge[left_edge_corner[i]].y, 5, 5));
		 lcd->FillColor(lcd->kPurple);
		 }
		 for (int i = 0; i < right_edge_corner.size(); i++) {
		 lcd->SetRegion(libsc::St7735r::Lcd::Rect(right_edge[right_edge_corner[i]].x, right_edge[right_edge_corner[i]].y, 5, 5));
		 lcd->FillColor(lcd->kRed);
		 }

		 lcd->SetRegion(libsc::St7735r::Lcd::Rect(midpoint.x, midpoint.y, 4, 4));
		 lcd->FillColor(lcd->kBlue);
		 char buffer[50];
		 sprintf(buffer, "t %d l %d", track_state, loop_state);
		 lcd->SetRegion(libsc::Lcd::Rect(0, 60, 160, 40));
		 writerP->WriteString(buffer);

		 //			lcd->Clear();
		 //			if (left_end_point_found) {
		 //				lcd->SetRegion(libsc::St7735r::Rect(left_end_point.x, left_end_point.y, 1, 1));
		 //				lcd->FillColor(lcd->kGreen);
		 //			}
		 //			if (right_end_point_found) {
		 //				lcd->SetRegion(libsc::St7735r::Rect(right_end_point.x, right_end_point.y, 1, 1));
		 //				lcd->FillColor(lcd->kRed);
		 //			}
		 int prev_servo_angle = servo->GetDegree();
		 int servo_angle;
		 coor destination;
		 if (track_state == Tstate::Normal || (track_state == Tstate::LeftLoop && loop_state == Lstate::In) || (track_state == Tstate::RightLoop && loop_state == Lstate::In)) {
		 switch (align) {
		 case 0:
		 if (left_edge.size()) {
		 if (left_end_point_found) {
		 destination = left_end_point;
		 } else {
		 destination = left_edge.back();
		 }
		 servo_angle = 1120 + std::atan(1.0 * (img2world[destination.x][destination.y][0] - img2world[46][118][0]) / (img2world[destination.x][destination.y][1] - img2world[46][118][1])) * 1800 / 3.14;
		 servo_angle += 0.4 * (servo_angle - prev_servo_angle);
		 servo->SetDegree(servo_angle);
		 }
		 break;
		 case 1:
		 if (left_end_point_found && right_end_point_found) {
		 destination= {(left_end_point.x+right_end_point.x)/2,(left_end_point.y+right_end_point.y)/2};
		 servo_angle = 1120 + std::atan(1.0*(img2world[destination.x][destination.y][0] - img2world[97][118][0]) / (img2world[destination.x][destination.y][1] - img2world[97][118][1])) * 1800 / 3.14;
		 servo_angle += 0.4 * (servo_angle - prev_servo_angle);
		 servo->SetDegree(servo_angle);
		 } else if (left_end_point_found) {
		 destination=left_end_point;
		 servo_angle = 1120 + std::atan(1.0*(img2world[destination.x][destination.y][0] - img2world[46][118][0]) / (img2world[destination.x][destination.y][1] - img2world[46][118][1])) * 1800 / 3.14;
		 servo_angle += 0.4 * (servo_angle - prev_servo_angle);
		 servo->SetDegree(servo_angle);
		 } else if (right_end_point_found) {
		 destination=right_end_point;
		 servo_angle = 1120 + std::atan(1.0*(img2world[destination.x][destination.y][0] - img2world[149][118][0]) / (img2world[destination.x][destination.y][1] - img2world[149][118][1])) * 1800 / 3.14;
		 servo_angle += 0.4 * (servo_angle - prev_servo_angle);
		 servo->SetDegree(servo_angle);
		 } else if(left_edge_corner.size()||right_edge_corner.size()) {
		 //					lcd->SetRegion(libsc::Lcd::Rect(final_point.x-1,final_point.y-1,3,3));
		 //					lcd->FillColor(libsc::Lcd::kRed);
		 switch(align) {
		 case 0:
		 destination=final_point;
		 servo_angle = 1120 + std::atan(1.0*(img2world[destination.x][destination.y][0] - img2world[46][118][0]) / (img2world[destination.x][destination.y][1] - img2world[46][118][1])) * 1800 / 3.14;
		 servo_angle += 0.4 * (servo_angle - prev_servo_angle);
		 servo->SetDegree(servo_angle);
		 break;
		 case 1:
		 destination=final_point;
		 servo_angle = 1120 + std::atan(1.0*(img2world[destination.x][destination.y][0] - img2world[97][118][0]) / (img2world[destination.x][destination.y][1] - img2world[97][118][1])) * 1800 / 3.14;
		 servo_angle += 0.4 * (servo_angle - prev_servo_angle);
		 servo->SetDegree(servo_angle);
		 break;
		 case 2:
		 destination=final_point;
		 servo_angle = 1120 + std::atan(1.0*(img2world[destination.x][destination.y][0] - img2world[149][118][0]) / (img2world[destination.x][destination.y][1] - img2world[149][118][1])) * 1800 / 3.14;
		 servo_angle += 0.4 * (servo_angle - prev_servo_angle);
		 servo->SetDegree(servo_angle);
		 break;
		 }
		 } else if (left_edge.size() > right_edge.size()) {
		 destination=left_edge.back();
		 servo_angle = 1120 + std::atan(1.0*(img2world[destination.x][destination.y][0] - img2world[46][118][0]) / (img2world[destination.x][destination.y][1] - img2world[46][118][1])) * 1800 / 3.14;
		 servo_angle += 0.4 * (servo_angle - prev_servo_angle);
		 servo->SetDegree(servo_angle);
		 } else if(right_edge.size()) {
		 destination=right_edge.back();
		 servo_angle = 1120 + std::atan(1.0*(img2world[destination.x][destination.y][0] - img2world[149][118][0]) / (img2world[destination.x][destination.y][1] - img2world[149][118][1])) * 1800 / 3.14;
		 servo_angle += 0.4 * (servo_angle - prev_servo_angle);
		 servo->SetDegree(servo_angle);
		 }
		 break;
		 case 2:
		 if(right_edge.size()) {
		 if (right_end_point_found) {
		 destination = right_end_point;
		 } else {
		 destination = right_edge.back();
		 }
		 servo_angle = 1120 + std::atan(1.0*(img2world[destination.x][destination.y][0] - img2world[149][118][0]) / (img2world[destination.x][destination.y][1] - img2world[149][118][1])) * 1800 / 3.14;
		 servo_angle += 0.4 * (servo_angle - prev_servo_angle);
		 servo->SetDegree(servo_angle);
		 break;
		 }
		 }
		 } else if(track_state==Tstate::Crossroad) {
		 switch (align) {
		 case 0:
		 servo_angle = 1120 + std::atan(1.0 * (img2world[final_point.x][final_point.y][0] - img2world[46][118][0]) / (img2world[final_point.x][final_point.y][1] - img2world[46][118][1])) * 1800 / 3.14;
		 servo_angle += 0.4 * (servo_angle - prev_servo_angle);
		 servo->SetDegree(servo_angle);
		 break;
		 case 1:
		 servo_angle = 1120 + std::atan(1.0*(img2world[final_point.x][final_point.y][0] - img2world[97][118][0]) / (img2world[final_point.x][final_point.y][1] - img2world[97][118][1])) * 1800 / 3.14;
		 servo_angle += 0.4 * (servo_angle - prev_servo_angle);
		 servo->SetDegree(servo_angle);
		 break;
		 case 2:
		 servo_angle = 1120 + std::atan(1.0*(img2world[final_point.x][final_point.y][0] - img2world[149][118][0]) / (img2world[final_point.x][final_point.y][1] - img2world[149][118][1])) * 1800 / 3.14;
		 servo_angle += 0.4 * (servo_angle - prev_servo_angle);
		 servo->SetDegree(servo_angle);
		 break;
		 }
		 } else if(track_state==Tstate::RightLoop) {
		 switch(loop_state) {
		 case Lstate::Entering:
		 destination=right_end_point;
		 servo_angle = 1120 + std::atan(1.0*(img2world[destination.x][destination.y][0] - img2world[149][118][0]) / (img2world[destination.x][destination.y][1] - img2world[149][118][1])) * 1800 / 3.14;
		 servo_angle += 0.4 * (servo_angle - prev_servo_angle);
		 servo->SetDegree(servo_angle);
		 break;
		 case Lstate::Leaving:
		 destination=right_end_point;
		 servo_angle = 1120 + std::atan(1.0*(img2world[destination.x][destination.y][0] - img2world[149][118][0]) / (img2world[destination.x][destination.y][1] - img2world[149][118][1])) * 1800 / 3.14;
		 servo_angle += 0.4 * (servo_angle - prev_servo_angle);
		 servo->SetDegree(servo_angle);
		 break;
		 case Lstate::Finished:
		 destination=left_edge.back();
		 servo_angle = 1120 + std::atan(1.0*(img2world[destination.x][destination.y][0] - img2world[46][118][0]) / (img2world[destination.x][destination.y][1] - img2world[46][118][1])) * 1800 / 3.14;
		 servo_angle += 0.4 * (servo_angle - prev_servo_angle);
		 servo->SetDegree(servo_angle);
		 break;
		 }
		 } else if(track_state==Tstate::LeftLoop) {

		 }
		 prev_track_state = track_state;
		 }*/
	}
}
