/*
 * algo.cpp
 *
 *  Created on: May 30, 2018
 *      Author: LeeChunHei
 */

#include <algo.h>

#define GetPoint(x, y) buffer[(x) + (y) * 189]

struct coor {
	int16_t x;
	int16_t y;
};

const Byte* buffer;

std::vector<coor> left_edge;
std::vector<coor> right_edge;
#define SobelEdgeDetection(x, y) std::abs(-GetPoint((x) - 1, (y)-1) - 2 * GetPoint((x) - 1, (y)) - GetPoint((x) - 1, (y)+1) + GetPoint((x) + 1, (y)-1) + 2 * GetPoint((x) + 1, (y)) + GetPoint((x) + 1, (y)+1)) + std::abs(-GetPoint((x) - 1, (y)-1) - 2 * GetPoint((x), (y)-1) - GetPoint((x) + 1, (y)-1) + GetPoint((x) - 1, (y)+1) + 2 * GetPoint((x), (y)+1) + GetPoint((x) + 1, (y)+1))
#define RobertsEdgeDetection(x, y) std::abs(GetPoint(x,y)-GetPoint(x+1,y+1))+std::abs(GetPoint(x+1,y)-GetPoint(x,y+1))
#define up 0
#define up_left 1
#define left 2
#define down_left 3
#define down 4
#define down_right 5
#define right 6
#define up_right 7
int left_edge_prev_dir;
int right_edge_prev_dir;

std::vector<coor> left_corner;
std::vector<coor> right_corner;
int threshold_FAST;
bool left_bottom_exist = false;
bool pre_left_bottom_exist = false;
bool right_bottom_exist = false;
bool pre_right_bottom_exist = false;
//loop states
bool roundaboutR = false;
bool roundaboutL = false;

bool finishRoundR = false;
bool finishRoundL = false;

bool enterRoundR = false;
bool enterRoundL = false;

bool leaveRoundR = false;
bool leaveRoundL = false;
int leavetime;

bool FindLeftEdge(int& edge_prev_dir) {
	int size = left_edge.size();
	if (size > 100 || !size)
		return false;
	coor edge_coor = left_edge.back();
	int x = edge_coor.x;
	int y = edge_coor.y;
	edge_prev_dir += 2;
	for (int i = 0; i < 5; i++) {
		edge_prev_dir = edge_prev_dir < 0 ? 8 + edge_prev_dir : edge_prev_dir > up_right ? edge_prev_dir - 8 : edge_prev_dir;
		switch (edge_prev_dir) {
		case up:
			if (SobelEdgeDetection(x, y - 1) > edge_threshold) {
				edge_coor.y = y - 1;
				left_edge.push_back(edge_coor);
				return true;
			}
			break;
		case up_left:
			if (SobelEdgeDetection(x - 1, y - 1) > edge_threshold) {
				edge_coor.x = x - 1;
				edge_coor.y = y - 1;
				left_edge.push_back(edge_coor);
				return true;
			}
			break;
		case left:
			if (SobelEdgeDetection(x - 1, y) > edge_threshold) {
				edge_coor.x = x - 1;
				left_edge.push_back(edge_coor);
				return true;
			}
			break;
		case down_left:
			if (size > 3 && SobelEdgeDetection(x - 1, y + 1) > edge_threshold) {
				edge_coor.x = x - 1;
				edge_coor.y = y + 1;
				left_edge.push_back(edge_coor);
				return true;
			}
			break;
		case down:
			if (size > 3 && SobelEdgeDetection(x, y + 1) > edge_threshold) {
				edge_coor.y = y + 1;
				left_edge.push_back(edge_coor);
				return true;
			}
			break;
		case down_right:
			if (size > 3 && SobelEdgeDetection(x + 1, y + 1) > edge_threshold) {
				edge_coor.x = x + 1;
				edge_coor.y = y + 1;
				left_edge.push_back(edge_coor);
				return true;
			}
			break;
		case right:
			if (SobelEdgeDetection(x + 1, y) > edge_threshold) {
				edge_coor.x = x + 1;
				left_edge.push_back(edge_coor);
				return true;
			}
			break;
		case up_right:
			if (SobelEdgeDetection(x + 1, y - 1) > edge_threshold) {
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
	edge_prev_dir -= 2;
	for (int i = 0; i < 5; i++) {
		edge_prev_dir = edge_prev_dir < 0 ? 8 + edge_prev_dir : edge_prev_dir > up_right ? edge_prev_dir - 8 : edge_prev_dir;
		switch (edge_prev_dir) {
		case up:
			if (SobelEdgeDetection(x, y - 1) > edge_threshold) {
				edge_coor.y = y - 1;
				right_edge.push_back(edge_coor);
				return true;
			}
			break;
		case up_left:
			if (SobelEdgeDetection(x - 1, y - 1) > edge_threshold) {
				edge_coor.x = x - 1;
				edge_coor.y = y - 1;
				right_edge.push_back(edge_coor);
				return true;
			}
			break;
		case left:
			if (SobelEdgeDetection(x - 1, y) > edge_threshold) {
				edge_coor.x = x - 1;
				right_edge.push_back(edge_coor);
				return true;
			}
			break;
		case down_left:
			if (size > 3 && SobelEdgeDetection(x - 1, y + 1) > edge_threshold) {
				edge_coor.x = x - 1;
				edge_coor.y = y + 1;
				right_edge.push_back(edge_coor);
				return true;
			}
			break;
		case down:
			if (size > 3 && SobelEdgeDetection(x, y + 1) > edge_threshold) {
				edge_coor.y = y + 1;
				right_edge.push_back(edge_coor);
				return true;
			}
			break;
		case down_right:
			if (size > 3 && SobelEdgeDetection(x + 1, y + 1) > edge_threshold) {
				edge_coor.x = x + 1;
				edge_coor.y = y + 1;
				right_edge.push_back(edge_coor);
				return true;
			}
			break;
		case right:
			if (SobelEdgeDetection(x + 1, y) > edge_threshold) {
				edge_coor.x = x + 1;
				right_edge.push_back(edge_coor);
				return true;
			}
			break;
		case up_right:
			if (SobelEdgeDetection(x + 1, y - 1) > edge_threshold) {
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

bool FAST(int x, int y, int threshold) {
	Byte center = GetPoint(x,y)-threshold;
	Byte center2 = GetPoint(x,y)+threshold;
	Byte point;
	int temp=0;
	int temp2=0;
	bool points[16]= {false};
	point=GetPoint(x,y-3);
	if(point<center) {
		temp++;
	} else if(point>center2) {
		temp2++;
	}
	point=GetPoint(x+1,y-3);
	if(point<center) {
		temp++;
	} else if(point>center2) {
		temp2++;
	}
	point=GetPoint(x+2,y-2);
	if(point<center) {
		temp++;
	} else if(point>center2) {
		temp2++;
	}
	point=GetPoint(x+3,y-1);
	if(point<center) {
		temp++;
	} else if(point>center2) {
		temp2++;
	}
	point=GetPoint(x+3,y);
	if(point<center) {
		temp++;
	} else if(point>center2) {
		temp2++;
	}
	point=GetPoint(x+3,y+1);
	if(point<center) {
		temp++;
	} else if(point>center2) {
		temp2++;
	}
	point=GetPoint(x+2,y+2);
	if(point<center) {
		temp++;
	} else if(point>center2) {
		temp2++;
	}
	point=GetPoint(x+1,y+3);
	if(point<center) {
		temp++;
	} else if(point>center2) {
		temp2++;
	}
	point=GetPoint(x,y+3);
	if(point<center) {
		temp++;
	} else if(point>center2) {
		temp2++;
	}
	point=GetPoint(x-1,y+3);
	if(point<center) {
		temp++;
	} else if(point>center2) {
		temp2++;
	}
	point=GetPoint(x-2,y+2);
	if(point<center) {
		temp++;
	} else if(point>center2) {
		temp2++;
	}
	point=GetPoint(x-3,y+1);
	if(point<center) {
		temp++;
	} else if(point>center2) {
		temp2++;
	}
	point=GetPoint(x-3,y);
	if(point<center) {
		temp++;
	} else if(point>center2) {
		temp2++;
	}
	point=GetPoint(x-3,y-1);
	if(point<center) {
		temp++;
	} else if(point>center2) {
		temp2++;
	}
	point=GetPoint(x-2,y-2);
	if(point<center) {
		temp++;
	} else if(point>center2) {
		temp2++;
	}
	point=GetPoint(x-1,y-3);
	if(point<center) {
		temp++;
	} else if(point>center2) {
		temp2++;
	}
	if(temp<6&&temp>1&&temp2>10&&temp2<15)
	return true;
	return false;
}

bool HarrisCorner(int x, int y) {
#define Gx(x,y) -GetPoint(x-1,y-1)-2*GetPoint(x-1,y)-GetPoint(x-1,y+1)+GetPoint(x+1,y-1)+2*GetPoint(x+1,y)+GetPoint(x+1,y+1)
#define Gy(x,y) -GetPoint(x-1,y-1)-2*GetPoint(x,y-1)-GetPoint(x+1,y-1)+GetPoint(x-1,y+1)+2*GetPoint(x,y+1)+GetPoint(x+1,y+1)
	float Sx2 = 0;
	float Sy2 = 0;
	float Sxy = 0;
	int Ix = Gx(x - 1, y - 1);
	int Iy = Gy(x - 1, y - 1);
	Sx2 += 0.3679 * Ix * Ix;
	Sy2 += 0.3679 * Iy * Iy;
	Sxy += 0.3679 * Ix * Iy;
	Ix = Gx(x, y - 1);
	Iy = Gy(x, y - 1);
	Sx2 += 0.6065 * Ix * Ix;
	Sy2 += 0.6065 * Iy * Iy;
	Sxy += 0.6065 * Ix * Iy;
	Ix = Gx(x + 1, y - 1);
	Iy = Gy(x + 1, y - 1);
	Sx2 += 0.3679 * Ix * Ix;
	Sy2 += 0.3679 * Iy * Iy;
	Sxy += 0.3679 * Ix * Iy;
	Ix = Gx(x - 1, y);
	Iy = Gy(x - 1, y);
	Sx2 += 0.6065 * Ix * Ix;
	Sy2 += 0.6065 * Iy * Iy;
	Sxy += 0.6065 * Ix * Iy;
	Ix = Gx(x, y);
	Iy = Gy(x, y);
	Sx2 += Ix * Ix;
	Sy2 += Iy * Iy;
	Sxy += Ix * Iy;
	Ix = Gx(x + 1, y);
	Iy = Gy(x + 1, y);
	Sx2 += 0.6065 * Ix * Ix;
	Sy2 += 0.6065 * Iy * Iy;
	Sxy += 0.6065 * Ix * Iy;
	Ix = Gx(x - 1, y + 1);
	Iy = Gy(x - 1, y + 1);
	Sx2 += 0.3679 * Ix * Ix;
	Sy2 += 0.3679 * Iy * Iy;
	Sxy += 0.3679 * Ix * Iy;
	Ix = Gx(x, y + 1);
	Iy = Gy(x, y + 1);
	Sx2 += 0.6065 * Ix * Ix;
	Sy2 += 0.6065 * Iy * Iy;
	Sxy += 0.6065 * Ix * Iy;
	Ix = Gx(x + 1, y + 1);
	Iy = Gy(x + 1, y + 1);
	Sx2 += 0.3679 * Ix * Ix;
	Sy2 += 0.3679 * Iy * Iy;
	Sxy += 0.3679 * Ix * Iy;
	float temp = Sx2 + Sy2;
	temp *= temp;
	temp *= 0.04;
	temp = Sx2 * Sy2 - Sxy * Sxy - temp;
	return temp > 5000000000;
}

void findRightCorner(){
	int size = right_edge.size();
	right_corner.clear();
	std::vector<coor> temp;
	for(int i = 0;i<size;i++){
		if(FAST(right_edge[i].x,right_edge[i].y,threshold_FAST)){
			temp.push_back(coor{right_edge[i].x,right_edge[i].y});
		}
	}
	for(int i = 0; i<temp.size(); i++){
		if(HarrisCorner(temp[i].x,temp[i].y)){
			right_corner.push_back(coor{temp[i].x,temp[i].y});
		}
	}
}
void findLeftCorner(){
	int size = left_edge.size();
	left_corner.clear();
	std::vector<coor> temp;
	for(int i = 0;i<size;i++){
		if(FAST(left_edge[i].x,left_edge[i].y,threshold_FAST)){
			coor t = coor{left_edge[i].x,left_edge[i].y};
			temp.push_back(t);
		}
	}
	for(int i = 0; i<temp.size(); i++){
		if(HarrisCorner(temp[i].x,temp[i].y)){
			left_corner.push_back(coor{temp[i].x,temp[i].y});
		}
	}
}
double slope(const std::vector<coor>& edge){
    int n = edge.size();
    int sumX = 0;
    int sumY = 0;
    for(int i=0; i<n; ++i){
		sumX += edge[i].x;
		sumY += edge[i].y;
	}
    double avgX = sumX / n;
    double avgY = sumY / n;

    double numerator = 0.0;
    double denominator = 0.0;

    for(int i=0; i<n; ++i){
        numerator += (edge[i].x - avgX) * (edge[i].y - avgY);
        denominator += (edge[i].x - avgX) * (edge[i].x - avgX);
    }

    if(denominator == 0){
        return 1000 ;
    }

    return numerator / denominator;
}
bool IsLeftStraight(){
	int size = left_corner.size();
	if(size<60){
		int mid = size/2;
		int xdiff = (left_corner[0].x+left_corner[size-1].x)/2-left_corner[mid].x;
		int ydiff = (left_corner[0].y+left_corner[size-1].y)/2-left_corner[mid].y;
		if(xdiff>5 || ydiff>5)
			return false;
		else
			return true;
	}
	else{
		int mid = 30;
		int xdiff = (left_corner[0].x+left_corner[60].x)/2-left_corner[mid].x;
		int ydiff = (left_corner[0].y+left_corner[60].y)/2-left_corner[mid].y;
		if(xdiff>5 || ydiff>5)
			return false;
		else
			return true;
	}

}
bool IsRightStraight(){
	int size = right_corner.size();
	if(size<60){
		int mid = size/2;
		int xdiff = (right_corner[0].x+right_corner[size-1].x)/2-right_corner[mid].x;
		int ydiff = (right_corner[0].y+right_corner[size-1].y)/2-right_corner[mid].y;
		if(xdiff>5 || ydiff>5)
			return false;
		else
			return true;
	}
	else{
		int mid = 30;
		int xdiff = (right_corner[0].x+right_corner[60].x)/2-right_corner[mid].x;
		int ydiff = (right_corner[0].y+right_corner[60].y)/2-right_corner[mid].y;
		if(xdiff>5 || ydiff>5)
			return false;
		else
			return true;
	}

}

void checkState(){
	if(right_corner.size()==1 && left_corner.size()==0 && IsRightStraight() && !IsLeftStraight() ){
		if(!roundaboutR && !finishRoundR){
			roundaboutR = true;
			enterRoundR = true;
		}
	}
	else if(left_bottom_exist == true && pre_left_bottom_exist == false && enterRoundR && roundaboutR){
		enterRoundR = false;
	}
	else if(roundaboutR && !enterRoundR && left_corner.size()==1 && right_corner.size()==0){
		leaveRoundR = true; //turn right until see
	}
	else if(roundaboutR&&!enterRoundR && leaveRoundR){
		if(left_bottom_exist == true && IsLeftStraight()){
			roundaboutR = false;
			leaveRoundR = false;
			finishRoundR = true;
			leavetime = libsc::System::Time();
			return;
		}
	}

	if(left_corner.size()==1 && right_corner.size()==0 && IsLeftStraight() && !IsRightStraight() ){
		if(!roundaboutL && !finishRoundL){
			roundaboutL = true;
			enterRoundL = true;
		}
	}
	else if(right_bottom_exist == true && pre_right_bottom_exist == false && enterRoundL && roundaboutL){
		enterRoundL = false;
	}
	else if(roundaboutL && !enterRoundL && right_corner.size()==1 && left_corner.size()==0){
		leaveRoundL = true; //turn right until see
	}
	else if(roundaboutL&&!enterRoundL && leaveRoundL){
		if(right_bottom_exist == true && IsRightStraight()){
			roundaboutL = false;
			leaveRoundL = false;
			finishRoundL = true;
			leavetime = libsc::System::Time();
			return;
		}
	}


	int curtime = libsc::System::Time();
	if((curtime-leavetime)>1500){
		finishRoundR = false;
		finishRoundL = false;
	}
}


void algo() {
	libsc::System::DelayMs(100);
	libsc::Timer::TimerInt time_now = 0;
	camera->Start();
	while (1) {
		if (camera->IsAvailable()) {
			buffer = camera->LockBuffer(); //Use GetPoint(x,y) to get the gradient of the point
			camera->UnlockBuffer();
		}

		checkState();
	}
}
