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

bool right_jump = false;
bool left_jump = false;

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

Align align = center_align;

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

void FindJumpStart(int& prev_edge_dir) {
	int x, y;
	if (prev_edge_dir == left) {
		x = left_edge.back().x;
		y = left_edge.back().y;
	} else {
		x = right_edge.back().x;
		y = right_edge.back().y;
	}
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
		if (point.front().x > point.back().x) {
			left_edge.push_back(point.front());
			prev_edge_dir = dir.front();
		} else {
			left_edge.push_back(point.back());
			prev_edge_dir = dir.back();
		}
	} else {
		if (point.front().x < point.back().x) {
			right_edge.push_back(point.front());
			prev_edge_dir = dir.front();
		} else {
			right_edge.push_back(point.back());
			prev_edge_dir = dir.back();
		}
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
    if(left_jump || right_jump) {
        int a2 = (img2world[pivot.x][pivot.y][0] - img2world[m.x][m.y][0]) * (img2world[pivot.x][pivot.y][0] - img2world[m.x][m.y][0]) +
                 (img2world[pivot.x][pivot.y][1] - img2world[m.x][m.y][1]) * (img2world[pivot.x][pivot.y][1] - img2world[m.x][m.y][1]);
        a2/=100;
        int b2 = (img2world[pivot.x][pivot.y][0] - img2world[n.x][n.y][0]) * (img2world[pivot.x][pivot.y][0] - img2world[n.x][n.y][0]) +
                 (img2world[pivot.x][pivot.y][1] - img2world[n.x][n.y][1]) * (img2world[pivot.x][pivot.y][1] - img2world[n.x][n.y][1]);
        b2/=100;
        int c2 = (img2world[m.x][m.y][0] - img2world[n.x][n.y][0]) * (img2world[m.x][m.y][0] - img2world[n.x][n.y][0]) +
                 (img2world[m.x][m.y][1] - img2world[n.x][n.y][1]) * (img2world[m.x][m.y][1] - img2world[n.x][n.y][1]);
        c2/=100;
        int pythagoras = a2 + b2 - c2;
        if (pythagoras >= 0)
            return true;
        pythagoras = std::abs(pythagoras);
        uint64_t pythagoras_uint = (uint64_t) pythagoras;
        float value = ((((float) ((uint64_t) (pythagoras_uint * pythagoras_uint)) / 4) / a2) / b2);
        if (value < 0.17)
            return true;
        return false;
    }
    else{
        int c2 = (m.x - n.x) * (m.x - n.x) + (m.y - n.y) * (m.y - n.y);
        if(c2>170)
            return false;
        int a2 = (pivot.x - m.x) * (pivot.x - m.x) + (pivot.y - m.y) * (pivot.y - m.y);
        int b2 = (pivot.x - n.x) * (pivot.x - n.x) + (pivot.y - n.y) * (pivot.y - n.y);
        int pythagoras = a2 + b2 - c2;
        if (pythagoras >= 0)
            return true;
        float value = (float) (pythagoras * pythagoras) / (4 * a2 * b2);
        if(value<0.07)
            return true;
    }
    return false;
}
float value_corner(coor pivot, coor m, coor n){
    int c2 = (m.x - n.x) * (m.x - n.x) + (m.y - n.y) * (m.y - n.y);
    int a2 = (pivot.x - m.x) * (pivot.x - m.x) + (pivot.y - m.y) * (pivot.y - m.y);
    int b2 = (pivot.x - n.x) * (pivot.x - n.x) + (pivot.y - n.y) * (pivot.y - n.y);
    int pythagoras = a2 + b2 - c2;
    return (float) (pythagoras * pythagoras) / (4 * a2 * b2);
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

	while (FindLeftEdge(edge_prev_dir)) {
		if ((direction == up && left_edge.back().y > start_point.y + 10) ||
			(direction == down && left_edge.back().y < start_point.y - 10) ||
			(direction == left && left_edge.back().x > start_point.x + 10) ||
			(direction == right && left_edge.back().x < start_point.x - 10))
			break;
		if (left_edge.size()>15 && left_edge[left_edge.size() - 11].y > 30 && check_corner(left_edge[left_edge.size()-11],left_edge[left_edge.size()-1],left_edge[left_edge.size()-16])){
			left_edge_corner.push_back(left_edge.size()-11);
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

    while (FindRightEdge(edge_prev_dir)) {
        if ((direction == up && right_edge[right_edge.size() - 1].y > start_point.y + 10) ||
            (direction == down && right_edge[right_edge.size() - 1].y < start_point.y - 10) ||
            (direction == left && right_edge[right_edge.size() - 1].x > start_point.x + 10) ||
            (direction == right && right_edge[right_edge.size() - 1].x < start_point.x - 10))
            break;
        if (right_edge.size()>15 && right_edge[right_edge.size() - 11].y > 30 && check_corner(right_edge[right_edge.size()-11],right_edge[right_edge.size()-1],right_edge[right_edge.size()-16])){
            right_edge_corner.push_back(right_edge.size()-11);
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
	while (FindRightEdge(edge_prev_dir) && ((right_edge[right_edge.size() - 1].x - start_point.x )> -20)) {
		if (right_edge[right_edge.size() - 1].x > rightmostP.x)
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
	while (FindRightEdge(edge_prev_dir) && (right_edge[right_edge.size() - 1].x - start_point.x > -20)) {
		if (right_edge[right_edge.size() - 1].x < leftmostP.x)
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
		if (left_edge[left_edge.size() - 1].x < leftmostP.x)
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
		if (left_edge[left_edge.size() - 1].x > rightmostP.x)
			rightmostP = left_edge[left_edge.size() - 1];
	}
}

bool jump(coor corner, coor point1, coor point2, coor& new_start, bool direction) { //jump from the corner to new edge. returns true if a edge is found.
    float slope = (float)(point1.x - corner.x) / (point1.y - corner.y);
    float constant = point2.x - slope * point2.y;
    new_start.y = point2.y-15;
    new_start.x = (int16_t) (new_start.y * slope + constant);
    while (new_start.x < width - 4 && new_start.x > 3 && new_start.y < height - 4 && new_start.y > 3 && SobelEdgeDetection(new_start.x, new_start.y) < edge_threshold) {
//        lcd->SetRegion(libsc::St7735r::Lcd::Rect(new_start.x, new_start.y, 1, 1));
//        lcd->FillColor(lcd->kBlue);
        new_start.y--;
        new_start.x = (new_start.y * slope + constant);
    }
    if (new_start.x < width - 4 && new_start.x > 3 && new_start.y < height - 4 && new_start.y > 3) {
        if(direction)
            right_jump = true;
        else
            left_jump = true;
        return true;
    }
    return false;
}

bool Ljump(coor& new_start){
    coor point1 = {0,0};
    coor point2 = {0,0};
    if (left_edge_corner[0] > 10)
        point1 = left_edge[left_edge_corner[0] - 10];
    else
        point1 = left_edge[0];
    int i=left_edge_corner[0];
    for(;FindLeftEdge(left_edge_prev_dir) && value_corner(left_edge[left_edge.size()-11],left_edge[left_edge.size()-1],left_edge[left_edge.size()-16])<0.9;i++);
    point2 = left_edge[i+3];
    return jump(left_edge[left_edge_corner[0]],point1,point2,new_start,false);
}

bool Rjump(coor& new_start){
    coor point1 = {0,0};
    coor point2 = {0,0};
    if (right_edge_corner[0] > 10)
        point1 = right_edge[right_edge_corner[0] - 10];
    else
        point1 = right_edge[0];
    int i=right_edge_corner[0];
    for(;FindRightEdge(right_edge_prev_dir) && value_corner(right_edge[right_edge.size()-11],right_edge[right_edge.size()-1],right_edge[right_edge.size()-16])<0.9;i++);
    point2 = right_edge[i+3];
    return jump(right_edge[right_edge_corner[0]],point1,point2,new_start,true);
}

bool right_start_point(coor midpoint, coor& right_start, int threshold) {
	right_start = midpoint;
	while (SobelEdgeDetection(right_start.x, right_start.y) < threshold && right_start.x < width - 4)
		right_start.x++;
	if (right_start.x < width - 1)
		return true;
	return false;
}

bool left_start_point(coor midpoint, coor& left_start, int threshold) {
	left_start = midpoint;
	while (SobelEdgeDetection(left_start.x, left_start.y) < threshold && left_start.x > 3)
		left_start.x--;
	if (left_start.x > 0)
		return true;
	return false;
}

void normal_left_corner_fsm(Tstate& track_state, coor& final_point, coor& midpoint, coor left_start, coor right_start, int& edge_prev_dir) {
    align = left_align;
    coor new_start = { 0, 0 };
    if (Ljump(new_start)) {
        left_edge_prev_dir = left;
        left_edge.push_back(new_start);
        FindJumpStart(edge_prev_dir);
        LeftEdge(left_edge.back(), left_edge_prev_dir, false);
        if (left_edge_corner.size() == 1) {
//            track_state = Crossroad;
        } else {
			track_state = LeftLoop;
        }
    } else {
        track_state = Normal;
        final_point = new_start;
        midpoint= {(left_start.x+right_start.x)/2,115};
    }
}

void normal_right_corner_fsm(Tstate& track_state, coor& final_point, coor& midpoint, coor left_start, coor right_start, int& edge_prev_dir) {
    align = right_align;
    coor new_start = { 0, 0 };
    if (Rjump(new_start)) {
        right_edge_prev_dir = right;
        right_edge.push_back(new_start);
        FindJumpStart(edge_prev_dir);
        RightEdge(new_start, right_edge_prev_dir, false);
        if (right_edge_corner.size() == 1) {
//            track_state = Crossroad;
        } else {
			track_state = RightLoop;
        }
    } else {
        final_point = new_start;
        midpoint= {(left_start.x+right_start.x)/2,115};
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
	coor midpoint = { 92, 115 };

	coor left_start = { 0, 0 };
	coor right_start = { 0, 0 };
	coor final_point = { 0, 0 };

	while (1) {
		if (camera->IsAvailable()) {
			buffer = camera->LockBuffer(); //Use GetPoint(x,y) to get the gradient of the point
			camera->UnlockBuffer();
		}
//		for (uint16_t i = 0; i < height; i++) {
//			lcd->SetRegion(libsc::Lcd::Rect(0, i, 160, 1));
//			lcd->FillGrayscalePixel(buffer + camera->GetW() * i, 160);
//		}
		motor->SetPower(200);
		motor->SetClockwise(false);

		InitSearchDistance(0);

		empty_left();
		empty_right();

		coor leftmostP = { 188, 120 }; //sure this is right? it resets leftmostP in every new frame
		coor rightmostP = { 0, 120 };  //sure this shouldn't be outside while loop?

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
//                            track_state = Crossroad;
                        } else {
                            final_point = {(left_edge[left_edge_corner[0]].x + right_edge[right_edge_corner[0]].x) / 2,
                                           (left_edge[left_edge_corner[0]].y + right_edge[right_edge_corner[0]].y) / 2};
                            align = center_align;
                            midpoint = {(left_start.x + right_start.x) / 2, 115};
                        }
                    } else if (left_edge[left_edge_corner[0]].y > right_edge[right_edge_corner[0]].y) {
                        if (left_edge[left_edge_corner[0]].y > 90)
                            normal_left_corner_fsm(track_state, final_point, midpoint, left_start, right_start, left_edge_prev_dir);
                        else {
                            final_point = left_end_point.y > left_edge[left_edge_corner[0]].y ? left_edge[left_edge_corner[0]] : left_end_point;
                            align = left_align;
                            midpoint = {(left_start.x + right_start.x) / 2, 115};
                        }
                    } else if (left_edge[left_edge_corner[0]].y < right_edge[right_edge_corner[0]].y) {
                        if (right_edge[right_edge_corner[0]].y > 90)
                            normal_right_corner_fsm(track_state, final_point, midpoint, left_start, right_start, right_edge_prev_dir);
                        else {
                            final_point = right_end_point.y > right_edge[right_edge_corner[0]].y ? right_edge[right_edge_corner[0]] : right_end_point;
                            align = right_align;
                            midpoint = {(left_start.x + right_start.x) / 2, 115};
                        }
                    }
                } else if (left_edge_corner.size() == 1 && right_edge_corner.size() == 0) {
                    if (left_edge[left_edge_corner[0]].y > 90)
                        normal_left_corner_fsm(track_state, final_point, midpoint, left_start, right_start, left_edge_prev_dir);
                    else {
                        final_point =
                                left_end_point.y > left_edge[left_edge_corner[0]].y ? left_edge[left_edge_corner[0]] : left_end_point;
                        align = left_align;
                        midpoint = {(left_start.x + right_start.x) / 2, 115};
                    }
                } else if (left_edge_corner.size() == 0 && right_edge_corner.size() == 1) {
                    if (right_edge[right_edge_corner[0]].y > 90)
                        normal_right_corner_fsm(track_state, final_point, midpoint, left_start, right_start, right_edge_prev_dir);
                    else {
                        final_point = right_end_point.y > right_edge[right_edge_corner[0]].y ? right_edge[right_edge_corner[0]] : right_end_point;
                        align = right_align;
                        midpoint = {(left_start.x + right_start.x) / 2, 115};
                    }
                } else {
                    align = center_align;
                    final_point = {(left_end_point.x + right_end_point.x) / 2, (left_end_point.y + right_end_point.y) / 2};
                    midpoint = {(left_start.x + right_start.x) / 2, 115};
                }
            } else if (right_edge.size()) {
                if (right_edge_corner.size() == 1 && right_edge[right_edge_corner[0]].y > 90) {
                    normal_right_corner_fsm(track_state, final_point, midpoint, left_start, right_start, right_edge_prev_dir);
                } else {
                    align = right_align;
                    final_point = right_end_point;
                    midpoint = {(left_start.x + right_start.x) / 2, 115};
                }
            } else if (left_edge.size()) {
                if (left_edge_corner.size() == 1 && left_edge[left_edge_corner[0]].y > 90) {
                    normal_left_corner_fsm(track_state, final_point, midpoint, left_start, right_start, right_edge_prev_dir);
                } else {
                    align = left_align;
                    final_point = left_end_point;
                    midpoint = {(left_start.x + right_start.x) / 2, 115};
                }
            }
        }
		if (track_state == Crossroad) {
            // TODO: integrate with amrut's code later
		}
		if (track_state == RightLoop) {
			//follow the right edge when Entering
			//right align
			if (loop_state == Entering) {
				if (prev_track_state == Normal) {
					for (int i = right_edge.size() - 1; i > 0; i--) {
						if (right_edge[i].x < leftmostP.x && right_edge[i].y >30)
							leftmostP = right_edge[i];
					}

					final_point = {leftmostP.x,leftmostP.y};
					align = right_align;
					midpoint = {leftmostP.x-10,leftmostP.y};

				} else if (prev_track_state == RightLoop) {
					left_edge_prev_dir = down;
					right_edge_prev_dir = down;
					if (left_start_point({midpoint.x,90}, left_start, edge_threshold)) {
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
						midpoint = {(left_start.x+right_start.x)/2, 90 };
					}
					else {
						if(FindEndPoint(leftmostP.x,leftmostP.y)){
							align = right_align;
							final_point = {leftmostP.x,leftmostP.y};
						}
						else{
							coor tempmidpoint = {midpoint.x,115};
							if (right_start_point(tempmidpoint, right_start, edge_threshold)){
								right_edge_prev_dir = up;
								RightEdge(right_start, right_edge_prev_dir, false);
								//midpoint.x = right_start.x-20;
								align = right_align;
								if(right_end_point_found)
									final_point = right_end_point;//right_edge.back();
								else if(right_edge.size())
									final_point = right_edge.back();

								//set midpoint
								if(leftmostP.y>115){
									midpoint = {right_start.x-20,115};
								}
							}
							else{
								right_edge_prev_dir = up;
								tempmidpoint = {leftmostP.x-10,leftmostP.y};
								if (right_start_point(tempmidpoint, right_start, edge_threshold)){
									RightEdge(right_start, right_edge_prev_dir, false);
								}
								align = right_align;
								if(right_end_point_found)
									final_point = right_end_point;//right_edge.back();
								else if(right_edge.size())
									final_point = right_edge.back();
							}
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
				if(right_start_point({midpoint.x,115},right_start,edge_threshold))
				RightEdge(right_start,right_edge_prev_dir,false);
				if(left_edge_corner.size()==1 &&  left_edge[left_edge_corner[0]].y<90 ){
					midpoint = {(left_start.x+right_start.x)/2,115};
				}
				else{
					midpoint = {(left_start.x+right_start.x)/2,(left_start.y+right_start.y)/2};
				}


				//&& left_edge[left_edge_corner[0]].y>100
				if(left_edge_corner.size() == 1 && right_edge_corner.size() == 0  && left_edge[left_edge_corner[0]].y>100) {
					loop_state = Leaving;
					midpoint = {(left_start.x+right_start.x)/2,90};
				}
				else{
					if(left_edge.size() && right_edge.size()){
						align = center_align;

						final_point = {(left_end_point.x + right_end_point.x)/2,
									(left_end_point.y + right_end_point.y)/2};
					}
					else if(left_edge.size()){
						align = left_align;
						if(left_end_point_found){
							final_point = left_end_point;
						}
						else{
							final_point = left_edge.back();
						}
					}
					else if(right_edge.size()){
						align = right_align;
						if(left_end_point_found){
							final_point = right_end_point;
						}
						else{
							final_point = right_edge.back();
						}
					}


				}


			}
			//follow the right edges when Leaving
			else if(loop_state == Leaving) {
				left_edge_prev_dir = up;
				right_edge_prev_dir = up;
				if(left_start_point(midpoint,left_start,edge_threshold))
					LeftEdge(left_start,left_edge_prev_dir,false);
				if(right_start_point({midpoint.x,115},right_start,edge_threshold))
					RightEdge(right_start,right_edge_prev_dir,false);
				midpoint = {(left_start.x+right_start.x)/2,(left_start.y+right_start.y)/2};
//				char buffer[50];
//				sprintf(buffer, "%d %d", right_edge_corner.size(),left_edge_corner.size());
//				lcd->SetRegion(libsc::Lcd::Rect(0, 80, 160, 40));
//				writerP->WriteString(buffer);
				if(left_edge.size()>80 && left_edge_corner.size()==0) {
					loop_state = Finished;
				}
				else {
					//set control
					align = right_align;
					if(right_edge.size())
						final_point = right_edge.back();
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
					for (int i = left_edge.size()-1; i >0;i--) {
						if (left_edge[i].x > rightmostP.x && left_edge[i].y>30)
							rightmostP = left_edge[i];
					}
					final_point = {rightmostP.x,rightmostP.y};
					align = left_align;
					midpoint = {rightmostP.x+10,rightmostP.y};

				} else if (prev_track_state == LeftLoop) {
					right_edge_prev_dir = down;
					left_edge_prev_dir = down;
					if (right_start_point({midpoint.x,90}, right_start, edge_threshold)) {
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
						if(FindEndPoint(rightmostP.x,rightmostP.y)){
							align = left_align;
							final_point = {rightmostP.x,rightmostP.y};
						}
						else{
							coor tempmidpoint = {midpoint.x,115};
							if (left_start_point(tempmidpoint, left_start, edge_threshold)){
								left_edge_prev_dir = up;
								LeftEdge(left_start, left_edge_prev_dir, false);
								//midpoint.x = right_start.x-20;
								align = left_align;
								if(left_end_point_found)
									final_point = left_end_point;//right_edge.back();
								else if(left_edge.size())
									final_point = left_edge.back();

								//set midpoint
								if(rightmostP.y>115){
									midpoint = {left_start.x+20,115};
								}
							}
							else{
								left_edge_prev_dir = up;
								tempmidpoint = {rightmostP.x+10,rightmostP.y};
								if (left_start_point(tempmidpoint, left_start, edge_threshold)){
									LeftEdge(left_start, left_edge_prev_dir, false);
								}
								align = left_align;
								if(left_end_point_found)
									final_point = left_end_point;//right_edge.back();
								else if(left_edge.size())
									final_point = left_edge.back();
							}
						}
					}

				}
			}
			//follow the normal way when In
			else if(loop_state == In) {
				left_edge_prev_dir = up;
				right_edge_prev_dir = up;
				if(left_start_point({midpoint.x,115},left_start,edge_threshold))
				LeftEdge(left_start,left_edge_prev_dir,false);
				if(right_start_point(midpoint,right_start,edge_threshold))
				RightEdge(right_start,right_edge_prev_dir,false);

				if(right_edge_corner.size()==1 &&  right_edge[right_edge_corner[0]].y<90 ){
					midpoint = {(left_start.x+right_start.x)/2,115};
				}
				else{
					midpoint = {(left_start.x+right_start.x)/2,(left_start.y+right_start.y)/2};
				}




				if(right_edge_corner.size() == 1 && left_edge_corner.size() == 0  &&  right_edge[right_edge_corner[0]].y>100) {
					loop_state = Leaving;
					midpoint = {(left_start.x+right_start.x)/2,90};
				}
				else{
					if(left_edge.size() && right_edge.size()){
						align = center_align;

						final_point = {(left_end_point.x + right_end_point.x)/2,
									(left_end_point.y + right_end_point.y)/2};
					}
					else if(left_edge.size()){
						align = left_align;
						if(left_end_point_found){
							final_point = left_end_point;
						}
						else{
							final_point = left_edge.back();
						}
					}
					else if(right_edge.size()){
						align = right_align;
						if(left_end_point_found){
							final_point = right_end_point;
						}
						else{
							final_point = right_edge.back();
						}
					}

				}
			}
			//follow the right edges when Leaving
			else if(loop_state == Leaving) {
				left_edge_prev_dir = up;
				right_edge_prev_dir = up;
				if(left_start_point({midpoint.x,115},left_start,edge_threshold))
					LeftEdge(left_start,left_edge_prev_dir,false);
				if(right_start_point(midpoint,right_start,edge_threshold))
					RightEdge(right_start,right_edge_prev_dir,false);
				midpoint = {(left_start.x+right_start.x)/2,(left_start.y+right_start.y)/2};
				if(right_edge.size()>80 && right_edge_corner.size()==0) {
					loop_state = Finished;
				}
				else {
					//set control
					align = left_align;
					if(left_edge.size())
						final_point = left_edge.back();
				}

			}
			//follow the left edges when Finished
			else if(loop_state == Finished) {
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
				else{
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
				else{
					//set control
					final_point = right_end_point;
					align = right_align;

				}
			}
		}

//		for (int i = 0; i < left_edge.size(); i++) {
//
//			lcd->SetRegion(libsc::St7735r::Lcd::Rect(left_edge[i].x, left_edge[i].y, 1, 1));
//			lcd->FillColor(lcd->kPurple);
//		}
//		for (int i = 0; i < right_edge.size(); i++) {
//			lcd->SetRegion(libsc::St7735r::Lcd::Rect(right_edge[i].x, right_edge[i].y, 1, 1));
//			lcd->FillColor(lcd->kRed);
//		}
//
//		for (int i = 0; i < left_edge_corner.size(); i++) {
//			lcd->SetRegion(libsc::St7735r::Lcd::Rect(left_edge[left_edge_corner[i]].x, left_edge[left_edge_corner[i]].y, 5, 5));
//			lcd->FillColor(lcd->kPurple);
//		}
//		for (int i = 0; i < right_edge_corner.size(); i++) {
//
//			lcd->SetRegion(libsc::St7735r::Lcd::Rect(right_edge[right_edge_corner[i]].x, right_edge[right_edge_corner[i]].y, 5, 5));
//			lcd->FillColor(lcd->kRed);
//		}
//
//		lcd->SetRegion(libsc::St7735r::Lcd::Rect(midpoint.x, midpoint.y, 4, 4));
//		lcd->FillColor(lcd->kBlue);
//		char buffer[50];
//		sprintf(buffer, "t %d l %d %d %d \n %d %d", track_state, loop_state,right_edge.size(),left_edge.size(),final_point.x,final_point.y);
//		lcd->SetRegion(libsc::Lcd::Rect(0, 0, 160, 40));
//		writerP->WriteString(buffer);

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
				}
				break;
			case 1:
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
				} else if(left_edge_corner.size()||right_edge_corner.size()) {
//					lcd->SetRegion(libsc::Lcd::Rect(final_point.x-1,final_point.y-1,3,3));
//					lcd->FillColor(libsc::Lcd::kRed);
					switch(align) {
						case 0:
						destination=final_point;
						servo_angle = 1120 + std::atan(1.0*(img2world[destination.x][destination.y][0] - img2world[46][118][0]) / (img2world[destination.x][destination.y][1] - img2world[46][118][1])) * 1800 / 3.14;
						servo_angle += 0.4 * (servo_angle - prev_servo_angle);
						break;
						case 1:
						destination=final_point;
						servo_angle = 1120 + std::atan(1.0*(img2world[destination.x][destination.y][0] - img2world[97][118][0]) / (img2world[destination.x][destination.y][1] - img2world[97][118][1])) * 1800 / 3.14;
						servo_angle += 0.4 * (servo_angle - prev_servo_angle);
						break;
						case 2:
						destination=final_point;
						servo_angle = 1120 + std::atan(1.0*(img2world[destination.x][destination.y][0] - img2world[149][118][0]) / (img2world[destination.x][destination.y][1] - img2world[149][118][1])) * 1800 / 3.14;
						servo_angle += 0.4 * (servo_angle - prev_servo_angle);
						break;
					}
				} else if (left_edge.size() > right_edge.size()) {
					destination=left_edge.back();
					servo_angle = 1120 + std::atan(1.0*(img2world[destination.x][destination.y][0] - img2world[46][118][0]) / (img2world[destination.x][destination.y][1] - img2world[46][118][1])) * 1800 / 3.14;
					servo_angle += 0.4 * (servo_angle - prev_servo_angle);
				} else if(right_edge.size()) {
					destination=right_edge.back();
					servo_angle = 1120 + std::atan(1.0*(img2world[destination.x][destination.y][0] - img2world[149][118][0]) / (img2world[destination.x][destination.y][1] - img2world[149][118][1])) * 1800 / 3.14;
					servo_angle += 0.4 * (servo_angle - prev_servo_angle);
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
					break;
				}
			}
		} else if(track_state==Tstate::Crossroad) {
			switch (align) {
				case 0:
				servo_angle = 1120 + std::atan(1.0 * (img2world[final_point.x][final_point.y][0] - img2world[46][118][0]) / (img2world[final_point.x][final_point.y][1] - img2world[46][118][1])) * 1800 / 3.14;
				servo_angle += 0.4 * (servo_angle - prev_servo_angle);
				break;
				case 1:
				servo_angle = 1120 + std::atan(1.0*(img2world[final_point.x][final_point.y][0] - img2world[97][118][0]) / (img2world[final_point.x][final_point.y][1] - img2world[97][118][1])) * 1800 / 3.14;
				servo_angle += 0.4 * (servo_angle - prev_servo_angle);
				break;
				case 2:
				servo_angle = 1120 + std::atan(1.0*(img2world[final_point.x][final_point.y][0] - img2world[149][118][0]) / (img2world[final_point.x][final_point.y][1] - img2world[149][118][1])) * 1800 / 3.14;
				servo_angle += 0.4 * (servo_angle - prev_servo_angle);
				break;
			}
		} else if(track_state==Tstate::RightLoop) {
			switch(loop_state) {
				case Lstate::Entering:
				destination=final_point;
				servo_angle = 1120 + std::atan(1.0*(img2world[destination.x][destination.y][0] - img2world[149][118][0]) / (img2world[destination.x][destination.y][1] - img2world[149][118][1])) * 1800 / 3.14;
				servo_angle += 0.4 * (servo_angle - prev_servo_angle);

				break;
				case Lstate::Leaving:
				destination=final_point;
				servo_angle = 1120 + std::atan(1.0*(img2world[destination.x][destination.y][0] - img2world[149][118][0]) / (img2world[destination.x][destination.y][1] - img2world[149][118][1])) * 1800 / 3.14;
				servo_angle += 0.4 * (servo_angle - prev_servo_angle);


				break;
				case Lstate::Finished:
				destination=left_end_point_found?left_end_point:left_edge.back();
				servo_angle = 1120 + std::atan(1.0*(img2world[destination.x][destination.y][0] - img2world[46][118][0]) / (img2world[destination.x][destination.y][1] - img2world[46][118][1])) * 1800 / 3.14;
				servo_angle += 0.4 * (servo_angle - prev_servo_angle);

				break;
			}
		} else if(track_state==Tstate::LeftLoop) {
			switch(loop_state) {
				case Lstate::Entering:
				destination=final_point;
				servo_angle = 1120 + std::atan(1.0*(img2world[destination.x][destination.y][0] - img2world[46][118][0]) / (img2world[destination.x][destination.y][1] - img2world[46][118][1])) * 1800 / 3.14;
				servo_angle += 0.4 * (servo_angle - prev_servo_angle);

				break;
				case Lstate::Leaving:
				destination=final_point;
				servo_angle = 1120 + std::atan(1.0*(img2world[destination.x][destination.y][0] - img2world[46][118][0]) / (img2world[destination.x][destination.y][1] - img2world[46][118][1])) * 1800 / 3.14;
				servo_angle += 0.4 * (servo_angle - prev_servo_angle);

				break;
				case Lstate::Finished:
				destination=right_end_point_found?right_end_point:right_edge.back();
				servo_angle = 1120 + std::atan(1.0*(img2world[destination.x][destination.y][0] - img2world[149][118][0]) / (img2world[destination.x][destination.y][1] - img2world[149][118][1])) * 1800 / 3.14;
				servo_angle += 0.4 * (servo_angle - prev_servo_angle);

				break;
			}

		}
		servo->SetDegree(libutil::Clamp(700,servo_angle,1500));

//
		prev_track_state = track_state;
		motor->SetPower(200);
	}
//	}
}
