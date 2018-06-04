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

bool LeftEdge(coor start_point, int& edge_prev_dir, int threshold, bool append = false){ // returns false if abnormal number of corners found
	if(!append) {
		left_edge.clear();
		left_edge_corner.clear();
	}
	left_edge.push_back(start_point);
	while(FindLeftEdge(edge_prev_dir)){
		if(FAST(left_edge[left_edge.size()-1].x,left_edge[left_edge.size()-1].y,threshold))
			if (HarrisCorner(left_edge[left_edge.size() - 1].x, left_edge[left_edge.size() - 1].y))
				left_edge_corner.push_back(left_edge.size()-1);
	}
	if((append && left_edge_corner.size() <= 2) || (!append && left_edge_corner.size() <= 1))
		return true;
	return false;
}

bool RightEdge(coor start_point, int& edge_prev_dir, int threshold, bool append = false){ // returns false if abnormal number of corners found
	if(!append) {
		right_edge.clear();
		right_edge_corner.clear();
	}
	right_edge.push_back(start_point);
	while(FindRightEdge(edge_prev_dir)){
		if(FAST(right_edge[right_edge.size()-1].x,right_edge[right_edge.size()-1].y,threshold))
			if (HarrisCorner(right_edge[right_edge.size() - 1].x, right_edge[right_edge.size() - 1].y))
				right_edge_corner.push_back(right_edge.size()-1);
	}
	if((append && right_edge_corner.size() <= 2) || (!append && right_edge_corner.size() <= 1))
		return true;
	return false;
}

bool jump(coor point1, coor point2, coor& new_start, int threshold){ //jump from the corner to new edge. returns true if a edge is found
	float slope = (point1.y - point2.y)/(point1.x - point2.x);
	float constant = point2.y - slope*point2.x;
	int16_t y = point2.y;
	y-=3;
	int16_t x = (int16_t)((y-constant)/slope);
	while(SobelEdgeDetection(x,y)<threshold && x<width-1 && x>0 && y<height-1 && y>0){
		y--;
		x = (int16_t)((y-constant)/slope);
	}
	if(SobelEdgeDetection(x,y)>=threshold) {
		new_start.x = x;
		new_start.y = y;
		return true;
	}
	return false;
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
	}
}
