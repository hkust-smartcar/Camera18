/*
 * algo.cpp
 *
 *  Created on: May 30, 2018
 *      Author: LeeChunHei
 */

#include <algo.h>

const Byte* buffer;

inline Byte GetPoint(uint8_t x, uint8_t y) {
	return buffer[x+y*189];
}

inline int16_t SobelEdgeDetection(uint8_t x, uint8_t y) {
	return std::abs(-GetPoint((x) - 1, (y) - 1) - 2 * GetPoint((x) - 1, (y)) - GetPoint((x) - 1, (y) + 1) + GetPoint((x) + 1, (y) - 1) + 2 * GetPoint((x) + 1, (y)) + GetPoint((x) + 1, (y) + 1)) + std::abs(-GetPoint((x) - 1, (y) - 1) - 2 * GetPoint((x), (y) - 1) - GetPoint((x) + 1, (y) - 1) + GetPoint((x) - 1, (y) + 1) + 2 * GetPoint((x), (y) + 1) + GetPoint((x) + 1, (y) + 1));
}

inline int16_t RobertsEdgeDetection(uint8_t x, uint8_t y) {
	return std::abs(GetPoint(x, y) - GetPoint(x + 1, y + 1)) + std::abs(GetPoint(x + 1, y) - GetPoint(x, y + 1));
}

std::vector<coor> left_edge;
std::vector<coor> right_edge;
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
