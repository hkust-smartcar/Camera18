/*
 * algo.cpp
 *
 *  Created on: May 30, 2018
 *      Author: Amrutavarsh S Kinagi
 *      	LeeChunHei
 */

#include <algo.h>

enum Tstate {
	Normal, Crossroad, RightLoop, LeftLoop
};

enum Cstate {
	Detected, L_inside, C_inside, R_inside, Outside
};

enum Lstate {
	Entering, In, Leaving, Finished,
};

enum Align {
	left_align, center_align, right_align
};

const Byte* buffer;

inline Byte GetPoint(uint8_t x, uint8_t y) {
	return buffer[x + y * 189];
}

inline int16_t SobelEdgeDetection(uint8_t x, uint8_t y) {
	return std::abs(-GetPoint((x) - 1, (y) - 1) - 2 * GetPoint((x) - 1, (y)) - GetPoint((x) - 1, (y) + 1) + GetPoint((x) + 1, (y) - 1) + 2 * GetPoint((x) + 1, (y)) + GetPoint((x) + 1, (y) + 1)) + std::abs(-GetPoint((x) - 1, (y) - 1) - 2 * GetPoint((x), (y) - 1) - GetPoint((x) + 1, (y) - 1) + GetPoint((x) - 1, (y) + 1) + 2 * GetPoint((x), (y) + 1) + GetPoint((x) + 1, (y) + 1));
}

float target_speed = 500;
float Kp = 0.015;
float Ki = 0.001;
float Kd = 0;
float servo_D = 6.0;
float search_distance = std::pow(target_speed * 0.4, 2);
float search_m;
float search_c;
float search_slope;
std::pair<float, float> target;
//img2world[91][119] is the bottom mid point of the image, may need to tune later
const float search_origin_x = img2world[97][115][0];
const float search_origin_y = img2world[97][115][1];

const float search_left_align_x = img2world[47][115][0];
const float search_left_align_y = img2world[47][115][1];

const float search_right_align_x = img2world[148][115][0];
const float search_right_align_y = img2world[148][115][1];

coor left_end_point;
coor right_end_point;
bool left_end_point_found;
bool right_end_point_found;
bool left_target_found;
bool right_target_found;

bool right_jump = false;
bool left_jump = false;

inline void InitSearchDistance(float mpu_angle) {
//	mpu_angle *= 0.00008726646;	//angle = angular_speed * 5ms * PI / 180
//	search_m = -std::tan(mpu_angle);
//	search_c = (search_distance * std::cos(mpu_angle) - search_m * (search_distance * std::sin(mpu_angle) - search_origin_x));
	left_end_point_found = false;
	right_end_point_found = false;
	left_target_found = false;
	right_target_found = false;
}

inline bool FindLeftEndPoint(int x, int y) {
	float rx = img2world[x][y][0] - search_left_align_x;
	rx *= rx;
	rx = search_distance - rx;
	float ry = img2world[x][y][1] - search_left_align_y;
	ry *= ry;
	return rx < ry;
}

inline bool FindEndPoint(int x, int y) {
	float rx = img2world[x][y][0] - search_origin_x;
	rx *= rx;
	rx = search_distance - rx;
	float ry = img2world[x][y][1] - search_origin_y;
	ry *= ry;
	return rx < ry;
}

inline bool FindRightEndPoint(int x, int y) {
	float rx = img2world[x][y][0] - search_right_align_x;
	rx *= rx;
	rx = search_distance - rx;
	float ry = img2world[x][y][1] - search_right_align_y;
	ry *= ry;
	return rx < ry;
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

Align align = center_align;

inline void FindEndPointFromLeft(int front_x, int front_y, int end_x, int end_y) {
	float temp_1 = end_x - front_x;
	float temp_2 = front_y - end_y;
	float vector_mag = std::sqrt(temp_1 * temp_1 + temp_2 * temp_2);
	float midpoint_x = 200 * (temp_2) / vector_mag + end_x;
	float midpoint_y = 200 * (temp_1) / vector_mag + end_y;
	float rx = midpoint_x - search_origin_x;
	rx *= rx;
	rx = search_distance - rx;
	float ry = midpoint_y - search_origin_y;
	ry *= ry;
	if (rx < ry) {
		left_target_found = true;
		target.first = right_target_found ? (target.first + midpoint_x) / 2 : midpoint_x;
		target.second = right_target_found ? (target.second + midpoint_y) / 2 : midpoint_y;
		if (right_target_found) {
			float rx = target.first - search_origin_x;
			rx *= rx;
			rx = search_distance - rx;
			float ry = target.second - search_origin_y;
			ry *= ry;
			if (rx > ry) {
				target.first = midpoint_x;
				target.second = midpoint_y;
			}
		}
	}
//	float slope = -1.0 * (front_x - end_x) / (front_y - end_y);
//	float temp = std::sqrt(40000 / (1 + slope * slope));
//	float midpoint_x1 = end_x + temp;
//	float midpoint_x2 = end_x - temp;
//	float midpoint_y1 = slope * (midpoint_x1 - end_x) + end_y;
//	float midpoint_y2 = slope * (midpoint_x2 - end_x) + end_y;
//	if ((end_x - midpoint_x1) * (front_y - end_y) - (end_y - midpoint_y1) * (front_x - end_x) > 0) {
//		float rx = midpoint_x1 - search_origin_x;
//		rx *= rx;
//		rx = search_distance - rx;
//		float ry = midpoint_y1 - search_origin_y;
//		ry *= ry;
// 		if (rx < ry) {
//			target_found = true;
//			target.first = midpoint_x1;
//			target.second = midpoint_y1;
//		}
//	} else {
//		float rx = midpoint_x2 - search_origin_x;
//		rx *= rx;
//		rx = search_distance - rx;
//		float ry = midpoint_y2 - search_origin_y;
//		ry *= ry;
//		if (rx < ry) {
//			target_found = true;
//			target.first = midpoint_x2;
//			target.second = midpoint_y2;
//		}
//	}
}

inline void FindEndPointFromRight(int front_x, int front_y, int end_x, int end_y) {
	float temp_1 = front_x - end_x;
	float temp_2 = end_y - front_y;
	float vector_mag = std::sqrt(temp_1 * temp_1 + temp_2 * temp_2);
	float midpoint_x = 200 * (temp_2) / vector_mag + end_x;
	float midpoint_y = 200 * (temp_1) / vector_mag + end_y;
	float rx = midpoint_x - search_origin_x;
	rx *= rx;
	rx = search_distance - rx;
	float ry = midpoint_y - search_origin_y;
	ry *= ry;
	if (rx < ry) {
		right_target_found = true;
		target.first = left_target_found ? (target.first + midpoint_x) / 2 : midpoint_x;
		target.second = left_target_found ? (target.second + midpoint_y) / 2 : midpoint_y;
		if (left_target_found) {
			float rx = target.first - search_origin_x;
			rx *= rx;
			rx = search_distance - rx;
			float ry = target.second - search_origin_y;
			ry *= ry;
			if (rx > ry) {
				target.first = midpoint_x;
				target.second = midpoint_y;
			}
		}
	}
//	float slope = -1.0 * (front_x - end_x) / (front_y - end_y);
//	float temp = std::sqrt(40000 / (1 + slope * slope));
//	float midpoint_x1 = end_x + temp;
//	float midpoint_x2 = end_x - temp;
//	float midpoint_y1 = slope * (midpoint_x1 - end_x) + end_y;
//	float midpoint_y2 = slope * (midpoint_x2 - end_x) + end_y;
//	if ((end_x - midpoint_x1) * (front_y - end_y) - (end_y - midpoint_y1) * (front_x - end_x) < 0) {
//		float rx = midpoint_x1 - search_origin_x;
//		rx *= rx;
//		rx = search_distance - rx;
//		float ry = midpoint_y1 - search_origin_y;
//		ry *= ry;
//		if (rx < ry) {
//			target_found = true;
//			target.first = midpoint_x1;
//			target.second = midpoint_y1;
//		}
//	} else {
//		float rx = midpoint_x2 - search_origin_x;
//		rx *= rx;
//		rx = search_distance - rx;
//		float ry = midpoint_y2 - search_origin_y;
//		ry *= ry;
//		if (rx < ry) {
//			target_found = true;
//			target.first = midpoint_x2;
//			target.second = midpoint_y2;
//		}
//	}
}

bool FindLeftEdge(int& edge_prev_dir) {
	int size = left_edge.size();
	if (size > 100 || !size)
		return false;
	coor edge_coor = left_edge.back();
	int x = edge_coor.x;
	int y = edge_coor.y;
	if (x < 4 || x > 184 || y < 4 || y > 116)
		return false;
//	if (!left_end_point_found && size % 5 == 0) {
//		if (FindLeftEndPoint(x, y)) {
//			left_end_point_found = true;
//			left_end_point= {x,y};
//		}
//	}
	if (!left_target_found && size % 4 == 0) {
		FindEndPointFromLeft(img2world[left_edge[size - 4].x][left_edge[size - 4].y][0], img2world[left_edge[size - 4].x][left_edge[size - 4].y][1], img2world[left_edge[size - 1].x][left_edge[size - 1].y][0], img2world[left_edge[size - 1].x][left_edge[size - 1].y][1]);
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
//	if (!right_end_point_found && size % 5 == 0) {
//		if (FindRightEndPoint(x, y)) {
//			right_end_point_found = true;
//			right_end_point= {x,y};
//		}
//	}
	if (!right_target_found && size % 4 == 0) {
		FindEndPointFromRight(img2world[right_edge[size - 4].x][right_edge[size - 4].y][0], img2world[right_edge[size - 4].x][right_edge[size - 4].y][1], img2world[right_edge[size - 1].x][right_edge[size - 1].y][0], img2world[right_edge[size - 1].x][right_edge[size - 1].y][1]);
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

void FindJumpStart(int& prev_edge_dir) {
	const int x = prev_edge_dir == left ? left_edge.back().x : right_edge.back().x;
	const int y = prev_edge_dir == left ? left_edge.back().y : right_edge.back().y;
	bool prev_white = SobelEdgeDetection(x + 1, y - 1) < edge_threshold;
	std::vector<coor> point;
	std::vector<uint8_t> dir;
	coor temp;
	for (int i = 0; i < 8; i++) {
		switch (i) {
		case up:
			if (prev_white != (SobelEdgeDetection(x, y - 1) < edge_threshold)) {
				if (prev_white) {
					temp= {x+1,y-1};
					dir.push_back(7);
				} else {
					temp= {x,y-1};
					dir.push_back(i);
				}
				point.push_back(temp);
				prev_white=!prev_white;
			}
			break;
			case up_left:
			if (prev_white!=(SobelEdgeDetection(x - 1, y - 1) < edge_threshold)) {
				if(prev_white) {
					temp= {x,y-1};
					dir.push_back(0);
				} else {
					temp= {x-1,y-1};
					dir.push_back(i);
				}
				point.push_back(temp);
				prev_white=!prev_white;
			}
			break;
			case left:
			if (prev_white!=(SobelEdgeDetection(x - 1, y) < edge_threshold)) {
				if(prev_white) {
					temp= {x-1,y-1};
					dir.push_back(1);
				} else {
					temp= {x-1,y};
					dir.push_back(i);
				}
				point.push_back(temp);
				prev_white=!prev_white;
			}
			break;
			case down_left:
			if (prev_white!=(SobelEdgeDetection(x - 1, y + 1) < edge_threshold)) {
				if(prev_white) {
					temp= {x-1,y};
					dir.push_back(2);
				} else {
					temp= {x-1,y+1};
					dir.push_back(i);
				}
				point.push_back(temp);
				prev_white=!prev_white;
			}
			break;
			case down:
			if (prev_white!=(SobelEdgeDetection(x, y + 1) < edge_threshold)) {
				if(prev_white) {
					temp= {x-1,y+1};
					dir.push_back(3);
				} else {
					temp= {x,y+1};
					dir.push_back(i);
				}
				point.push_back(temp);
				prev_white=!prev_white;
			}
			break;
			case down_right:
			if (prev_white!=(SobelEdgeDetection(x + 1, y + 1) < edge_threshold)) {
				if(prev_white) {
					temp= {x,y+1};
					dir.push_back(4);
				} else {
					temp= {x+1,y+1};
					dir.push_back(i);
				}
				point.push_back(temp);
				prev_white=!prev_white;
			}
			break;
			case right:
			if (prev_white!=(SobelEdgeDetection(x + 1, y) < edge_threshold)) {
				if(prev_white) {
					temp= {x+1,y+1};
					dir.push_back(5);
				} else {
					temp= {x+1,y};
					dir.push_back(i);
				}
				point.push_back(temp);
				prev_white=!prev_white;
			}
			break;
			case up_right:
			if (prev_white!=(SobelEdgeDetection(x + 1, y - 1) < edge_threshold)) {
				if(prev_white) {
					temp= {x+1,y};
					dir.push_back(6);
				} else {
					temp= {x+1,y-1};
					dir.push_back(i);
				}
				point.push_back(temp);
				prev_white=!prev_white;
			}
			break;
		}
	}
	if (prev_edge_dir == left) {
		coor final = { 5, 5 };
		for (int i = 0; i < point.size(); i++) {
			if ((point[i].x > final.x) || (GetPoint(point[i].x, point[i].y) > GetPoint(final.x, final.y))) {
				final = point[i];
				prev_edge_dir = dir[i];
			}
		}
		left_edge.push_back(final);
//		if (point.front().x > point.back().x) {
//			left_edge.push_back(point.front());
//			prev_edge_dir = dir.front();
//		} else {
//			left_edge.push_back(point.back());
//			prev_edge_dir = dir.back();
//		}
	} else {
		coor final = { 186, 5 };
		for (int i = 0; i < point.size(); i++) {
			if ((point[i].x < final.x) || (GetPoint(point[i].x, point[i].y) > GetPoint(final.x, final.y))) {
				final = point[i];
				prev_edge_dir = dir[i];
			}
		}
//		if (point.front().x < point.back().x) {
//			right_edge.push_back(point.front());
//			prev_edge_dir = dir.front();
//		} else {
//			right_edge.push_back(point.back());
//			prev_edge_dir = dir.back();
//		}
	}
}

bool dist_corners(coor m, coor n) {
	int x1 = img2world[m.x][m.y][0];
	int y1 = img2world[m.x][m.y][1];
	int x2 = img2world[n.x][n.y][0];
	int y2 = img2world[n.x][n.y][1];
	return ((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2) <= dist_threshold);
}

bool check_corner(coor pivot, coor m, coor n) {
	if (left_jump || right_jump) {
		int a2 = (img2world[pivot.x][pivot.y][0] - img2world[m.x][m.y][0]) * (img2world[pivot.x][pivot.y][0] - img2world[m.x][m.y][0]) + (img2world[pivot.x][pivot.y][1] - img2world[m.x][m.y][1]) * (img2world[pivot.x][pivot.y][1] - img2world[m.x][m.y][1]);
		a2 /= 100;
		int b2 = (img2world[pivot.x][pivot.y][0] - img2world[n.x][n.y][0]) * (img2world[pivot.x][pivot.y][0] - img2world[n.x][n.y][0]) + (img2world[pivot.x][pivot.y][1] - img2world[n.x][n.y][1]) * (img2world[pivot.x][pivot.y][1] - img2world[n.x][n.y][1]);
		b2 /= 100;
		int c2 = (img2world[m.x][m.y][0] - img2world[n.x][n.y][0]) * (img2world[m.x][m.y][0] - img2world[n.x][n.y][0]) + (img2world[m.x][m.y][1] - img2world[n.x][n.y][1]) * (img2world[m.x][m.y][1] - img2world[n.x][n.y][1]);
		c2 /= 100;
		int pythagoras = a2 + b2 - c2;
		if (pythagoras >= 0)
			return true;
		pythagoras = std::abs(pythagoras);
		uint64_t pythagoras_uint = (uint64_t) pythagoras;
		float value = ((((float) ((uint64_t) (pythagoras_uint * pythagoras_uint)) / 4) / a2) / b2);
		if (value < 0.17)
			return true;
		return false;
	} else {
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
	}
	return false;
}

float value_corner(coor pivot, coor m, coor n) {
	int c2 = (m.x - n.x) * (m.x - n.x) + (m.y - n.y) * (m.y - n.y);
	int a2 = (pivot.x - m.x) * (pivot.x - m.x) + (pivot.y - m.y) * (pivot.y - m.y);
	int b2 = (pivot.x - n.x) * (pivot.x - n.x) + (pivot.y - n.y) * (pivot.y - n.y);
	int pythagoras = a2 + b2 - c2;
	return ((float) (pythagoras * pythagoras)) / (4 * a2 * b2);
}

void empty_left() {
	left_edge.clear();
	left_edge_corner.clear();
	left_jump = false;
}

void empty_right() {
	right_edge.clear();
	right_edge_corner.clear();
	right_jump = false;
}

void LeftEdge(coor start_point, int& edge_prev_dir, bool append) {
	int direction = edge_prev_dir;
	if (!append) {
		left_edge.clear();
		left_edge_corner.clear();
		left_edge.push_back(start_point);
	}
	if (left_edge.size() && left_edge.back().y != start_point.y && left_edge.back().x != start_point.x)
		left_edge.push_back(start_point);
	else if (!left_edge.size())
		left_edge.push_back(start_point);

	while (FindLeftEdge(edge_prev_dir) && left_edge.back().y > 30) {
		if ((direction == up && left_edge.back().y > start_point.y + 10) || (direction == down && left_edge.back().y < start_point.y - 10) || (direction == left && left_edge.back().x > start_point.x + 10) || (direction == right && left_edge.back().x < start_point.x - 10))
			break;
		if (left_edge.size() > 15 && left_edge[left_edge.size() - 11].y > 30 && check_corner(left_edge[left_edge.size() - 11], left_edge[left_edge.size() - 1], left_edge[left_edge.size() - 16])) {
			left_edge_corner.push_back(left_edge.size() - 11);
			break;
		}
	}
}

void RightEdge(coor start_point, int& edge_prev_dir, bool append) {
	int direction = edge_prev_dir;
	if (!append) {
		right_edge.clear();
		right_edge_corner.clear();
		right_edge.push_back(start_point);
	}
	if (right_edge.size() && right_edge[right_edge.size() - 1].y != start_point.y && right_edge[right_edge.size() - 1].x != start_point.x)
		right_edge.push_back(start_point);
	else if (!right_edge.size())
		right_edge.push_back(start_point);

	while (FindRightEdge(edge_prev_dir) && right_edge.back().y > 30) {
		if ((direction == up && right_edge[right_edge.size() - 1].y > start_point.y + 10) || (direction == down && right_edge[right_edge.size() - 1].y < start_point.y - 10) || (direction == left && right_edge[right_edge.size() - 1].x > start_point.x + 10) || (direction == right && right_edge[right_edge.size() - 1].x < start_point.x - 10))
			break;
		if (right_edge.size() > 15 && right_edge[right_edge.size() - 11].y > 30 && check_corner(right_edge[right_edge.size() - 11], right_edge[right_edge.size() - 1], right_edge[right_edge.size() - 16])) {
			right_edge_corner.push_back(right_edge.size() - 11);
			break;
		}
	}
}

//for left loop
void RightLoopEdgeL(coor start_point, int& edge_prev_dir, coor& rightmostP, bool append) {
	if (!append) {
		empty_right();
	}
	right_edge.push_back(start_point);
	if (right_edge[0].x > rightmostP.x)
		rightmostP = right_edge[0];
	while (FindRightEdge(edge_prev_dir) && ((right_edge[right_edge.size() - 1].x - start_point.x) > -20)) {
		if (right_edge[right_edge.size() - 1].y > 30 && right_edge[right_edge.size() - 1].x > rightmostP.x)
			rightmostP = right_edge[right_edge.size() - 1];
	}
}
//for right loop
void RightLoopEdgeR(coor start_point, int& edge_prev_dir, coor& leftmostP, bool append) {
	if (!append) {
		empty_right();
	}

	right_edge.push_back(start_point);
	if (right_edge[0].x < leftmostP.x)
		leftmostP = right_edge[0];
	while (FindRightEdge(edge_prev_dir) && ((right_edge[right_edge.size() - 1].x - start_point.x) > -20)) {
		if (right_edge[right_edge.size() - 1].y > 30 && right_edge[right_edge.size() - 1].x < leftmostP.x)
			leftmostP = right_edge[right_edge.size() - 1];
	}
}
//for right loop
void LeftLoopEdgeR(coor start_point, int& edge_prev_dir, coor& leftmostP, bool append) {
	if (!append)
		empty_left();
	left_edge.push_back(start_point);
	if (left_edge[0].x < leftmostP.x)
		leftmostP = left_edge[0];
	while (FindLeftEdge(edge_prev_dir) && (left_edge[left_edge.size() - 1].x - start_point.x < 20)) {
		if (left_edge[left_edge.size() - 1].x > 30 && left_edge[left_edge.size() - 1].x < leftmostP.x)
			leftmostP = left_edge[left_edge.size() - 1];
	}
}
//for left loop
void LeftLoopEdgeL(coor start_point, int& edge_prev_dir, coor& rightmostP, bool append) {
	if (!append)
		empty_left();
	//if (left_edge.size() && left_edge[left_edge.size() - 1].y != start_point.y && left_edge[left_edge.size() - 1].x != start_point.x)
	left_edge.push_back(start_point);
	if (left_edge[0].x > rightmostP.x)
		rightmostP = left_edge[0];
	while (FindLeftEdge(edge_prev_dir) && (left_edge[left_edge.size() - 1].x - start_point.x < 20)) {
		if (left_edge[left_edge.size() - 1].y > 30 && left_edge[left_edge.size() - 1].x > rightmostP.x)
			rightmostP = left_edge[left_edge.size() - 1];
	}
}

bool jump(coor corner, coor point1, coor point2, coor& new_start, bool direction) { //jump from the corner to new edge. returns true if a edge is found.
	float slope = (float) (point1.x - corner.x) / (point1.y - corner.y);
	float constant = point2.x - slope * point2.y;
	new_start.y = point2.y - 15;
	new_start.x = (int16_t) (new_start.y * slope + constant);
	coor prev_start = { 0, 0 };
	while (new_start.x < width - 4 && new_start.x > 3 && new_start.y < height - 4 && new_start.y > 3 && SobelEdgeDetection(new_start.x, new_start.y) < edge_threshold) {
//        lcd->SetRegion(libsc::St7735r::Lcd::Rect(new_start.x, new_start.y, 1, 1));
//        lcd->FillColor(lcd->kBlue);
		prev_start = new_start;
		new_start.y--;
		new_start.x = (new_start.y * slope + constant);
	}
	new_start = prev_start;
	if (new_start.x < width - 4 && new_start.x > 3 && new_start.y < height - 4 && new_start.y > 3) {
		if (direction)
			right_jump = true;
		else
			left_jump = true;
		return true;
	}
	return false;
}

bool Ljump(coor& new_start) {
	coor point1 = { 0, 0 };
	coor point2 = { 0, 0 };
	if (left_edge_corner[0] > 10)
		point1 = left_edge[left_edge_corner[0] - 10];
	else
		point1 = left_edge[0];
	int i = left_edge_corner[0];
	for (; FindLeftEdge(left_edge_prev_dir) && value_corner(left_edge[left_edge.size() - 11], left_edge[left_edge.size() - 1], left_edge[left_edge.size() - 16]) < 0.9; i++)
		;
	point2 = left_edge[i + 3];
	return jump(left_edge[left_edge_corner[0]], point1, point2, new_start, false);
}

bool Rjump(coor& new_start) {
	coor point1 = { 0, 0 };
	coor point2 = { 0, 0 };
	if (right_edge_corner[0] > 10)
		point1 = right_edge[right_edge_corner[0] - 10];
	else
		point1 = right_edge[0];
	int i = right_edge_corner[0];
	for (; FindRightEdge(right_edge_prev_dir) && value_corner(right_edge[right_edge.size() - 11], right_edge[right_edge.size() - 1], right_edge[right_edge.size() - 16]) < 0.9; i++)
		;
	point2 = right_edge[i + 3];
	return jump(right_edge[right_edge_corner[0]], point1, point2, new_start, true);
}

bool right_start_point(coor midpoint, coor& right_start, int threshold) {
	right_start = midpoint;
	while (right_start.x < width - 4 && right_start.x > 3 && SobelEdgeDetection(right_start.x, right_start.y) < threshold)
		right_start.x++;
	if (right_start.x < width - 4)
		return true;
	return false;
}

bool left_start_point(coor midpoint, coor& left_start, int threshold) {
	left_start = midpoint;
	while (left_start.x > 3 && left_start.x < width - 4 && SobelEdgeDetection(left_start.x, left_start.y) < threshold)
		left_start.x--;
	if (left_start.x > 3)
		return true;
	return false;
}

void normal_left_corner_fsm(Tstate& track_state, coor& final_point, coor& midpoint, coor left_start, coor right_start, int& edge_prev_dir) {
	align = left_align;
	coor new_start = { 0, 0 };
	if (Ljump(new_start)) {
		left_edge_prev_dir = right;
		left_edge.push_back(new_start);
//		FindJumpStart(left_edge_prev_dir);
		LeftEdge(left_edge.back(), left_edge_prev_dir, false);
		if (left_edge_corner.size() == 1)
			track_state = Crossroad;
		else
			track_state = LeftLoop;
	} else {
		final_point = new_start;
		midpoint= {(left_start.x+right_start.x)/2,110};
	}
}

void normal_right_corner_fsm(Tstate& track_state, coor& final_point, coor& midpoint, coor left_start, coor right_start, int& edge_prev_dir) {
	align = right_align;
	coor new_start = { 0, 0 };
	if (Rjump(new_start)) {
		right_edge_prev_dir = left;
		right_edge.push_back(new_start);
//		FindJumpStart(right_edge_prev_dir);
		RightEdge(right_edge.back(), right_edge_prev_dir, false);
		if (right_edge_corner.size() == 1)
			track_state = Crossroad;
		else
			track_state = RightLoop;
	} else {
		final_point = new_start;
		midpoint= {(left_start.x+right_start.x)/2,110};
	}
}

bool isLeftBottomExist(coor midpoint, int threshold) {
	coor left_start = { midpoint.x, 115 };
	while (SobelEdgeDetection(left_start.x, left_start.y) < threshold && left_start.x > 0)
		left_start.x--;
	if (left_start.x > 0)
		return true;
	return false;
}

bool isRightBottomExist(coor midpoint, int threshold) {
	coor right_start = { midpoint.x, 115 };
	while (SobelEdgeDetection(right_start.x, right_start.y) < threshold && right_start.x < width - 1)
		right_start.x++;
	if (right_start.x < width - 1)
		return true;
	return false;
}

void print(const std::vector<coor>& v) {
	for (uint16_t i = 0; i < v.size(); i++) {
		coor t = v[i];
		lcd->SetRegion(libsc::Lcd::Rect(t.x, t.y, 2, 2));
		lcd->FillColor(lcd->kRed);
	}
}

void printsize() {

	char buffer[50];
	sprintf(buffer, "l %d r %d", left_edge.size(), right_edge.size());
	lcd->SetRegion(libsc::Lcd::Rect(80, 40, 40, 40));
	writerP->WriteString(buffer);

}

void algo() {
	libsc::System::DelayMs(100);
	libsc::Timer::TimerInt time_now = 0;
	camera->Start();
	Tstate track_state = Normal;
	Tstate prev_track_state = Normal;
	Cstate crossroad_state = Detected;
	Lstate loop_state = Entering;
	coor midpoint = { 97, 115 };

	coor left_start = { 0, 0 };
	coor right_start = { 0, 0 };
	coor final_point = { 0, 0 };

	int no_movement_count = 0;
	int cum_error = 0;
	int prev_error = 0;

	int degree_1 = 0;
	int degree_2 = 0;
	int degree_3 = 0;
	int prev_angle = 0;

	bool debug = true;

//	float search_const = 0.4;
	InitBluetoothDebug(&debug);
//	PushTuneFloat("servo_D\n", (Byte*) &servo_D);
//	PushTuneInt32("speed\n", (Byte*) &target_speed);
//	PushTuneFloat("search_c\n", (Byte*) &search_const);
//	InitValueDebug();
	libsc::Timer::TimerInt motor_start_time = libsc::System::Time();
	bool motor_start = true; //false;
	int count_encoder = 0;
//	target_speed = 100;
	while (1) {
		if (camera->IsAvailable()) {
			if (!debug) {
//				if (target_speed < 610)
//				target_speed += 0.1;
				search_distance = std::pow(target_speed * 0.4, 2);
//				char buffer[100] = { };
//				sprintf(buffer, "%.2f,%.2f\n", target_speed, search_distance);
//				bt->SendStr(buffer);
			}
			if (search_distance < 10000)
				search_distance = 10000;
//			float search_distance = std::pow(target_speed * search_const, 2);
			time_now = libsc::System::Time();
			buffer = camera->LockBuffer(); //Use GetPoint(x,y) to get the gradient of the point
			camera->UnlockBuffer();
			if (debug) {
				if (joystick->GetState() == libsc::Joystick::State::kSelect) {
					debug = false;
					encoder->Update();
				}
			}
			if (debug) {
				for (uint16_t i = 0; i < height; i++) {
					lcd->SetRegion(libsc::Lcd::Rect(0, i, 160, 1));
					lcd->FillGrayscalePixel(buffer + camera->GetW() * i, 160);
				}
			}
//			encoder->Update();
//			int32_t curr = encoder->GetCount();
//			count_encoder += curr;
//			char buffer[100] = { };
//			sprintf(buffer, "%d", count_encoder);
//			console->Clear(false);
//			console->SetCursorRow(4);
//			console->WriteString(buffer);
			if (!debug) {
				encoder->Update();
				int32_t curr = encoder->GetCount();
//				char buffer[100] = { };
//				sprintf(buffer, "%d", curr);
//				console->Clear(false);
//				console->SetCursorRow(4);
//				console->WriteString(buffer);
				if (std::abs(curr) < 100) {
					++no_movement_count;
					cum_error = 0;
				} else {
					no_movement_count = 0;
				}
				if (std::abs(curr) > 10000) {
					curr = target_speed;
				}
//				if (std::abs(curr) > 100 && !motor_start && libsc::System::Time() - motor_start_time > 4000) {
//					motor_start_time = libsc::System::Time();
//					motor_start = true;
//				}
//				if ((libsc::System::Time() - motor_start_time) > 3000) {
//					motor_start = false;
//				}
				int32_t error = target_speed + curr;	//curr is negative when forward
				cum_error += error;
				float volt = Kp * error + Ki * cum_error + Kd * (error - prev_error);
				int speed_set = (int) (volt / battery_meter->GetVoltage() * 1000);
				speed_set = (speed_set > -500) ? ((speed_set > 500) ? 500 : speed_set) : -500;
				speed_set = -speed_set;
				if (no_movement_count > 5 || !motor_start) {
					motor->SetPower(0);
				} else {
					if (speed_set < 0) {
						motor->SetClockwise(false);
						motor->SetPower(-speed_set);
					} else {
						motor->SetClockwise(true);
						motor->SetPower(speed_set);
					}
				}
				prev_error = error;
			} else {
				motor->SetPower(0);
				cum_error = 0;
			}

//			mpu->UpdateF();

			InitSearchDistance(0); //-(mpu->GetOmegaF()[2]));

			if (debug) {
//				char buff[10]={};
//				sprintf(buff,"%.2f",mpu->GetOmegaF()[2]);
//				console->Clear(false);
//				console->SetCursorRow(4);
//				console->WriteString(buff);
			}

			empty_left();
			empty_right();

			coor leftmostP = { 188, 120 }; //sure this is right? it resets leftmostP in every new frame
			coor rightmostP = { 0, 120 };  //sure this shouldn't be outside while loop?
			coor upmostP = { 188, 120 };
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
							if ((right_edge[right_edge_corner[0]].y > 90) || left_edge[left_edge_corner[0]].y > 90) {
								track_state = Crossroad;
							} else {
								final_point = {
									(left_edge[left_edge_corner[0]].x + right_edge[right_edge_corner[0]].x) / 2,
									(left_edge[left_edge_corner[0]].y + right_edge[right_edge_corner[0]].y) / 2};
								align = center_align;
								midpoint = {(left_start.x + right_start.x) / 2, 110};
							}
						} else if (left_edge[left_edge_corner[0]].y > right_edge[right_edge_corner[0]].y) {
							if (left_edge[left_edge_corner[0]].y > 90)
							normal_left_corner_fsm(track_state, final_point, midpoint, left_start, right_start,
									left_edge_prev_dir);
							else {
								final_point = left_end_point.y > left_edge[left_edge_corner[0]].y
								? left_edge[left_edge_corner[0]] : left_end_point;
								align = left_align;
								midpoint = {(left_start.x + right_start.x) / 2, 110};
							}
						} else if (left_edge[left_edge_corner[0]].y < right_edge[right_edge_corner[0]].y) {
							if (right_edge[right_edge_corner[0]].y > 90)
							normal_right_corner_fsm(track_state, final_point, midpoint, left_start, right_start,
									right_edge_prev_dir);
							else {
								final_point = right_end_point.y > right_edge[right_edge_corner[0]].y
								? right_edge[right_edge_corner[0]] : right_end_point;
								align = right_align;
								midpoint = {(left_start.x + right_start.x) / 2, 110};
							}
						}
					} else if (left_edge_corner.size() == 1 && right_edge_corner.size() == 0) {
						if (left_edge[left_edge_corner[0]].y > 90)
						normal_left_corner_fsm(track_state, final_point, midpoint, left_start, right_start,
								left_edge_prev_dir);
						else {
							final_point =
							left_end_point.y > left_edge[left_edge_corner[0]].y ? left_edge[left_edge_corner[0]]
							: left_end_point;
							align = left_align;
							midpoint = {(left_start.x + right_start.x) / 2, 110};
						}
					} else if (left_edge_corner.size() == 0 && right_edge_corner.size() == 1) {
						if (right_edge[right_edge_corner[0]].y > 90)
						normal_right_corner_fsm(track_state, final_point, midpoint, left_start, right_start,
								right_edge_prev_dir);
						else {
							final_point = right_end_point.y > right_edge[right_edge_corner[0]].y
							? right_edge[right_edge_corner[0]] : right_end_point;
							align = right_align;
							midpoint = {(left_start.x + right_start.x) / 2, 110};
						}
					} else {
						align = center_align;
						final_point = {(left_end_point.x + right_end_point.x) / 2,
							(left_end_point.y + right_end_point.y) / 2};
						midpoint = {(left_start.x + right_start.x) / 2, 110};
					}
				} else if (right_edge.size()) {
					if (right_edge_corner.size() == 1 && right_edge[right_edge_corner[0]].y > 90) {
						normal_right_corner_fsm(track_state, final_point, midpoint, left_start, right_start,
								right_edge_prev_dir);
					} else {
						align = right_align;
						final_point = right_end_point;
						midpoint = {(left_start.x + right_start.x) / 2, 110};
					}
				} else if (left_edge.size()) {
					if (left_edge_corner.size() == 1 && left_edge[left_edge_corner[0]].y > 90) {
						normal_left_corner_fsm(track_state, final_point, midpoint, left_start, right_start,
								right_edge_prev_dir);
					} else {
						align = left_align;
						final_point = left_end_point;
						midpoint = {(left_start.x + right_start.x) / 2, 110};
					}
				}
			}
			if (track_state == Crossroad) {
				if (crossroad_state == Detected) {
					if (!left_jump && !right_jump) {

						coor new_start = { 0, 0 };

						Rjump(new_start);
						right_edge_prev_dir = left;
						right_edge.push_back(new_start);
//						FindJumpStart(right_edge_prev_dir);
						RightEdge(right_edge.back(), right_edge_prev_dir, false);

						Ljump(new_start);
						left_edge_prev_dir = right;
						left_edge.push_back(new_start);
//						FindJumpStart(left_edge_prev_dir);
						LeftEdge(left_edge.back(), left_edge_prev_dir, false);

						if (left_edge_corner.size() == 1 && right_edge_corner.size() == 1) {
							align = center_align;
							crossroad_state = C_inside;
							if (right_edge[right_edge_corner[0]].x + 15 < 185)
								right_start = {right_edge[right_edge_corner[0]].x + 15, right_edge[right_edge_corner[0]].y + 20};
								else
								right_start = {184, right_edge[right_edge_corner[0]].y + 20};

							if (left_edge[left_edge_corner[0]].x - 15 > 5)
								left_start = {left_edge[left_edge_corner[0]].x - 15, left_edge[left_edge_corner[0]].y + 20};
								else
								left_start = {6, left_edge[left_edge_corner[0]].y + 20};
							final_point = {
								(left_edge[left_edge_corner[0]].x + right_edge[right_edge_corner[0]].x) / 2,
								(left_edge[left_edge_corner[0]].y + right_edge[right_edge_corner[0]].y) / 2};
						} else if (left_edge_corner.size() == 1) {
							align = left_align;
							crossroad_state = L_inside;
							if(left_edge[left_edge_corner[0]].x - 15 > 5)
							left_start = {left_edge[left_edge_corner[0]].x - 15, left_edge[left_edge_corner[0]].y + 20};
							else
							left_start = {6, left_edge[left_edge_corner[0]].y + 20};
							final_point = left_edge[left_edge_corner[0]];
						} else if (right_edge_corner.size() == 1) {
							align = right_align;
							crossroad_state = R_inside;
							if(right_edge[right_edge_corner[0]].x + 15<185)
							right_start = {right_edge[right_edge_corner[0]].x + 15, right_edge[right_edge_corner[0]].y + 20};
							else
							right_start = {184, right_edge[right_edge_corner[0]].y + 20};
							final_point = right_edge[right_edge_corner[0]];
						}
					} else if (left_jump) {
						align = left_align;
						crossroad_state = L_inside;
						if(left_edge[left_edge_corner[0]].x - 15 > 5)
						left_start = {left_edge[left_edge_corner[0]].x - 15, left_edge[left_edge_corner[0]].y + 20};
						else
						left_start = {6, left_edge[left_edge_corner[0]].y + 20};
						final_point = left_edge[left_edge_corner[0]];
					} else if (right_jump) {
						align = right_align;
						crossroad_state = R_inside;
						if(right_edge[right_edge_corner[0]].x + 15<185)
						right_start = {right_edge[right_edge_corner[0]].x + 15, right_edge[right_edge_corner[0]].y + 20};
						else
						right_start = {184, right_edge[right_edge_corner[0]].y + 20};
						final_point = right_edge[right_edge_corner[0]];
					}
				} else if (crossroad_state == C_inside) {
					right_jump = true;
					while (SobelEdgeDetection(right_start.x, right_start.y) < edge_threshold && right_start.y > 10)
					right_start.y--;
					right_edge_prev_dir = left;
					right_edge.push_back(right_start);
//					FindJumpStart(right_edge_prev_dir);
					RightEdge(right_edge.back(), right_edge_prev_dir, false);

					left_jump = true;
					while (SobelEdgeDetection(left_start.x, left_start.y) < edge_threshold && left_start.y > 10)
					left_start.y--;
					left_edge_prev_dir = right;
					left_edge.push_back(left_start);
//					FindJumpStart(left_edge_prev_dir);
					LeftEdge(left_edge.back(), left_edge_prev_dir, false);

					align = center_align;
					if(right_edge[right_edge_corner[0]].x + 15<185)
					right_start = {right_edge[right_edge_corner[0]].x + 15, right_edge[right_edge_corner[0]].y + 20};
					else
					right_start = {184, right_edge[right_edge_corner[0]].y + 20};

					if(left_edge[left_edge_corner[0]].x - 15 > 5)
					left_start = {left_edge[left_edge_corner[0]].x - 15, left_edge[left_edge_corner[0]].y + 20};
					else
					left_start = {6, left_edge[left_edge_corner[0]].y + 20};
					final_point = {(left_edge[left_edge_corner[0]].x + right_edge[right_edge_corner[0]].x) / 2,
						(left_edge[left_edge_corner[0]].y + right_edge[right_edge_corner[0]].y) / 2};

					if (right_start.y > 110 || left_start.y > 110) {
						crossroad_state = Outside;
						midpoint = {(left_edge.back().x + right_edge.back().x) / 2,
							((left_edge.back().y + right_edge.back().y) / 2) - 10};
					}
				} else if (crossroad_state == L_inside) {
					left_jump = true;
					while (SobelEdgeDetection(left_start.x, left_start.y) < edge_threshold && left_start.y > 10)
					left_start.y--;
					left_edge_prev_dir = right;
					left_edge.push_back(left_start);
//					FindJumpStart(left_edge_prev_dir);
					LeftEdge(left_edge.back(), left_edge_prev_dir, false);

					align = left_align;
					if(left_edge[left_edge_corner[0]].x - 15 > 5)
					left_start = {left_edge[left_edge_corner[0]].x - 15, left_edge[left_edge_corner[0]].y + 20};
					else
					left_start = {6, left_edge[left_edge_corner[0]].y + 20};
					final_point = left_edge[left_edge_corner[0]];

					if (left_start.y > 110) {
						crossroad_state = Outside;
						if(final_point.x+20<185)
						midpoint = {final_point.x + 20, final_point.y - 10};
						else
						midpoint = {184, final_point.y - 10};
					}
				} else if (crossroad_state == R_inside) {
					right_jump = true;
					while (SobelEdgeDetection(right_start.x, right_start.y) < edge_threshold && right_start.y > 10)
					right_start.y--;
					right_edge_prev_dir = left;
					right_edge.push_back(right_start);
//					FindJumpStart(right_edge_prev_dir);
					RightEdge(right_edge.back(), right_edge_prev_dir, false);

					align = right_align;
					if(right_edge[right_edge_corner[0]].x + 15<185)
					right_start = {right_edge[right_edge_corner[0]].x + 15, right_edge[right_edge_corner[0]].y + 20};
					else
					right_start = {184, right_edge[right_edge_corner[0]].y + 20};
					final_point = right_edge[right_edge_corner[0]];

					if (right_start.y > 110) {
						crossroad_state = Outside;
						if(final_point.x-20 > 5)
						midpoint = {final_point.x - 20, final_point.y - 10};
						else
						midpoint = {6, final_point.y - 10};
					}
				} else if (crossroad_state == Outside) {
					left_edge_prev_dir = down;
					right_edge_prev_dir = down;
					left_jump = true;
					right_jump = true;
					if (left_start_point(midpoint, left_start, edge_threshold))
					RightEdge(left_start, right_edge_prev_dir, false);
					if (right_start_point(midpoint, right_start, edge_threshold))
					LeftEdge(right_start, left_edge_prev_dir, false);

					if (midpoint.y < 106) {
						if (right_edge_corner.size() && left_edge_corner.size()) {
							align = center_align;
							midpoint = {(right_edge[right_edge_corner[0]].x + left_edge[left_edge_corner[0]].x) / 2,
								(right_edge[right_edge_corner[0]].y + left_edge[left_edge_corner[0]].y) / 2};
							if(left_edge[left_edge_corner[0]].y>110 || right_edge[right_edge_corner[0]].y>110)
							final_point = {(right_edge[0].x + left_edge[0].x) / 2, (right_edge[0].y + left_edge[0].y) / 2};
							else
							final_point = midpoint;
							midpoint.y -= 10;
						} else if (left_edge_corner.size()) {
							align = right_align;
							midpoint = {left_edge[left_edge_corner[0]].x - 40, left_edge[left_edge_corner[0]].y - 10};
							if(left_edge[left_edge_corner[0]].y<110)
							final_point = left_edge[left_edge_corner[0]];
							else
							final_point = left_edge[0];

						} else if (right_edge_corner.size()) {
							align = left_align;
							midpoint = {right_edge[right_edge_corner[0]].x + 40, right_edge[right_edge_corner[0]].y - 10};
							if(left_edge[left_edge_corner[0]].y<110)
							final_point = right_edge[right_edge_corner[0]];
							else
							final_point = right_edge[0];
						} else {
							align = center_align;
							final_point = {(right_edge[0].x + left_edge[0].x) / 2,
								((right_edge[0].y + left_edge[0].y) / 2)};
							midpoint = {(right_edge.back().x + left_edge.back().x) / 2,
								((right_edge.back().y + left_edge.back().y) / 2 - 10)};
						}
					} else {
						left_edge_prev_dir = up;
						right_edge_prev_dir = up;
						RightEdge(right_start, right_edge_prev_dir, false);
						LeftEdge(left_start, left_edge_prev_dir, false);
						track_state = Normal;
						crossroad_state = Detected;
						midpoint.y = 110;
					}
				}
			}
			if (track_state == RightLoop) {
				//follow the right edge when Entering
				//right align
				if (loop_state == Entering) {
					if (prev_track_state == Normal) {
						for (int i = right_edge.size() - 1; i > 0; i--) {
							if (right_edge[i].x < leftmostP.x && right_edge[i].y > 30)
								leftmostP = right_edge[i];
						}

						final_point = {leftmostP.x,leftmostP.y};
						align = right_align;
						midpoint = {leftmostP.x-10,leftmostP.y};

					} else if (prev_track_state == RightLoop) {
						left_edge_prev_dir = down;
						right_edge_prev_dir = down;
						if (left_start_point( {midpoint.x,90}, left_start, edge_threshold)) {
							//as the direction is down, use rightedge function
							//and the left edge will be stored in right_edge
							RightEdge(left_start, left_edge_prev_dir, false);
						}
						if (right_start_point(midpoint, right_start, edge_threshold)) {
							//opposite as well
							LeftLoopEdgeR(right_start, right_edge_prev_dir, leftmostP, false);
						}
						midpoint = {leftmostP.x-10,leftmostP.y};

						if (right_edge_corner.size() == 1 && right_edge[right_edge_corner[0]].y > 100) {
							loop_state = In;
							midpoint = {(left_start.x+right_start.x)/2, 90};
						}
						else {
							right_edge_prev_dir = up;
							left_end_point_found=false;
							right_end_point_found=false;
							left_target_found=false;
							right_target_found=false;
							coor tempmidpoint = {leftmostP.x-10,leftmostP.y};
							if(leftmostP.y > 115) {
								tempmidpoint.y = 110;
							}

							if (right_start_point(tempmidpoint, right_start, edge_threshold)) {
								RightLoopEdgeR(right_start, right_edge_prev_dir, leftmostP,false);
							}
							midpoint = {leftmostP.x-10,leftmostP.y};
							align = right_align;
							if(right_end_point_found)
							final_point = right_end_point;							//right_edge.back();
							else if(right_edge.size()) {
								final_point = right_edge.back();
							}
							final_point = {final_point.x,final_point.y-10};
//							if(FindRightEndPoint(leftmostP.x,leftmostP.y)) {
//								align = right_align;
//								final_point = {leftmostP.x,leftmostP.y};
//							}
//							else {
//								right_edge_prev_dir = up;
//								left_end_point_found=false;
//								right_end_point_found=false;
//								left_target_found=false;
//								right_target_found=false;
//								coor tempmidpoint = {leftmostP.x-10,leftmostP.y};
//								if(leftmostP.y > 115) {
//									tempmidpoint.y = 110;
//								}
//
//								if (right_start_point(tempmidpoint, right_start, edge_threshold)) {
//									RightLoopEdgeR(right_start, right_edge_prev_dir, leftmostP,false);
//								}
//								midpoint = {leftmostP.x-10,leftmostP.y};
//								align = right_align;
//								if(right_end_point_found)
//								final_point = right_end_point;							//right_edge.back();
//								else if(right_edge.size()) {
//									final_point = right_edge.back();
//								}
//								final_point = {final_point.x,final_point.y-10};
//							}
						}

					}
				}
				//follow the normal way when In
				if(loop_state == In) {
					left_edge_prev_dir = up;
					right_edge_prev_dir = up;
					if(left_start_point(midpoint,left_start,edge_threshold))
					LeftEdge(left_start,left_edge_prev_dir,false);
					if(right_start_point( {midpoint.x,115},right_start,edge_threshold))
					RightEdge(right_start,right_edge_prev_dir,false);
					if(left_edge_corner.size()==1 && left_edge[left_edge_corner[0]].y<90 ) {
						midpoint = {(left_start.x+right_start.x)/2,115};
					}
					else if(midpoint.y!=115) {
						midpoint = {(left_start.x+right_start.x)/2,90};
					}
					else {
						midpoint = {(left_start.x+right_start.x)/2,115};
					}

					//&& left_edge[left_edge_corner[0]].y>100
					if(left_edge_corner.size() == 1 && right_edge_corner.size() == 0 && left_edge[left_edge_corner[0]].y>90) {
						loop_state = Leaving;
						midpoint = {(left_start.x+right_start.x)/2,90};
					}
					else {
						if(left_edge.size() && right_edge.size()) {
							align = center_align;

							final_point = {(left_end_point.x + right_end_point.x)/2,
								(left_end_point.y + right_end_point.y)/2};
						}
						else if(left_edge.size()) {
							align = left_align;
							if(left_end_point_found) {
								final_point = left_end_point;
							}
							else {
								final_point = left_edge.back();
							}
						}
						else if(right_edge.size()) {
							align = right_align;
							if(left_end_point_found) {
								final_point = right_end_point;
							}
							else {
								final_point = right_edge.back();
							}
						}

					}

				}
				//follow the right edges when Leaving
				if(loop_state == Leaving) {
					left_edge_prev_dir = up;
					right_edge_prev_dir = up;
					if(left_start_point(midpoint,left_start,edge_threshold))
					LeftEdge(left_start,left_edge_prev_dir,false);
					std::pair<float,float> temp_target=target;
					left_target_found=false;
					right_target_found=false;
					if(right_start_point( {midpoint.x,115},right_start,edge_threshold))
					RightEdge(right_start,right_edge_prev_dir,false);
					midpoint = {(left_start.x+right_start.x)/2,90};
					if(left_edge.size()>80 && left_edge_corner.size()==0&&std::abs(temp_target.first-target.first)<100&&std::abs(temp_target.second-target.second)<100)
					loop_state=Finished;
//					if(left_edge.size()>80 && left_edge_corner.size()==0) {							// && ((img2world[left_edge[10].x][left_edge[10].y][0]-img2world[left_edge[0].x][left_edge[0].y][0])<50)) {
//						loop_state = Finished;
//					}
					else {
						//set control
						align = right_align;
						if(right_end_point_found)
						final_point = right_end_point;//right_edge.back();
						else if(right_edge.size())
						final_point = right_edge.back();
					}
				}
				//follow the left edges when Finished
				if(loop_state == Finished) {
					//increase the midpoint height for detecting the corner?
					midpoint = {midpoint.x,90};
					left_edge_prev_dir = up;
					right_edge_prev_dir = down;
					std::vector<coor> temp_edge;
					std::vector<int> temp_edge_corner;
					if(left_start_point( midpoint,left_start,edge_threshold)) {
						LeftEdge(left_start,left_edge_prev_dir,false);
						temp_edge = left_edge;
						temp_edge_corner = left_edge_corner;
					}
					if(right_start_point(midpoint,right_start,edge_threshold))
					LeftEdge(right_start,right_edge_prev_dir,false);
					else {

						left_edge_corner.clear();
						left_edge.clear();
					}
					right_edge = left_edge;
					right_edge_corner = left_edge_corner;
					left_edge = temp_edge;
					left_edge_corner = temp_edge_corner;
					midpoint = {(left_start.x+right_start.x)/2,(left_start.y+right_start.y)/2};
					//
					if(right_edge_corner.size()==1 && right_edge[right_edge_corner[0]].y>100) {
						track_state = Normal;
						loop_state = Entering;
						midpoint = {midpoint.x,115};
					}
					else {
						//set control
						final_point = left_end_point;
						align = left_align;

					}
				}
			}
			if (track_state == LeftLoop) {
				//follow the right edge when Entering
				if (loop_state == Entering) {
					if (prev_track_state == Normal) {
						for (int i = left_edge.size() - 1; i > 0; i--) {
							if (left_edge[i].x > rightmostP.x && left_edge[i].y > 30)
								rightmostP = left_edge[i];
						}
						final_point = {rightmostP.x,rightmostP.y};
						align = left_align;
						midpoint = {rightmostP.x+10,rightmostP.y};

					} else if (prev_track_state == LeftLoop) {
						right_edge_prev_dir = down;
						left_edge_prev_dir = down;
						if (right_start_point( {midpoint.x,90}, right_start, edge_threshold)) {
							//as the direction is down, use rightedge function
							//and the left edge will be stored in right_edge
							LeftEdge(right_start, right_edge_prev_dir, false);
						}
						if (left_start_point(midpoint, left_start, edge_threshold)) {
							//opposite as well
							RightLoopEdgeL(left_start, left_edge_prev_dir, rightmostP, false);
						}
						midpoint = {rightmostP.x+10,rightmostP.y};

						if (left_edge_corner.size() == 1 && left_edge[left_edge_corner[0]].y>100) {
							loop_state = In;
							midpoint = {(left_start.x+right_start.x)/2,90};
						}
						else {
							left_edge_prev_dir = up;
							left_end_point_found=false;
							right_end_point_found=false;
							left_target_found=false;
							right_target_found=false;
							coor tempmidpoint = {rightmostP.x+10,rightmostP.y};
							if(rightmostP.y>115) {
								tempmidpoint.y = 110;
							}
							if (left_start_point(tempmidpoint, left_start, edge_threshold)) {
								LeftLoopEdgeL(left_start, left_edge_prev_dir, rightmostP, false);
							}
							align = left_align;
							midpoint = {rightmostP.x+10,rightmostP.y};
							if(left_end_point_found)
							final_point = left_end_point;							//right_edge.back();
							else if(left_edge.size())
							final_point = left_edge.back();
							final_point = {final_point.x,final_point.y-10};
//							if(FindLeftEndPoint(rightmostP.x,rightmostP.y)) {
//								align = left_align;
//								final_point = {rightmostP.x,rightmostP.y};
//							}
//							else {
////								coor tempmidpoint = {midpoint.x,115};
////								if (left_start_point(tempmidpoint, left_start, edge_threshold)){
////									left_edge_prev_dir = up;
////									LeftEdge(left_start, left_edge_prev_dir, false);
////									//midpoint.x = right_start.x-20;
////									align = left_align;
////									if(left_end_point_found)
////										final_point = left_end_point;
////									else if(left_edge.size())
////										final_point = left_edge.back();
////
////									final_point = {final_point.x,final_point.y-10};
////									//set midpoint
////									if(rightmostP.y>115){
////										midpoint = {left_start.x+20,115};
////									}
////								}
////								else{
//								left_edge_prev_dir = up;
//								left_end_point_found=false;
//								right_end_point_found=false;
//								left_target_found=false;
//								right_target_found=false;
//								coor tempmidpoint = {rightmostP.x+10,rightmostP.y};
//								if(rightmostP.y>115) {
//									tempmidpoint.y = 110;
//								}
//								if (left_start_point(tempmidpoint, left_start, edge_threshold)) {
//									LeftLoopEdgeL(left_start, left_edge_prev_dir, rightmostP, false);
//								}
//								align = left_align;
//								midpoint = {rightmostP.x+10,rightmostP.y};
//								if(left_end_point_found)
//								final_point = left_end_point;							//right_edge.back();
//								else if(left_edge.size())
//								final_point = left_edge.back();
//								final_point = {final_point.x,final_point.y-10};
//
//							}
						}

					}
				}
				//follow the normal way when In
				if(loop_state == In) {
					left_edge_prev_dir = up;
					right_edge_prev_dir = up;
					if(left_start_point( {midpoint.x,115},left_start,edge_threshold))
					LeftEdge(left_start,left_edge_prev_dir,false);
					if(right_start_point(midpoint,right_start,edge_threshold))
					RightEdge(right_start,right_edge_prev_dir,false);

					if(right_edge_corner.size()==1 && right_edge[right_edge_corner[0]].y<90 ) {
						midpoint = {(left_start.x+right_start.x)/2,115};
					}
					else if(midpoint.y !=115) {
						midpoint = {(left_start.x+right_start.x)/2,90};
					}
					else {
						midpoint = {(left_start.x+right_start.x)/2,115};
					}

					if(right_edge_corner.size() == 1 && left_edge_corner.size() == 0 && right_edge[right_edge_corner[0]].y>90) {
						loop_state = Leaving;
						midpoint = {(left_start.x+right_start.x)/2,90};
					}
					else {
						if(left_edge.size() && right_edge.size()) {
							align = center_align;

							final_point = {(left_end_point.x + right_end_point.x)/2,
								(left_end_point.y + right_end_point.y)/2};
						}
						else if(left_edge.size()) {
							align = left_align;
							if(left_end_point_found) {
								final_point = left_end_point;
							}
							else {
								final_point = left_edge.back();
							}
						}
						else if(right_edge.size()) {
							align = right_align;
							if(left_end_point_found) {
								final_point = right_end_point;
							}
							else {
								final_point = right_edge.back();
							}
						}

					}
				}
				//follow the right edges when Leaving
				if(loop_state == Leaving) {
					left_edge_prev_dir = up;
					right_edge_prev_dir = up;
					if(left_start_point( {midpoint.x,115},left_start,edge_threshold))
					LeftEdge(left_start,left_edge_prev_dir,false);
					std::pair<float, float> temp_target=target;
					left_target_found=false;
					right_target_found=false;
					if(right_start_point(midpoint,right_start,edge_threshold))
					RightEdge(right_start,right_edge_prev_dir,false);
					midpoint = {(left_start.x+right_start.x)/2,90};
					if(right_edge.size()>80 && right_edge_corner.size()==0&&std::abs(temp_target.first-target.first)<100&&std::abs(temp_target.second-target.second)<100)
					loop_state=Finished;
//					if(right_edge.size()>80 && right_edge_corner.size()==0) {							// && ((img2world[right_edge[10].x][right_edge[10].y][0] - img2world[right_edge[0].x][right_edge[0].y][0]) > -50)) {
//						loop_state = Finished;
//					}
					else {
						//set control
						align = left_align;
						if(left_end_point_found)
						final_point = left_end_point;//right_edge.back();
						else if(left_edge.size())
						final_point = left_edge.back();
					}
					target=temp_target;
				}
				//follow the left edges when Finished
				if(loop_state == Finished) {
					//increase the midpoint height for detecting the corner?
					midpoint = {midpoint.x,90};
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
					else {
						right_edge_corner.clear();
						right_edge.clear();

					}
					left_edge = right_edge;
					left_edge_corner = right_edge_corner;
					right_edge = temp_edge;
					right_edge_corner = temp_edge_corner;
					midpoint = {(left_start.x+right_start.x)/2,(left_start.y+right_start.y)/2};
					if(left_edge_corner.size()==1 && left_edge[left_edge_corner[0]].y>100) {
						track_state = Normal;
						loop_state = Entering;
						midpoint = {midpoint.x,115};
					}
					else {
						//set control
						final_point = right_end_point;
						align = right_align;

					}
				}
			}
			if (debug) {
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
				sprintf(buffer, "t %d l %d %d %d \n %d %d", track_state, loop_state, right_edge.size(), left_edge.size(), final_point.x, final_point.y);
				lcd->SetRegion(libsc::Lcd::Rect(0, 0, 160, 40));
				writerP->WriteString(buffer);
			}
			//			lcd->Clear();
			//			if (left_end_point_found) {
			//				lcd->SetRegion(libsc::St7735r::Rect(left_end_point.x, left_end_point.y, 1, 1));
			//				lcd->FillColor(lcd->kGreen);
			//			}
			//			if (right_end_point_found) {
			//				lcd->SetRegion(libsc::St7735r::Rect(right_end_point.x, right_end_point.y, 1, 1));
			//				lcd->FillColor(lcd->kRed);
			//			}
			degree_1 = degree_2;
			degree_2 = degree_3;
			int servo_angle;			// = prev_angle;
			coor destination;
			if (track_state == Tstate::Normal || (track_state == Tstate::LeftLoop && loop_state == Lstate::In) || (track_state == Tstate::RightLoop && loop_state == Lstate::In)) {
				if (left_edge_corner.size() && right_edge_corner.size() && !FindLeftEndPoint(left_edge[left_edge_corner.front()].x, left_edge[left_edge_corner.front()].y) && !FindRightEndPoint(right_edge[right_edge_corner.front()].x, right_edge[right_edge_corner.front()].y)) {
					destination = final_point;
					servo_angle = 1120 + std::atan(1.0 * (img2world[destination.x][destination.y][0] - img2world[97][115][0]) / (img2world[destination.x][destination.y][1] - img2world[97][115][1])) * 1800 / 3.14;
				} else if (left_edge_corner.size() && !FindLeftEndPoint(left_edge[left_edge_corner.front()].x, left_edge[left_edge_corner.front()].y)) {
					destination = left_edge[left_edge_corner.front()];
					servo_angle = 1120 + std::atan(1.0 * (img2world[destination.x][destination.y][0] - img2world[47][115][0]) / (img2world[destination.x][destination.y][1] - img2world[47][115][1])) * 1800 / 3.14;
				} else if (right_edge_corner.size() && !FindRightEndPoint(right_edge[right_edge_corner.front()].x, right_edge[right_edge_corner.front()].y)) {
					destination = right_edge[right_edge_corner.front()];
					servo_angle = 1120 + std::atan(1.0 * (img2world[destination.x][destination.y][0] - img2world[148][115][0]) / (img2world[destination.x][destination.y][1] - img2world[148][115][1])) * 1800 / 3.14;
				} else if (left_target_found || right_target_found) {
					servo_angle = 1120 + std::atan(1.0 * (target.first - img2world[97][115][0]) / (target.second - img2world[97][115][1])) * 1800 / 3.14;
				} else if (left_edge.size()) {
					destination = left_edge.back();
					servo_angle = 1120 + std::atan(1.0 * (img2world[destination.x][destination.y][0] - img2world[47][115][0]) / (img2world[destination.x][destination.y][1] - img2world[47][115][1])) * 1800 / 3.14;
				} else if (right_edge.size()) {
					destination = right_edge.back();
					servo_angle = 1120 + std::atan(1.0 * (img2world[destination.x][destination.y][0] - img2world[148][115][0]) / (img2world[destination.x][destination.y][1] - img2world[148][115][1])) * 1800 / 3.14;
				}
//				servo_angle += servo_D * (servo_angle - servo->GetDegree());
//				switch (align) {
//				case 0:
//					if (left_edge.size()) {
//						if (left_end_point_found) {
//							destination = left_end_point;
//						} else if (left_edge_corner.size()) {
//							destination = left_edge[left_edge_corner.front()];
//						} else {
//							destination = left_edge.back();
//						}
//						servo_angle = 1120 + std::atan(1.0 * (img2world[destination.x][destination.y][0] - img2world[47][115][0]) / (img2world[destination.x][destination.y][1] - img2world[47][115][1])) * 1800 / 3.14;
//						servo_angle += servo_D * (servo_angle - prev_servo_angle);
//					}
//					break;
//				case 1:
//					if (target_found) {
//						servo_angle = 1120 + std::atan(1.0 * (target.front - img2world[97][115][0]) / (target.second - img2world[97][115][1])) * 1800 / 3.14;
//						servo_angle += servo_D * (servo_angle - prev_servo_angle);
//					}
////					if (left_end_point_found && right_end_point_found) {
////						if (left_edge.size() > right_edge.size()) {
////							destination = left_end_point;
////							servo_angle = 1120 + std::atan(1.0 * (img2world[destination.x][destination.y][0] - img2world[47][115][0]) / (img2world[destination.x][destination.y][1] - img2world[47][115][1])) * 1800 / 3.14;
////							servo_angle += servo_D * (servo_angle - prev_servo_angle);
////						} else {
////							destination = right_end_point;
////							servo_angle = 1120 + std::atan(1.0 * (img2world[destination.x][destination.y][0] - img2world[148][115][0]) / (img2world[destination.x][destination.y][1] - img2world[148][115][1])) * 1800 / 3.14;
////							servo_angle += servo_D * (servo_angle - prev_servo_angle);
////						}
////					} else if (left_end_point_found) {
////						destination = left_end_point;
////						servo_angle = 1120 + std::atan(1.0 * (img2world[destination.x][destination.y][0] - img2world[47][115][0]) / (img2world[destination.x][destination.y][1] - img2world[47][115][1])) * 1800 / 3.14;
////						servo_angle += servo_D * (servo_angle - prev_servo_angle);
////					} else if (right_end_point_found) {
////						destination = right_end_point;
////						servo_angle = 1120 + std::atan(1.0 * (img2world[destination.x][destination.y][0] - img2world[148][115][0]) / (img2world[destination.x][destination.y][1] - img2world[148][115][1])) * 1800 / 3.14;
////						servo_angle += servo_D * (servo_angle - prev_servo_angle);
////					} else if (left_edge_corner.size() || right_edge_corner.size()) {
////						//					lcd->SetRegion(libsc::Lcd::Rect(final_point.x-1,final_point.y-1,3,3));
////						//					lcd->FillColor(libsc::Lcd::kRed);
////						switch (align) {
////						case 0:
////							destination = final_point;
////							servo_angle = 1120 + std::atan(1.0 * (img2world[destination.x][destination.y][0] - img2world[47][115][0]) / (img2world[destination.x][destination.y][1] - img2world[47][115][1])) * 1800 / 3.14;
////							servo_angle += servo_D * (servo_angle - prev_servo_angle);
////							break;
////						case 1:
////							destination = final_point;
////							servo_angle = 1120 + std::atan(1.0 * (img2world[destination.x][destination.y][0] - img2world[97][115][0]) / (img2world[destination.x][destination.y][1] - img2world[97][115][1])) * 1800 / 3.14;
////							servo_angle += servo_D * (servo_angle - prev_servo_angle);
////							break;
////						case 2:
////							destination = final_point;
////							servo_angle = 1120 + std::atan(1.0 * (img2world[destination.x][destination.y][0] - img2world[148][115][0]) / (img2world[destination.x][destination.y][1] - img2world[148][115][1])) * 1800 / 3.14;
////							servo_angle += servo_D * (servo_angle - prev_servo_angle);
////							break;
////						}
////					} else if (left_edge.size() > right_edge.size()) {
////						destination = left_edge.back();
////						servo_angle = 1120 + std::atan(1.0 * (img2world[destination.x][destination.y][0] - img2world[47][115][0]) / (img2world[destination.x][destination.y][1] - img2world[47][115][1])) * 1800 / 3.14;
////						servo_angle += servo_D * (servo_angle - prev_servo_angle);
////					} else if (right_edge.size()) {
////						destination = right_edge.back();
////						servo_angle = 1120 + std::atan(1.0 * (img2world[destination.x][destination.y][0] - img2world[148][115][0]) / (img2world[destination.x][destination.y][1] - img2world[148][115][1])) * 1800 / 3.14;
////						servo_angle += servo_D * (servo_angle - prev_servo_angle);
////					}
//					break;
//				case 2:
//					if (right_edge.size()) {
//						if (right_end_point_found) {
//							destination = right_end_point;
//						} else if (right_edge_corner.size()) {
//							destination = right_edge[right_edge_corner.front()];
//						} else {
//							destination = right_edge.back();
//						}
//						servo_angle = 1120 + std::atan(1.0 * (img2world[destination.x][destination.y][0] - img2world[148][115][0]) / (img2world[destination.x][destination.y][1] - img2world[148][115][1])) * 1800 / 3.14;
//						servo_angle += servo_D * (servo_angle - prev_servo_angle);
//						break;
//					}
//				}
			} else if (track_state == Tstate::Crossroad) {
				switch (align) {
				case 0:
					servo_angle = 1120 + std::atan(1.0 * (img2world[final_point.x][final_point.y][0] - img2world[47][115][0]) / (img2world[final_point.x][final_point.y][1] - img2world[47][115][1])) * 1800 / 3.14;
//					servo_angle += servo_D * (servo_angle - prev_servo_angle);
					break;
				case 1:
					servo_angle = 1120 + std::atan(1.0 * (img2world[final_point.x][final_point.y][0] - img2world[97][115][0]) / (img2world[final_point.x][final_point.y][1] - img2world[97][115][1])) * 1800 / 3.14;
//					servo_angle += servo_D * (servo_angle - prev_servo_angle);
					break;
				case 2:
					servo_angle = 1120 + std::atan(1.0 * (img2world[final_point.x][final_point.y][0] - img2world[148][115][0]) / (img2world[final_point.x][final_point.y][1] - img2world[148][115][1])) * 1800 / 3.14;
//					servo_angle += servo_D * (servo_angle - prev_servo_angle);
					break;
				}
			} else if (track_state == Tstate::RightLoop) {
				servo_angle = 1120 + std::atan(1.0 * (target.first - img2world[97][115][0]) / (target.second - img2world[97][115][1])) * 1800 / 3.14;

//				switch (loop_state) {
//				case Lstate::Entering:
//					servo_angle = 1120 + std::atan(1.0 * (target.first - img2world[97][115][0]) / (target.second - img2world[97][115][1])) * 1800 / 3.14;
////					if (!FindRightEndPoint(leftmostP.x, leftmostP.y)) {
////						destination = final_point;
////						servo_angle = 1120 + std::atan(1.0 * (img2world[destination.x][destination.y][0] - img2world[148][115][0]) / (img2world[destination.x][destination.y][1] - img2world[148][115][1])) * 1800 / 3.14;
////
////					} else {
////						servo_angle = 1120 + std::atan(1.0 * (target.first - img2world[97][115][0]) / (target.second - img2world[97][115][1])) * 1800 / 3.14;
////					}
////					servo_angle += servo_D * (servo_angle - prev_servo_angle);
//
//					break;
//				case Lstate::Leaving:
//					destination = final_point;
//					servo_angle = 1120 + std::atan(1.0 * (img2world[destination.x][destination.y][0] - img2world[148][115][0]) / (img2world[destination.x][destination.y][1] - img2world[148][115][1])) * 1800 / 3.14;
////					servo_angle += servo_D * (servo_angle - prev_servo_angle);
//
//					break;
//				case Lstate::Finished:
//					destination = left_end_point_found ? left_end_point : left_edge.back();
//					servo_angle = 1120 + std::atan(1.0 * (img2world[destination.x][destination.y][0] - img2world[47][115][0]) / (img2world[destination.x][destination.y][1] - img2world[47][115][1])) * 1800 / 3.14;
////					servo_angle += servo_D * (servo_angle - prev_servo_angle);
//
//					break;
//				}
			} else if (track_state == Tstate::LeftLoop) {
				servo_angle = 1120 + std::atan(1.0 * (target.first - img2world[97][115][0]) / (target.second - img2world[97][115][1])) * 1800 / 3.14;

//				switch (loop_state) {
//				case Lstate::Entering:
////					if (!FindLeftEndPoint(rightmostP.x, rightmostP.y)) {
////						destination = final_point;
////						servo_angle = 1120 + std::atan(1.0 * (img2world[destination.x][destination.y][0] - img2world[47][115][0]) / (img2world[destination.x][destination.y][1] - img2world[47][115][1])) * 1800 / 3.14;
////					} else {
////						servo_angle = 1120 + std::atan(1.0 * (target.first - img2world[97][115][0]) / (target.second - img2world[97][115][1])) * 1800 / 3.14;
////					}
//				servo_angle = 1120 + std::atan(1.0 * (target.first - img2world[97][115][0]) / (target.second - img2world[97][115][1])) * 1800 / 3.14;
////					servo_angle += servo_D * (servo_angle - prev_servo_angle);
//
//					break;
//				case Lstate::Leaving:
//					destination = final_point;
//					servo_angle = 1120 + std::atan(1.0 * (img2world[destination.x][destination.y][0] - img2world[47][115][0]) / (img2world[destination.x][destination.y][1] - img2world[47][115][1])) * 1800 / 3.14;
////					servo_angle += servo_D * (servo_angle - prev_servo_angle);
//
//					break;
//				case Lstate::Finished:
//					destination = right_end_point_found ? right_end_point : right_edge.back();
//					servo_angle = 1120 + std::atan(1.0 * (img2world[destination.x][destination.y][0] - img2world[148][115][0]) / (img2world[destination.x][destination.y][1] - img2world[148][115][1])) * 1800 / 3.14;
////					servo_angle += servo_D * (servo_angle - prev_servo_angle);
//
//					break;
//				}

			}
			degree_3 = servo_angle - prev_angle;
			prev_angle = servo_angle;
			servo_angle += servo_D * ((degree_1 + degree_2 + degree_3) / 3.0);
			servo->SetDegree(libutil::Clamp(700, servo_angle, 1500));

			if (debug) {
				lcd->SetRegion(libsc::St7735r::Lcd::Rect(destination.x - 2, destination.y - 2, 5, 5));
				lcd->FillColor(lcd->kGreen);
			}

//			char buffer[100] = { };
//			sprintf(buffer, "%d\n", libsc::System::Time() - time_now);
//			bt->SendStr(buffer);
			prev_track_state = track_state;
		}

	}
}

