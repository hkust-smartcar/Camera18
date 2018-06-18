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

enum Tstate {
	Normal, Crossroad, RightLoop, LeftLoop
};

enum Cstate {
	Detected, Inside,
};

enum Lstate {
	Entering, In, Leaving, Finished,
};

enum Align{
	left_align,
	center_align,
	right_align
};

const Byte* buffer;

inline Byte GetPoint(uint8_t x, uint8_t y) {
	return buffer[x + y * 189];
}

inline int16_t SobelEdgeDetection(uint8_t x, uint8_t y) {
	return std::abs(-GetPoint((x) - 1, (y) - 1) - 2 * GetPoint((x) - 1, (y)) - GetPoint((x) - 1, (y) + 1) + GetPoint((x) + 1, (y) - 1) + 2 * GetPoint((x) + 1, (y)) + GetPoint((x) + 1, (y) + 1)) + std::abs(-GetPoint((x) - 1, (y) - 1) - 2 * GetPoint((x), (y) - 1) - GetPoint((x) + 1, (y) - 1) + GetPoint((x) - 1, (y) + 1) + 2 * GetPoint((x), (y) + 1) + GetPoint((x) + 1, (y) + 1));
}

float search_distance;
float search_m;
float search_c;
float search_slope;
//img2world[91][119] is the bottom mid point of the image, may need to tune later
float search_origin_x = img2world[91][119][0];
float search_origin_y = img2world[91][119][1];

coor left_end_point;
coor right_end_point;
bool left_end_point_found;
bool right_end_point_found;

bool left_jump;
bool right_jump;

inline void InitSearchDistance(float mpu_angle) {
	mpu_angle *= 0.00008726646;	//angle = angular_speed * 5ms * PI / 180
	search_m = -std::tan(mpu_angle);
	search_c = (search_distance * std::cos(mpu_angle) - search_m * (search_distance * std::sin(mpu_angle) - search_origin_x)) - search_origin_y;
	left_end_point_found = false;
	right_end_point_found = false;
}

inline bool FindEndPoint(int x, int y) {
	return (img2world[x][y][1] - search_origin_y) < (search_m * (img2world[x][y][0] - search_origin_x) + search_c);
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

bool FindLeftEdge(int& edge_prev_dir) {
	int size = left_edge.size();
	if (size > 120 || !size)
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
			if (size > 3 && SobelEdgeDetection(x - 1, y + 1) < edge_threshold) {
				edge_coor.x = x - 1;
				edge_coor.y = y + 1;
				left_edge.push_back(edge_coor);
				return true;
			}
			break;
		case down:
			if (size > 3 && SobelEdgeDetection(x, y + 1) < edge_threshold) {
				edge_coor.y = y + 1;
				left_edge.push_back(edge_coor);
				return true;
			}
			break;
		case down_right:
			if (size > 3 && SobelEdgeDetection(x + 1, y + 1) < edge_threshold) {
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
	if (size > 120 || !size)
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
			if (size > 3 && SobelEdgeDetection(x - 1, y + 1) < edge_threshold) {
				edge_coor.x = x - 1;
				edge_coor.y = y + 1;
				right_edge.push_back(edge_coor);
				return true;
			}
			break;
		case down:
			if (size > 3 && SobelEdgeDetection(x, y + 1) < edge_threshold) {
				edge_coor.y = y + 1;
				right_edge.push_back(edge_coor);
				return true;
			}
			break;
		case down_right:
			if (size > 3 && SobelEdgeDetection(x + 1, y + 1) < edge_threshold) {
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
	Byte center = GetPoint(x, y) - fast_threshold;
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

bool FAST4(int x, int y) {
	if (x < 5 || x > 185 || y < 5 || y > 115)
		return false;
	int black_count = 0, center = GetPoint(x, y) - fast_threshold;
	if (GetPoint(x, y + 4) < center)
		black_count++;
	if (GetPoint(x, y - 4) < center)
		black_count++;
	if (GetPoint(x + 4, y) < center)
		black_count++;
	if (GetPoint(x - 4, y) < center)
		black_count++;
	if (GetPoint(x + 1, y + 4) < center)
		black_count++;
	if (GetPoint(x - 1, y + 4) < center)
		black_count++;
	if (GetPoint(x + 1, y - 4) < center)
		black_count++;
	if (GetPoint(x - 1, y - 4) < center)
		black_count++;
	if (GetPoint(x + 4, y + 1) < center)
		black_count++;
	if (GetPoint(x + 4, y - 1) < center)
		black_count++;
	if (GetPoint(x - 4, y + 1) < center)
		black_count++;
	if (GetPoint(x - 4, y - 1) < center)
		black_count++;
	if (GetPoint(x + 2, y + 3) < center)
		black_count++;
	if (GetPoint(x - 2, y + 3) < center)
		black_count++;
	if (GetPoint(x + 2, y - 3) < center)
		black_count++;
	if (GetPoint(x - 2, y - 3) < center)
		black_count++;
	if (GetPoint(x + 3, y + 2) < center)
		black_count++;
	if (GetPoint(x + 3, y - 2) < center)
		black_count++;
	if (GetPoint(x - 3, y + 2) < center)
		black_count++;
	if (GetPoint(x - 3, y - 2) < center)
		black_count++;
	return black_count > 0 && black_count < 6;
}

bool dist_corners(coor m, coor n) {
    int x1 = img2world[m.y][m.x][0];
    int y1 = img2world[m.y][m.x][1];
    int x2 = img2world[n.y][n.x][0];
    int y2 = img2world[n.y][n.x][1];
	return ((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2) <= dist_threshold);
}

bool check_corner(coor pivot, coor m, coor n, bool direction) {
	int a2 = (pivot.x - m.x) * (pivot.x - m.x) + (pivot.y - m.y) * (pivot.y - m.y);
	int b2 = (pivot.x - n.x) * (pivot.x - n.x) + (pivot.y - n.y) * (pivot.y - n.y);
	int c2 = (m.x - n.x) * (m.x - n.x) + (m.y - n.y) * (m.y - n.y);
	int pythagoras = a2 + b2 - c2;
	if (pythagoras >= 0)
		return true;
	float value = (float) (pythagoras * pythagoras) / (4 * a2 * b2);
	if ((direction && !right_jump && value < 0.05) || (!direction && !left_jump && value < 0.05))
		return true;
	else if ((direction && right_jump && value < 0.25) || (!direction && left_jump && value < 0.25))
		return true;
	return false;
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
	bool breach = false;
	if (!append) {
		empty_left();
		left_edge.push_back(start_point);
	}
	bool guessed_corner = false;
	if (left_edge.size() && left_edge[left_edge.size() - 1].y != start_point.y && left_edge[left_edge.size() - 1].x != start_point.x)
		left_edge.push_back(start_point);
	else if (!left_edge.size())
		left_edge.push_back(start_point);

	while (FindLeftEdge(edge_prev_dir) && !guessed_corner && !breach) {
		if (direction == up && left_edge[left_edge.size() - 1].y > start_point.y + 20)
			breach = true;
		else if (direction == down && left_edge[left_edge.size() - 1].y < start_point.y - 20)
			breach = true;
		else if (direction == left && left_edge[left_edge.size() - 1].x > start_point.x + 20)
			breach = true;
		else if (direction == right && left_edge[left_edge.size() - 1].x < start_point.x - 20)
			breach = true;
		if (left_edge[left_edge.size() - 1].y> 30 && FAST4(left_edge[left_edge.size() - 1].x, left_edge[left_edge.size() - 1].y))
			guessed_corner = true;
	}
	if (guessed_corner) {
		int i = 0;
		for (; i < 10 && FindLeftEdge(edge_prev_dir); i++);
		if (i == 10) {
			if (left_edge.size() > 16 && check_corner(left_edge[left_edge.size() - 11], left_edge[left_edge.size() - 1], left_edge[left_edge.size() - 16],false))
				left_edge_corner.push_back(left_edge.size() - 11);
			else if (check_corner(left_edge[left_edge.size() - 11], left_edge[left_edge.size() - 1], left_edge[0],false))
				left_edge_corner.push_back(left_edge.size() - 11);
			else {
			    left_edge.erase(left_edge.end() - 11,left_edge.end() - 1);
                LeftEdge(left_edge[left_edge.size() - 1], edge_prev_dir, true);
            }
		}
	}
}

void RightEdge(coor start_point, int& edge_prev_dir, bool append) {
	int direction = edge_prev_dir;
	bool breach = false;
	if (!append) {
		empty_right();
		right_edge.push_back(start_point);
	}
	bool guessed_corner = false;
	if (right_edge.size() && right_edge[right_edge.size() - 1].y != start_point.y && right_edge[right_edge.size() - 1].x != start_point.x)
		right_edge.push_back(start_point);
	else if (!right_edge.size())
		right_edge.push_back(start_point);

	while (FindRightEdge(edge_prev_dir) && !guessed_corner && !breach) {
		if (direction == up && right_edge[right_edge.size() - 1].y > start_point.y + 20)
			breach = true;
		else if (direction == down && right_edge[right_edge.size() - 1].y < start_point.y - 20)
			breach = true;
		else if (direction == left && right_edge[right_edge.size() - 1].x > start_point.x + 20)
			breach = true;
		else if (direction == right && right_edge[right_edge.size() - 1].x < start_point.x - 20)
			breach = true;
		if (right_edge[right_edge.size() - 1].y>40 && FAST4(right_edge[right_edge.size() - 1].x, right_edge[right_edge.size() - 1].y))
			guessed_corner = true;
	}
	if (guessed_corner) {
		int i = 0;
		for (; i < 10 && FindRightEdge(edge_prev_dir); i++);
		if (i == 10) {
			if (right_edge.size() > 16 && check_corner(right_edge[right_edge.size() - 11], right_edge[right_edge.size() - 1], right_edge[right_edge.size() - 16],true))
				right_edge_corner.push_back(right_edge.size() - 11);
			else if (check_corner(right_edge[right_edge.size() - 11], right_edge[right_edge.size() - 1], right_edge[0],true))
				right_edge_corner.push_back(right_edge.size() - 11);
			else {
			    right_edge.erase(right_edge.end() - 11,right_edge.end() - 1);
                RightEdge(right_edge[right_edge.size() - 1], edge_prev_dir, true);
            }
		}
	}
}

//for left loop
void RightLoopEdgeL(coor start_point, int& edge_prev_dir, coor& rightmostP, bool append) {
	if (!append) {
		empty_right();
		right_edge.push_back(start_point);
	}
	if (right_edge.size() && right_edge[right_edge.size() - 1].y != start_point.y && right_edge[right_edge.size() - 1].x != start_point.x)
		right_edge.push_back(start_point);
	while (FindRightEdge(edge_prev_dir) && (right_edge[right_edge.size() - 1].x - start_point.x > -20)) {
		if (right_edge[right_edge.size() - 1].x > rightmostP.x)
			rightmostP = right_edge[right_edge.size() - 1];
	}
}
//for right loop
void RightLoopEdgeR(coor start_point, int& edge_prev_dir, coor& leftmostP, bool append) {
	if (!append) {
		empty_right();
		right_edge.push_back(start_point);
	}
	if (right_edge.size() && right_edge[right_edge.size() - 1].y != start_point.y && right_edge[right_edge.size() - 1].x != start_point.x)
		right_edge.push_back(start_point);
	while (FindRightEdge(edge_prev_dir) && (right_edge[right_edge.size() - 1].x - start_point.x > -20)) {
		if (right_edge[right_edge.size() - 1].x < leftmostP.x)
			leftmostP = right_edge[right_edge.size() - 1];
	}
}
//for right loop
void LeftLoopEdgeR(coor start_point, int& edge_prev_dir, coor& leftmostP, bool append) {
	if (!append)
		empty_right();
	left_edge.push_back(start_point);
	while (FindLeftEdge(edge_prev_dir) && (left_edge[left_edge.size() - 1].x - start_point.x < 20)) {
		if (left_edge[left_edge.size() - 1].x < leftmostP.x)
			leftmostP = left_edge[left_edge.size() - 1];
	}
}
//for left loop
void LeftLoopEdgeL(coor start_point, int& edge_prev_dir, coor& rightmostP, bool append) {
	if (!append)
		empty_right();
	if (left_edge.size() && left_edge[left_edge.size() - 1].y != start_point.y && left_edge[left_edge.size() - 1].x != start_point.x)
		left_edge.push_back(start_point);
	while (FindLeftEdge(edge_prev_dir) && (left_edge[left_edge.size() - 1].x - start_point.x < 20)) {
		if (left_edge[left_edge.size() - 1].x > rightmostP.x)
			rightmostP = left_edge[left_edge.size() - 1];
	}
}

bool jump(coor point1, coor point2, coor& new_start, int threshold, bool direction) { //jump from the corner to new edge. returns true if a edge is found.
	float slope = (point1.x - point2.x) / (point1.y - point2.y);
	float constant = point2.x - slope * point2.y;
	new_start.y = point2.y - 3;
	new_start.x = (int16_t) (new_start.y * slope + constant);
	while (new_start.x < width - 4 && new_start.x > 3 && new_start.y < height - 4 && new_start.y > 3 && SobelEdgeDetection(new_start.x, new_start.y) < threshold) {
		new_start.y--;
		new_start.x = (new_start.y * slope + constant);
	}
	if (new_start.x < width - 4 && new_start.x > 3 && new_start.y < height - 4 && new_start.y > 3) {
		if(direction)
			right_jump = true;
		else if(!direction)
			left_jump = true;
		return true;
	}
	return false;
}

bool right_start_point(coor midpoint, coor& right_start, int threshold) {
	right_start = midpoint;
	while (SobelEdgeDetection(right_start.x, right_start.y) < threshold && right_start.x < width - 1)
		right_start.x++;
	if (right_start.x < width - 1)
		return true;
	return false;
}

bool left_start_point(coor midpoint, coor& left_start, int threshold) {
	left_start = midpoint;
	while (SobelEdgeDetection(left_start.x, left_start.y) < threshold && left_start.x > 0)
		left_start.x--;
	if (left_start.x > 0)
		return true;
	return false;
}

void normal_left_corner_fsm(Tstate& track_state, coor& final_point, coor& midpoint, coor left_start, coor right_start) {
	align = left_align;
	coor point1;
	if (left_edge_corner[0] > 10)
		point1 = left_edge[left_edge_corner[0] - 10];
	else
		point1 = left_edge[0];
	coor new_start = { 0, 0 };
	if (jump(point1, left_edge[left_edge_corner[0] + 3], new_start, edge_threshold, false)) {
		left_edge_prev_dir = right;
		LeftEdge(new_start, left_edge_prev_dir, true);
		if (left_edge_corner.size() == 2) {
			track_state = Crossroad;
		} else {
//			track_state = LeftLoop;
		}
	} else {
		track_state = Normal;
		final_point = new_start;
		midpoint= {(left_start.x+right_start.x)/2,115};
	}
}

void normal_right_corner_fsm(Tstate& track_state, coor& final_point, coor& midpoint, coor left_start, coor right_start) {
	align = right_align;
	coor point1;
	if (right_edge_corner[0] > 10)
		point1 = right_edge[right_edge_corner[0] - 10];
	else
		point1 = right_edge[0];
	coor new_start = { 0, 0 };
	if (jump(point1, right_edge[right_edge_corner[0] + 3], new_start, edge_threshold, true)) {
		right_edge_prev_dir = left;
		RightEdge(new_start, right_edge_prev_dir, true);
		if (right_edge_corner.size() == 2) {
			track_state = Crossroad;
		} else {
//			track_state = RightLoop;
		}
	} else {
		track_state = Normal;
		final_point = new_start;
		midpoint= {(left_start.x+right_start.x)/2,115};
	}
}

bool isLeftBottomExist(coor midpoint, int threshold){
	coor left_start = {midpoint.x,115};
	while (SobelEdgeDetection(left_start.x, left_start.y) < threshold && left_start.x > 0)
		left_start.x--;
	if (left_start.x > 0)
		return true;
	return false;
}

bool isRightBottomExist(coor midpoint, int threshold){
	coor right_start = {midpoint.x,115};
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
	coor midpoint = { 92, 115 };

	coor left_start = { 0, 0 };
	coor right_start = { 0, 0 };
	coor final_point = { 0, 0 };

	while (1) {
		if (camera->IsAvailable()) {
			buffer = camera->LockBuffer(); //Use GetPoint(x,y) to get the gradient of the point
			camera->UnlockBuffer();
		}
		for (uint16_t i = 0; i < height; i++) {
			lcd->SetRegion(libsc::Lcd::Rect(0, i, 160, 1));
			lcd->FillGrayscalePixel(buffer + camera->GetW() * i+29, 160);
		}

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
				if (left_edge_corner.size() == 1 && right_edge_corner.size() == 1) { //TODO: could be corners of different features, check distance between corners to determine.
					if (dist_corners(left_edge[left_edge_corner[0]], right_edge[right_edge_corner[0]]) && ((right_edge[right_edge_corner[0]].y>40) || left_edge[left_edge_corner[0]].y>40)) {
						track_state = Crossroad;
					} else if (left_edge[left_edge_corner[0]].y > right_edge[right_edge_corner[0]].y) {
					    right_edge_corner.clear();
						normal_left_corner_fsm(track_state, final_point, midpoint, left_start, right_start);
					} else if (left_edge[left_edge_corner[0]].y < right_edge[right_edge_corner[0]].y) {
					    left_edge_corner.clear();
						normal_right_corner_fsm(track_state, final_point, midpoint, left_start, right_start);
					}
				} else if (left_edge_corner.size() == 1 && right_edge_corner.size() == 0) { //TODO: corner distance requirement needs to be rethought
					normal_left_corner_fsm(track_state, final_point, midpoint, left_start, right_start);
				} else if (left_edge_corner.size() == 0 && right_edge_corner.size() == 1) { //TODO: corner distance requirement needs to be rethought
					normal_right_corner_fsm(track_state, final_point, midpoint, left_start, right_start);
				} else {
					align = center_align;
					track_state = Normal;
//					final_point = {(left_edge[left_edge.size()-1].x + right_edge[right_edge.size()-1].x)/2,
//						(left_edge[left_edge.size()-1].y + right_edge[right_edge.size()-1].y)/2};
					midpoint= {(left_start.x+right_start.x)/2,115};
				}
			}
			else if(right_edge.size()) {
				if(right_edge_corner.size() == 1) {
					normal_right_corner_fsm(track_state,final_point,midpoint,left_start,right_start);
				}
				else if(right_edge_corner.size() == 0) {
				    align = right_align;
					track_state = Normal;
//					final_point = right_edge[right_edge.size()-1];
					midpoint= {(left_start.x+right_start.x)/2,115};
				}
			}
			else if(left_edge.size()) {
				if(left_edge_corner.size() == 1) {
					normal_left_corner_fsm(track_state,final_point,midpoint,left_start,right_start);
				}
				else if(left_edge_corner.size() == 0) {
				    align = left_align;
					track_state = Normal;
//					final_point = left_edge[left_edge.size()-1];
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
				    align = center_align;
					if (left_edge[left_edge_corner[0]].y > 70 || right_edge[right_edge_corner[0]].y > 70) {
						crossroad_state = Inside;
//						if (prev_track_state != Normal) {
							coor point1;
							if (right_edge_corner[0] > 10)
								point1 = right_edge[right_edge_corner[0] - 10];
							else
								point1 = right_edge[0];
							coor new_start = { 0, 0 };
							jump(point1, right_edge[right_edge_corner[0]+3], new_start, edge_threshold, true);
							right_edge_prev_dir = left;
							RightEdge(new_start, right_edge_prev_dir, true);

							if (left_edge_corner[0] > 10)
								point1 = left_edge[left_edge_corner[0] - 10];
							else
								point1 = left_edge[0];
							jump(point1, left_edge[left_edge_corner[0]+3], new_start, edge_threshold, false);
							left_edge_prev_dir = right;
							LeftEdge(new_start, left_edge_prev_dir,true);
//						}

						final_point= {(left_edge[left_edge_corner[1]].x + right_edge[right_edge_corner[1]].x) / 2,
							(left_edge[left_edge_corner[1]].y + right_edge[right_edge_corner[1]].y) / 2};
						midpoint = final_point;
					} else {
						crossroad_state = Detected;
						final_point.x = (left_edge[left_edge_corner[0]].x + right_edge[right_edge_corner[0]].x) / 2;
						final_point.y = (left_edge[left_edge_corner[0]].y + right_edge[right_edge_corner[0]].y) / 2;
						midpoint= {(left_start.x+right_start.x)/2,115};
					}
				}
				else if(left_edge.size()) {
				    align = left_align;
					if (left_edge[left_edge_corner[0]].y > 70) {
						crossroad_state = Inside;
						if (prev_track_state != Normal) {
							coor point1;
							if (left_edge_corner[0] > 10)
							point1 = left_edge[left_edge_corner[0] - 10];
							else
							point1 = left_edge[0];
							coor new_start = {0, 0};
							jump(point1, left_edge[left_edge_corner[0] + 3], new_start, edge_threshold, false);
							left_edge_prev_dir = right;
							LeftEdge(new_start, left_edge_prev_dir, true);
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
					if (right_edge[right_edge_corner[0]].y > 70) {
						crossroad_state = Inside;
						if(prev_track_state != Normal) {
							coor point1;
							if (right_edge_corner[0] > 10)
							point1 = right_edge[right_edge_corner[0] - 10];
							else
							point1 = right_edge[0];
							coor new_start = {0, 0};
							jump(point1, right_edge[right_edge_corner[0] + 3], new_start, edge_threshold, true);
							right_edge_prev_dir = left;
							RightEdge(new_start, right_edge_prev_dir, true);
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
				if(left_start_point(midpoint,left_start,edge_threshold))
                    RightEdge(left_start,right_edge_prev_dir,false);
				if(right_start_point(midpoint,right_start,edge_threshold))
                    LeftEdge(right_start,left_edge_prev_dir,false);

				if(left_edge_corner.size() == 1 && right_edge_corner.size() == 1 && left_edge[left_edge_corner[0]].y < 114 && right_edge[right_edge_corner[0]].y <114) {
				    align = center_align;
					final_point = {(right_edge[right_edge_corner[0]].x+left_edge[left_edge_corner[0]].x)/2,
						(right_edge[right_edge_corner[0]].y+left_edge[left_edge_corner[0]].y)/2};
					midpoint=final_point;
				}
				else {
					track_state = Normal;
					crossroad_state = Detected;
					left_edge_prev_dir = up;
					right_edge_prev_dir = up;
//					if(left_start_point(midpoint,left_start,edge_threshold))
					    LeftEdge(left_start,left_edge_prev_dir,false);
//					if(right_start_point(midpoint,right_start,edge_threshold))
					    RightEdge(right_start,right_edge_prev_dir,false);
					if(left_edge.size() && right_edge.size()) {
					    align=center_align;
//						final_point = {(left_edge[left_edge.size()-1].x + right_edge[right_edge.size()-1].x)/2,
//							(left_edge[left_edge.size()-1].y + right_edge[right_edge.size()-1].y)/2};
					}
					else if(left_edge.size()) {
					    align = left_align;
//						final_point = left_edge[left_edge.size()-1];
					}
					else if(right_edge.size()) {
					    align = right_align;
//						final_point = right_edge[right_edge.size()-1];
					}
					midpoint= {(left_start.x+right_start.x)/2,115};
				};
			}

		}
		if (track_state == RightLoop) {
			//follow the right edge when Entering
			//right align
			if (loop_state == Entering) {
				if (prev_track_state == Normal) {
					for (int i = right_edge.size()-1; i > right_edge_corner[0]+10; i--) {

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
					}
					if (right_start_point(midpoint, right_start, edge_threshold)) {
						//opposite as well
						LeftLoopEdgeR(right_start, right_edge_prev_dir, leftmostP, true);
					}
					midpoint = {leftmostP.x-10,leftmostP.y};
					align = right_align;
					final_point = {leftmostP.x,leftmostP.y};

//					left_edge = right_edge;
//					left_edge_corner = right_edge_corner;

//					right_edge_prev_dir = up;
//					if (right_start_point(midpoint, right_start, edge_threshold))
//						RightEdge(right_start, right_edge_prev_dir,  false);

					if (right_edge_corner.size() == 1 && right_edge[right_edge_corner[0]].y>100) {

						loop_state = In;
						midpoint = {(left_start.x+right_start.x)/2,100};
					}
					else{
						if(isRightBottomExist(midpoint,edge_threshold)){
							right_edge_prev_dir = up;
							if (right_start_point(midpoint, right_start, edge_threshold))
								RightEdge(right_start, right_edge_prev_dir, false);

							//set control
							//if the bottom exist, then we find the farthest point as end point
							if(right_edge.size()>0){
								right_end_point = right_edge[right_edge.size()-1];
								right_end_point_found = true;
							}
							else{
								right_end_point_found = false;
							}
							align = right_align;
						}
						else{
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
				if(left_edge_corner.size() == 1 && right_edge_corner.size() == 0 && left_edge[left_edge_corner[0]].y>100) {
					loop_state = Leaving;
				}

				//set control
				//TODO
				//as normal way
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
				if(left_edge.size()>0 && left_edge_corner.size()==0) {
					loop_state = Finished;
				}
				else{
					//set control
					right_end_point_found = true;
					if(right_edge.size()>0)
						right_end_point = right_edge[right_edge.size()-1];
					else
						right_end_point_found = false; //follow previous direction
					align = right_align;
				}
			}
			//follow the left edges when Finished
			else if(loop_state == Finished) {
				//increase the midpoint height for detecting the corner?
				midpoint = {midpoint.x,115};
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
				right_edge = left_edge;
				right_edge_corner = left_edge_corner;
				left_edge = temp_edge;
				left_edge_corner = temp_edge_corner;
				midpoint = {(left_start.x+right_start.x)/2,(left_start.y+right_start.y)/2};
				if(right_edge_corner.size()==1) {
					track_state = Normal;
					loop_state = Entering;
				}
				else{
					//set control
					left_end_point_found = true;
					if(left_edge.size()>11)
						left_end_point = left_edge[left_edge.size()-11];
					else if(left_edge.size()>0)
						left_end_point =  left_edge[left_edge.size()-1];
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
						LeftEdge(left_start, left_edge_prev_dir,  false);
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

		lcd->SetRegion(libsc::St7735r::Lcd::Rect(midpoint.x,midpoint.y,4,4));
		lcd->FillColor(lcd->kBlue);
//		char buffer[50];
//		sprintf(buffer, "t %d l %d", track_state, loop_state);
//		lcd->SetRegion(libsc::Lcd::Rect(0, 60, 160, 40));
//		writerP->WriteString(buffer);
		prev_track_state = track_state;

	}
}
