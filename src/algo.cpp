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

enum Tstate{
	Normal,
	Crossroad,
	RightLoop,
	LeftLoop
};

enum Cstate{
	Detected,
	Inside,
};

enum Lstate{
	Entering,
	In,
	Leaving,
	Finished,
};

const Byte* buffer;

std::vector<coor> left_edge;
std::vector<coor> right_edge;
std::vector<int> left_edge_corner;
std::vector<int> right_edge_corner;
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
#define width 189
#define height 120
#define dist_threshold 1600

int left_edge_prev_dir;
int right_edge_prev_dir;

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

bool dist_corners(coor m, coor n){ //TODO: need to use world coordinates
	return ((m.x - n.x)*(m.x - n.x) + (m.y - n.y)*(m.y - n.y)<=dist_threshold);
}

bool check_corner(coor pivot, coor m, coor n){
    int a2 = (pivot.x - m.x)*(pivot.x - m.x) + (pivot.y - m.y)*(pivot.y - m.y);
    int b2 = (pivot.x - n.x)*(pivot.x - n.x) + (pivot.y - n.y)*(pivot.y - n.y);
    int c2 = (m.x - n.x)*(m.x - n.x) + (m.y - n.y)*(m.y - n.y);
    int pythagoras = a2 + b2 - c2;
    if(pythagoras>=0)
        return true;
    float value = (float)(pythagoras*pythagoras)/(4*a2*b2);
    if(value<0.117)
    	return true;
    return false;
}

void empty_left(){
	left_edge.clear();
	left_edge_corner.clear();
}

void empty_right(){
	right_edge.clear();
	right_edge_corner.clear();
}

void LeftEdge(coor start_point, int& edge_prev_dir, int threshold, bool append){
	if(!append)
		empty_left();
	bool guessed_corner = false;
	if(left_edge.size() && left_edge[left_edge.size()-1].y != start_point.y && left_edge[left_edge.size()-1].x != start_point.x)
		left_edge.push_back(start_point);
	while(FindLeftEdge(edge_prev_dir) && !guessed_corner){
		if(FAST(left_edge[left_edge.size()-1].x,left_edge[left_edge.size()-1].y,threshold))
			if (HarrisCorner(left_edge[left_edge.size() - 1].x, left_edge[left_edge.size() - 1].y))
				guessed_corner = true;
	}
	if(guessed_corner){
		int i=0;
		for(;i<5 && FindLeftEdge(edge_prev_dir);i++);
		if(i==4) {
			if (check_corner(left_edge[left_edge.size() - 6], left_edge[left_edge.size() - 1],
							 left_edge[left_edge.size() - 11]))
				left_edge_corner.push_back(left_edge.size() - 6);
			else
				LeftEdge(left_edge[left_edge.size() - 1], edge_prev_dir, threshold, true);
		}
	}
}

void RightEdge(coor start_point, int& edge_prev_dir, int threshold, bool append = false){
	if(!append)
		empty_right();
	bool guessed_corner = false;
	if(right_edge.size() && right_edge[right_edge.size()-1].y != start_point.y && right_edge[right_edge.size()-1].x != start_point.x)
		right_edge.push_back(start_point);
	while(FindRightEdge(edge_prev_dir) && !guessed_corner){
		if(FAST(right_edge[right_edge.size()-1].x,right_edge[right_edge.size()-1].y,threshold))
			if (HarrisCorner(right_edge[right_edge.size() - 1].x, right_edge[right_edge.size() - 1].y))
				guessed_corner = true;
	}
	if(guessed_corner){
		int i=0;
		for(;i<5 && FindRightEdge(edge_prev_dir);i++);
		if(i==4) {
			if (check_corner(right_edge[right_edge.size() - 6], right_edge[right_edge.size() - 1],
							 right_edge[right_edge.size() - 11]))
				right_edge_corner.push_back(right_edge.size() - 6);
			else
				RightEdge(right_edge[right_edge.size() - 1], edge_prev_dir, threshold, true);
		}
	}
}
//for left loop
void RightLoopEdgeL(coor start_point, int& edge_prev_dir, coor& rightmostP, int threshold,bool append){
	if(!append) {
        empty_right();
        right_edge.push_back(start_point);
    }
	if(right_edge.size() && right_edge[right_edge.size()-1].y != start_point.y && right_edge[right_edge.size()-1].x != start_point.x)
		right_edge.push_back(start_point);
	while(FindRightEdge(edge_prev_dir) && (right_edge[right_edge.size()-1].x - start_point.x > -20)){
		if(right_edge[right_edge.size()-1].x>rightmostP.x)
			rightmostP = right_edge[right_edge.size()-1];
	}
}
//for right loop
void RightLoopEdgeR(coor start_point, int& edge_prev_dir, coor& leftmostP, int threshold,bool append){
	if(!append) {
        empty_right();
        right_edge.push_back(start_point);
    }
	if(right_edge.size() && right_edge[right_edge.size()-1].y != start_point.y && right_edge[right_edge.size()-1].x != start_point.x)
		right_edge.push_back(start_point);
	while(FindRightEdge(edge_prev_dir) && (right_edge[right_edge.size()-1].x - start_point.x > -20)){
		if(right_edge[right_edge.size()-1].x<leftmostP.x)
			leftmostP = right_edge[right_edge.size()-1];
	}
}
//for right loop
void LeftLoopEdgeR(coor start_point, int& edge_prev_dir, coor& leftmostP, int threshold, bool append = false){
	if(!append)
		empty_right();
	if(left_edge.size() && left_edge[left_edge.size()-1].y != start_point.y && left_edge[left_edge.size()-1].x != start_point.x)
		left_edge.push_back(start_point);
	while(FindLeftEdge(edge_prev_dir)&& (left_edge[left_edge.size()-1].x - start_point.x < 20)){
		if(left_edge[left_edge.size()-1].x<leftmostP.x)
			leftmostP = left_edge[left_edge.size()-1];
	}
}
//for left loop
void LeftLoopEdgeL(coor start_point, int& edge_prev_dir, coor& rightmostP, int threshold, bool append = false){
	if(!append)
		empty_right();
	if(left_edge.size() && left_edge[left_edge.size()-1].y != start_point.y && left_edge[left_edge.size()-1].x != start_point.x)
		left_edge.push_back(start_point);
	while(FindLeftEdge(edge_prev_dir)&& (left_edge[left_edge.size()-1].x - start_point.x < 20)){
		if(left_edge[left_edge.size()-1].x>rightmostP.x)
			rightmostP = left_edge[left_edge.size()-1];
	}
}
bool jump(coor point1, coor point2, coor& new_start, int threshold, bool direction){ //jump from the corner to new edge. returns true if a edge is found.
	float slope = (point1.x - point2.x)/(point1.y - point2.y);						 //direction = true is for right edge, direction = false is for left edge.
	float constant = point2.x - slope*point2.y;
	new_start.y = point2.y;
	new_start.y-=3;
	new_start.x = (int16_t)(new_start.y*slope+constant);
	while(SobelEdgeDetection(new_start.x,new_start.y)<threshold && new_start.x<width-1 && new_start.x>0 && new_start.y<height-1 && new_start.y>0){
		new_start.y--;
		new_start.x = (int16_t)(new_start.y*slope+constant);
	}
	if(new_start.x<width-1 && new_start.x>0 && new_start.y<height-1 && new_start.y>0) {
		return true;
	}
	return false;
}

//void midpoint(coor& midpoint,int16_t x,int16_t y){
//	midpoint.x = x;
//	midpoint.y = y;
//}

//void set_midpoint(coor& midpoint,coor a,coor b){
//	midpoint.x = (a.x+b.x)/2;
//	midpoint.y = 115;
//}

bool right_start_point(coor midpoint, coor& right_start,int threshold){
	right_start = midpoint;
	while(SobelEdgeDetection(right_start.x,right_start.y)<threshold && right_start.x<width-1)
		right_start.x++;
	if(right_start.x<width-1)
		return true;
	return false;
}

bool left_start_point(coor midpoint, coor& left_start,int threshold){
	left_start = midpoint;
	while(SobelEdgeDetection(left_start.x,left_start.y)<threshold && left_start.x>0)
		left_start.x--;
	if(left_start.x>0)
		return true;
	return false;
}

void normal_left_corner_fsm(Tstate& track_state,coor& final_point, coor& midpoint, coor left_start, coor right_start){
	coor point1;
	if(left_edge_corner[0]>10)
		point1 = left_edge[left_edge_corner[0]-10];
	else
		point1 = left_edge[0];
	coor new_start = {0, 0};
	if (jump(point1,left_edge[left_edge_corner[0]],new_start,edge_threshold,false)){
		left_edge_prev_dir = right;
		LeftEdge(new_start,left_edge_prev_dir,edge_threshold,true);
		if(left_edge_corner.size() == 2){
			track_state = Crossroad;
//			final_point = left_edge[left_edge.size()-1];
//			set_midpoint(midpoint,left_edge[left_edge.size()-1].x + 5,left_edge[left_edge.size()-1].y);
		}
		else{
			track_state = LeftLoop;
//			final_point = left_edge[(left_edge.size()/2)-1+left_edge_corner[0]];
//			set_midpoint(midpoint,left_edge[left_edge.size()/2-1+left_edge_corner[0]].x + 5,left_edge[left_edge.size()/2-1+left_edge_corner[0]].y);
		}
	}
	else{
		track_state = Normal;
		final_point = new_start;
		midpoint = {(left_start.x+right_start.x)/2,(left_start.y+right_start.y)/2};

	}
}

void normal_right_corner_fsm(Tstate& track_state,coor& final_point, coor& midpoint, coor left_start, coor right_start){
	coor point1;
	if(right_edge_corner[0]>10)
		point1 = right_edge[right_edge_corner[0]-10];
	else
		point1 = right_edge[0];
	coor new_start = {0, 0};
	if (jump(point1,right_edge[right_edge_corner[0]],new_start,edge_threshold,true)){
		right_edge_prev_dir = left;
		RightEdge(new_start,right_edge_prev_dir,edge_threshold,true);
		if(right_edge_corner.size() == 2){
			track_state = Crossroad;
//			final_point = right_edge[right_edge.size()-1];
//			set_midpoint(midpoint,right_edge[right_edge.size()-1].x - 5,right_edge[right_edge.size()-1].y);
		}
		else{
			track_state = RightLoop;
//			final_point = right_edge[right_edge.size()/2-1+right_edge_corner[0]];
//			set_midpoint(midpoint,right_edge[(right_edge.size()/2)-1+right_edge_corner[0]].x - 5,right_edge[right_edge.size()/2-1+right_edge_corner[0]].y);
		}
	}
	else{
		track_state = Normal;
		final_point = new_start;
		midpoint = {(left_start.x+right_start.x)/2,(left_start.y+right_start.y)/2};
	}
}

void print(const std::vector<coor>& v){
	for(uint i = 0; i<v.size(); i++){
		coor t = v[i];
		lcd->SetRegion(libsc::Lcd::Rect(t.x,t.y,2,2));
		lcd->FillColor(lcd->kRed);
	}
}

void printsize(){

	char buffer[50];
	sprintf(buffer,"l %d r %d",left_edge.size(),right_edge.size());
	lcd->SetRegion(libsc::Lcd::Rect(80,40,40,40));
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
	coor midpoint = {92,115};
	coor leftmostP = {188,120};
	coor rightmostP = {0,120};

	coor left_start;
	coor right_start;
	coor final_point;
	while (1) {
		if (camera->IsAvailable()) {
			buffer = camera->LockBuffer(); //Use GetPoint(x,y) to get the gradient of the point
			camera->UnlockBuffer();
		}
		for(uint i=0;i<height;i++){
			lcd->SetRegion(libsc::Lcd::Rect(0,i,160,1));
			lcd->FillGrayscalePixel(buffer+camera->GetW()*i,160);
		}

		empty_left();
		empty_right();
		leftmostP = {188,120};
		rightmostP = {0,120};

		if(track_state == Normal){

			left_edge_prev_dir = up;
			right_edge_prev_dir = up;
			if(left_start_point(midpoint,left_start,edge_threshold))
				LeftEdge(left_start,left_edge_prev_dir,edge_threshold,false);
			if(right_start_point(midpoint,right_start,edge_threshold))
				RightEdge(right_start,right_edge_prev_dir,edge_threshold,false);

			if(left_edge.size() && right_edge.size()){
				if(left_edge_corner.size() == 1 && right_edge_corner.size() == 1){ //TODO: could be corners of different features, check distance between corners to determine.
					if(dist_corners(left_edge[left_edge_corner[0]],right_edge[right_edge_corner[0]])) {
						track_state = Crossroad;
//						final_point.x = (left_edge[left_edge_corner[0]].x + right_edge[right_edge_corner[0]].x) / 2;
//						final_point.y = (left_edge[left_edge_corner[0]].y + right_edge[right_edge_corner[0]].y) / 2;
					}
					else if(left_edge[left_edge_corner[0]].y>right_edge[right_edge_corner[0]].y){
						normal_left_corner_fsm(track_state,final_point,midpoint,left_start,right_start);
					}
					else if(left_edge[left_edge_corner[0]].y>right_edge[right_edge_corner[0]].y){
						normal_right_corner_fsm(track_state,final_point,midpoint,left_start,right_start);
					}
				}
				else if(left_edge_corner.size() == 1 && right_edge_corner.size() == 0){ //TODO: corner distance requirement needs to be rethought
					normal_left_corner_fsm(track_state,final_point,midpoint,left_start,right_start);
				}
				else if(left_edge_corner.size() == 0 && right_edge_corner.size() == 1){ //TODO: corner distance requirement needs to be rethought
					normal_right_corner_fsm(track_state,final_point,midpoint,left_start,right_start);
				}
				else{
					track_state = Normal;
					final_point.x = (left_edge[left_edge.size()-1].x + right_edge[right_edge.size()-1].x)/2;
					final_point.y = (left_edge[left_edge.size()-1].y + right_edge[right_edge.size()-1].y)/2;
					midpoint = {(left_start.x+right_start.x)/2,(left_start.y+right_start.y)/2};
				}
			}
			else if(right_edge.size()){
				if(right_edge_corner.size() == 1){
					normal_right_corner_fsm(track_state,final_point,midpoint,left_start,right_start);
				}
				else if(right_edge_corner.size() == 0);{
					track_state = Normal;
					final_point = right_edge[right_edge.size()-1];
					midpoint = {(left_start.x+right_start.x)/2,(left_start.y+right_start.y)/2};
				}
			}
			else if(left_edge.size()){
				if(left_edge_corner.size() == 1){
					normal_left_corner_fsm(track_state,final_point,midpoint,left_start,right_start);
				}
				else if(left_edge_corner.size() == 0);{
					track_state = Normal;
					final_point = left_edge[left_edge.size()-1];
					midpoint = {(left_start.x+right_start.x)/2,(left_start.y+right_start.y)/2};
				}
			}
		}
		if(track_state == Crossroad){
			if(crossroad_state == Detected){
				if(prev_track_state != Normal){
					left_edge_prev_dir = up;
					right_edge_prev_dir = up;
					if(left_start_point(midpoint,left_start,edge_threshold))
						LeftEdge(left_start,left_edge_prev_dir,edge_threshold,false);
					if(right_start_point(midpoint,right_start,edge_threshold))
						RightEdge(right_start,right_edge_prev_dir,edge_threshold,false);
				}
				if(left_edge.size() && right_edge.size()) {
					if (left_edge[left_edge_corner[0]].y > 70 || right_edge[right_edge_corner[0]].y > 70) {
						crossroad_state = Inside;
						if(prev_track_state != Normal) {
							coor point1;
							if (right_edge_corner[0] > 10)
								point1 = right_edge[right_edge_corner[0] - 10];
							else
								point1 = right_edge[0];
							coor new_start = {0, 0};
							jump(point1, right_edge[right_edge_corner[0]], new_start, edge_threshold, true);
							right_edge_prev_dir = left;
							RightEdge(new_start, right_edge_prev_dir, edge_threshold, true);

							if (left_edge_corner[0] > 10)
								point1 = left_edge[left_edge_corner[0] - 10];
							else
								point1 = left_edge[0];
							jump(point1, left_edge[left_edge_corner[0]], new_start, edge_threshold, false);
							left_edge_prev_dir = right;
							LeftEdge(new_start, left_edge_prev_dir, edge_threshold, true);
						}

						final_point={(left_edge[left_edge_corner[1]].x + right_edge[right_edge_corner[1]].x) / 2,
									 (left_edge[left_edge_corner[1]].y + right_edge[right_edge_corner[1]].y) / 2};
						midpoint = final_point;
					}
					else {
						crossroad_state = Detected;
						final_point.x = (left_edge[left_edge_corner[0]].x + right_edge[right_edge_corner[0]].x) / 2;
						final_point.y = (left_edge[left_edge_corner[0]].y + right_edge[right_edge_corner[0]].y) / 2;
						midpoint={(left_start.x+right_start.x)/2,115};
					}
				}
				else if(left_edge.size()){
					if (left_edge[left_edge_corner[0]].y > 70) {
						crossroad_state = Inside;
						if (prev_track_state != Normal) {
							coor point1;
							if (left_edge_corner[0] > 10)
								point1 = left_edge[left_edge_corner[0] - 10];
							else
								point1 = left_edge[0];
							coor new_start = {0, 0};
							jump(point1, left_edge[left_edge.size() - 1], new_start, edge_threshold, false);
							left_edge_prev_dir = right;
							LeftEdge(new_start, left_edge_prev_dir, edge_threshold, true);
						}
						final_point = left_edge[left_edge.size() - 1];
						midpoint= {left_edge[left_edge.size() - 1].x + 10, left_edge[left_edge.size() - 1].y};
					}
					else{
						crossroad_state = Detected;
						final_point = left_edge[left_edge_corner[0]];
						midpoint={(left_start.x+right_start.x)/2,115};
					}
				}
				else if(right_edge.size()){
					if (right_edge[right_edge_corner[0]].y > 70) {
						crossroad_state = Inside;
						if(prev_track_state != Normal) {
							coor point1;
							if (right_edge_corner[0] > 10)
								point1 = right_edge[right_edge_corner[0] - 10];
							else
								point1 = right_edge[0];
							coor new_start = {0, 0};
							jump(point1, right_edge[right_edge.size() - 1], new_start, edge_threshold, true);
							right_edge_prev_dir = left;
							RightEdge(new_start, right_edge_prev_dir, edge_threshold, true);
						}
						final_point = right_edge[right_edge.size()-1];
						midpoint={right_edge[right_edge.size()-1].x - 10,right_edge[right_edge.size()-1].y};
					}
					else{
						crossroad_state = Detected;
						final_point = right_edge[right_edge_corner[0]];
						midpoint={(left_start.x+right_start.x)/2,115};
					}
				}
			}
			else if(crossroad_state == Inside){
				left_edge_prev_dir = down;
				right_edge_prev_dir = down;
				if(left_start_point(midpoint,left_start,edge_threshold))
					LeftEdge(right_start,left_edge_prev_dir,edge_threshold,false);
				if(right_start_point(midpoint,right_start,edge_threshold))
					RightEdge(left_start,right_edge_prev_dir,edge_threshold,false);

				if(left_edge_corner.size() == 1 && right_edge_corner.size() == 1){
					final_point = {(right_edge[right_edge_corner[0]].x+left_edge[left_edge_corner[0]].x)/2,
									 (right_edge[right_edge_corner[0]].y+left_edge[left_edge_corner[0]].y)/2};
					midpoint=final_point;
				}
				else if(left_edge_corner.size()==1){
					left_edge_prev_dir = up;
					LeftEdge(left_start,left_edge_prev_dir,edge_threshold,false);
					final_point = left_edge[left_edge.size()-1];
					midpoint={(left_start.x+right_start.x)/2,(left_start.y+right_start.y)/2};
				}
				else if(right_edge_corner.size()==1){
					right_edge_prev_dir = up;
					RightEdge(right_start,right_edge_prev_dir,edge_threshold,false);
					final_point = right_edge[right_edge.size()-1];
					midpoint={(left_start.x+right_start.x)/2,(left_start.y+right_start.y)/2};
				}
				else{
					track_state = Normal;
					crossroad_state = Detected;
					left_edge_prev_dir = up;
					right_edge_prev_dir = up;
					if(left_start_point(midpoint,left_start,edge_threshold))
						LeftEdge(left_start,left_edge_prev_dir,edge_threshold,false);
					if(right_start_point(midpoint,right_start,edge_threshold))
						RightEdge(right_start,right_edge_prev_dir,edge_threshold,false);
					if(left_edge.size() && right_edge.size()){
						final_point = {(left_edge[left_edge.size()-1].x + right_edge[right_edge.size()-1].x)/2,
									   (left_edge[left_edge.size()-1].y + right_edge[right_edge.size()-1].y)/2};
					}
					else if(left_edge.size()){
						final_point = left_edge[left_edge.size()-1];
					}
					else if(right_edge.size()) {
						final_point = right_edge[right_edge.size()-1];
					}
					midpoint={(left_start.x+right_start.x)/2,115};;
				};
			}


		}
		if(track_state == RightLoop){
			//follow the right edge when Entering
			if(loop_state == Entering){
				if(prev_track_state == Normal){
//					left_edge_prev_dir = up;
//					right_edge_prev_dir = up;
//					if(left_start_point(midpoint,left_start,edge_threshold)){
//						LeftEdge(left_start,left_edge_prev_dir,edge_threshold,false);
//					}
//					if(right_start_point(midpoint,right_start,edge_threshold))
//						RightEdge(right_start,right_edge_prev_dir,edge_threshold,false);
//					coor point1;
//					if(right_edge_corner[0]>10)
//						point1 = right_edge[right_edge_corner[0]-10];
//					else
//						point1 = right_edge[0];
//					coor new_start = {0, 0};
//					if (jump(point1,right_edge[right_edge_corner[0]+4],new_start,edge_threshold,true)){
//						right_edge_prev_dir = left;
//						RightLoopEdgeR(new_start,right_edge_prev_dir,leftmostP,edge_threshold,true);
//						midpoint = {leftmostP.x-10,leftmostP.y};
//						right_edge_prev_dir = up;
//						if(right_start_point(midpoint,right_start,edge_threshold))
//							RightEdge(right_start,right_edge_prev_dir,edge_threshold,false);
					for(int i = 0; i< right_edge.size();i++){
						if(right_edge[i].x<leftmostP.x)
							leftmostP = right_edge[i];
					}
					midpoint = {leftmostP.x-10,leftmostP.y};
					right_edge_prev_dir = up;
					if(right_start_point(midpoint,right_start,edge_threshold))
					RightEdge(right_start,right_edge_prev_dir,edge_threshold,false);
				}
				else if(prev_track_state == RightLoop){
					left_edge_prev_dir = down;
					right_edge_prev_dir = down_left;
					if(left_start_point(midpoint,left_start,edge_threshold)){
						//as the direction is down, use rightedge function
						//and the left edge will be stored in right_edge
						RightEdge(left_start,left_edge_prev_dir,edge_threshold,false);
					}
					if(right_start_point(midpoint,right_start,edge_threshold)){
						//opposite as well
						LeftLoopEdgeR(right_start,right_edge_prev_dir,leftmostP,edge_threshold,true);
					}
					midpoint = {leftmostP.x-10,leftmostP.y};

					left_edge = right_edge;
					left_edge_corner = right_edge_corner;

					right_edge_prev_dir = up;
					if(right_start_point(midpoint,right_start,edge_threshold))
						RightEdge(right_start,right_edge_prev_dir,edge_threshold,false);

					if(left_edge_corner.size() == 1){
						loop_state = In;
						midpoint = {(left_start.x+right_start.x)/2,115};
					}
				}
			}
			//follow the normal way when In
			else if(loop_state == In){
				left_edge_prev_dir = up;
				right_edge_prev_dir = up;
				if(left_start_point(midpoint,left_start,edge_threshold))
					LeftEdge(left_start,left_edge_prev_dir,edge_threshold,false);
				if(right_start_point(midpoint,right_start,edge_threshold))
					RightEdge(right_start,right_edge_prev_dir,edge_threshold,false);
				midpoint = {(left_start.x+right_start.x)/2,(left_start.y+right_start.y)/2};
				if(left_edge_corner.size() == 1 && right_edge_corner.size() == 0){
					loop_state = Leaving;
				}
			}
			//follow the right edges when Leaving
			else if(loop_state == Leaving){
				left_edge_prev_dir = up;
				right_edge_prev_dir = up;
				if(left_start_point(midpoint,left_start,edge_threshold))
					LeftEdge(left_start,left_edge_prev_dir,edge_threshold,false);
				if(right_start_point(midpoint,right_start,edge_threshold))
					RightEdge(right_start,right_edge_prev_dir,edge_threshold,false);
				midpoint = {(left_start.x+right_start.x)/2,(left_start.y+right_start.y)/2};
				if(left_edge.size()>0 && left_edge_corner.size()==0){
					loop_state = Finished;
				}
			}
			//follow the left edges when Finished
			else if(loop_state == Finished){
				//increase the midpoint height for detecting the corner?
				midpoint = {midpoint.x,100};
				left_edge_prev_dir = up;
				right_edge_prev_dir = down;
				std::vector<coor> temp_edge;
				std::vector<int> temp_edge_corner;
				if(left_start_point(midpoint,left_start,edge_threshold)){
					LeftEdge(left_start,left_edge_prev_dir,edge_threshold,false);
					temp_edge = left_edge;
					temp_edge_corner = left_edge_corner;
				}
				if(right_start_point(midpoint,right_start,edge_threshold))
					LeftEdge(right_start,right_edge_prev_dir,edge_threshold,false);
				right_edge = left_edge;
				right_edge_corner = left_edge_corner;
				left_edge = temp_edge;
				left_edge_corner = temp_edge_corner;
				midpoint = {(left_start.x+right_start.x)/2,(left_start.y+right_start.y)/2};
				if(right_edge_corner.size()==1){
					track_state = Normal;
					loop_state = Entering;
				}
			}
		}
		if(track_state == LeftLoop){
			//follow the right edge when Entering
			if(loop_state == Entering){
				if(prev_track_state == Normal){
					for(int i = 0; i< left_edge.size();i++){
						if(left_edge[i].x<rightmostP.x)
							rightmostP = left_edge[i];
					}
					midpoint = {rightmostP.x-10,rightmostP.y};
					left_edge_prev_dir = up;
					if(left_start_point(midpoint,left_start,edge_threshold))
					LeftEdge(left_start,left_edge_prev_dir,edge_threshold,false);
				}
				else if(prev_track_state == LeftLoop){
					right_edge_prev_dir = down;
					left_edge_prev_dir = down_left;
					if(right_start_point(midpoint,right_start,edge_threshold)){
						//as the direction is down, use rightedge function
						//and the left edge will be stored in right_edge
						LeftEdge(right_start,right_edge_prev_dir,edge_threshold,false);
					}
					if(left_start_point(midpoint,left_start,edge_threshold)){
						//opposite as well
						RightLoopEdgeL(right_start,right_edge_prev_dir,rightmostP,edge_threshold,true);
					}
					midpoint = {rightmostP.x-10,rightmostP.y};

					right_edge = left_edge;
					right_edge_corner = left_edge_corner;

					left_edge_prev_dir = up;
					if(left_start_point(midpoint,left_start,edge_threshold))
						LeftEdge(left_start,left_edge_prev_dir,edge_threshold,false);

					if(right_edge_corner.size() == 1){
						loop_state = In;
						midpoint = {(left_start.x+right_start.x)/2,115};
					}
				}
			}
			//follow the normal way when In
			else if(loop_state == In){
				left_edge_prev_dir = up;
				right_edge_prev_dir = up;
				if(left_start_point(midpoint,left_start,edge_threshold))
					LeftEdge(left_start,left_edge_prev_dir,edge_threshold,false);
				if(right_start_point(midpoint,right_start,edge_threshold))
					RightEdge(right_start,right_edge_prev_dir,edge_threshold,false);
				midpoint = {(left_start.x+right_start.x)/2,(left_start.y+right_start.y)/2};
				if(right_edge_corner.size() == 1 && left_edge_corner.size() == 0){
					loop_state = Leaving;
				}
			}
			//follow the right edges when Leaving
			else if(loop_state == Leaving){
				left_edge_prev_dir = up;
				right_edge_prev_dir = up;
				if(left_start_point(midpoint,left_start,edge_threshold))
					LeftEdge(left_start,left_edge_prev_dir,edge_threshold,false);
				if(right_start_point(midpoint,right_start,edge_threshold))
					RightEdge(right_start,right_edge_prev_dir,edge_threshold,false);
				midpoint = {(left_start.x+right_start.x)/2,(left_start.y+right_start.y)/2};
				if(right_edge.size()>0 && right_edge_corner.size()==0){
					loop_state = Finished;
				}
			}
			//follow the left edges when Finished
			else if(loop_state == Finished){
				//increase the midpoint height for detecting the corner?
				midpoint = {midpoint.x,100};
				right_edge_prev_dir = up;
				left_edge_prev_dir = down;
				std::vector<coor> temp_edge;
				std::vector<int> temp_edge_corner;
				if(right_start_point(midpoint,right_start,edge_threshold)){
					RightEdge(right_start,right_edge_prev_dir,edge_threshold,false);
					temp_edge = right_edge;
					temp_edge_corner = right_edge_corner;
				}
				if(left_start_point(midpoint,left_start,edge_threshold))
					RightEdge(left_start,left_edge_prev_dir,edge_threshold,false);
				left_edge = right_edge;
				left_edge_corner = right_edge_corner;
				right_edge = temp_edge;
				right_edge_corner = temp_edge_corner;
				midpoint = {(left_start.x+right_start.x)/2,(left_start.y+right_start.y)/2};
				if(left_edge_corner.size()==1){
					track_state = Normal;
					loop_state = Entering;
				}
			}
		}

	prev_track_state = track_state;
	print(left_edge);
	print(right_edge);
	printsize();
	}
}

