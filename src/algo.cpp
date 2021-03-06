/*
 * algo.cpp
 *
 *  Created on: May 30, 2018
 *      Author: Amrutavarsh S Kinagi
 *          LeeChunHei
 */

#include <algo.h>
#include <flash_storage.h>

enum Tstate {
    Normal, Crossroad, RightLoop, LeftLoop, RightObs, LeftObs, StartLine, Stop
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

enum Ostate {
    Approach, Pass
};

const Byte *buffer;

inline Byte GetPoint(uint8_t x, uint8_t y) {
    return buffer[x + y * 189];
}

inline int16_t SobelEdgeDetection(uint8_t x, uint8_t y) {
    return std::abs(-GetPoint((x) - 1, (y) - 1) - 2 * GetPoint((x) - 1, (y)) - GetPoint((x) - 1, (y) + 1) +
                    GetPoint((x) + 1, (y) - 1) + 2 * GetPoint((x) + 1, (y)) + GetPoint((x) + 1, (y) + 1)) + std::abs(
            -GetPoint((x) - 1, (y) - 1) - 2 * GetPoint((x), (y) - 1) - GetPoint((x) + 1, (y) - 1) +
            GetPoint((x) - 1, (y) + 1) + 2 * GetPoint((x), (y) + 1) + GetPoint((x) + 1, (y) + 1));
}

float target_speed = 530;
float Kp = 0.03;
float Ki = 0.0004;
float Kd = 0.0002;
float servo_P = 0.55; //0.55
float servo_D = 13.0;
float search_distance = std::pow(target_speed * servo_P, 2);

std::pair<float, float> target;
std::pair<float, float> speed_target_point;
//img2world[91][119] is the bottom mid point of the image, may need to tune later
const float search_origin_x = img2world[99][110][0];
const float search_origin_y = img2world[99][110][1];

const float search_left_align_x = img2world[48][110][0];
const float search_left_align_y = img2world[48][110][1];

const float search_right_align_x = img2world[150][110][0];
const float search_right_align_y = img2world[150][110][1];

coor left_end_point;
coor right_end_point;
bool left_end_point_found;
bool right_end_point_found;
bool left_target_found;
bool right_target_found;
bool left_speed_target_found;
bool right_speed_target_found;

bool right_jump = false;
bool left_jump = false;

bool right_obs = false;
bool left_obs = false;

int obs_width = 0;
coor right_obs_coor = {0, 0};
coor left_obs_coor = {0, 0};

int distance_from_edge = 6;

inline void InitSearchDistance(float mpu_angle) {
//    mpu_angle *= 0.00008726646;    //angle = angular_speed * 5ms * PI / 180
//    search_m = -std::tan(mpu_angle);
//    search_c = (search_distance * std::cos(mpu_angle) - search_m * (search_distance * std::sin(mpu_angle) - search_origin_x));
    left_end_point_found = false;
    right_end_point_found = false;
    left_target_found = false;
    right_target_found = false;
    left_speed_target_found = false;
    right_speed_target_found = false;
}

inline bool FindLeftEndPoint(int x, int y) {
    float rx = img2world[x][y][0] - search_left_align_x;
    rx *= rx;
    rx = search_distance - rx;
    float ry = img2world[x][y][1] - search_left_align_y;
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
#define slope_threshold 1
int left_edge_prev_dir;
int right_edge_prev_dir;

Align align = center_align;

float midpoint_offset = 200;

Tstate track_state = Normal;
Tstate prev_track_state = Normal;
Cstate crossroad_state = Detected;
Lstate loop_state = Entering;
Ostate obstacle_state = Approach;

int startline_count = 0;

inline void FindEndPointFromLeft(int front_x, int front_y, int end_x, int end_y) {
    float temp_1 = end_x - front_x;
    float temp_2 = front_y - end_y;
    float vector_mag = std::sqrt(temp_1 * temp_1 + temp_2 * temp_2);
    float midpoint_x = midpoint_offset * (temp_2) / vector_mag + end_x;
    float midpoint_y = midpoint_offset * (temp_1) / vector_mag + end_y;
    float rx = midpoint_x - search_origin_x;
    rx *= rx;
    rx = search_distance - rx;
    float ry = midpoint_y - search_origin_y;
    ry *= ry;
    if (rx < ry && track_state != Tstate::LeftObs) {
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
}

inline void FindEndPointFromRight(int front_x, int front_y, int end_x, int end_y) {
    float temp_1 = front_x - end_x;
    float temp_2 = end_y - front_y;
    float vector_mag = std::sqrt(temp_1 * temp_1 + temp_2 * temp_2);
    float midpoint_x = midpoint_offset * (temp_2) / vector_mag + end_x;
    float midpoint_y = midpoint_offset * (temp_1) / vector_mag + end_y;
    float rx = midpoint_x - search_origin_x;
    rx *= rx;
    rx = search_distance - rx;
    float ry = midpoint_y - search_origin_y;
    ry *= ry;
    if (rx < ry && track_state != Tstate::RightObs) {
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
}

inline void FindSpeedEndPointFromLeft(int front_x, int front_y, int end_x, int end_y) {
    float temp_1 = end_x - front_x;
    float temp_2 = front_y - end_y;
    float vector_mag = std::sqrt(temp_1 * temp_1 + temp_2 * temp_2);
    float midpoint_x = midpoint_offset * (temp_2) / vector_mag + end_x;
    float midpoint_y = midpoint_offset * (temp_1) / vector_mag + end_y;
    float rx = midpoint_x - search_origin_x;
    rx *= rx;
    rx = search_distance * 3.24 - rx;
    float ry = midpoint_y - search_origin_y;
    ry *= ry;
    if (rx < ry && track_state != Tstate::LeftObs) {
        left_speed_target_found = true;
        speed_target_point.first = right_speed_target_found ? (speed_target_point.first + midpoint_x) / 2 : midpoint_x;
        speed_target_point.second = right_speed_target_found ? (speed_target_point.second + midpoint_y) / 2
                                                             : midpoint_y;
        if (right_speed_target_found) {
            float rx = speed_target_point.first - search_origin_x;
            rx *= rx;
            rx = search_distance * 3.24 - rx;
            float ry = speed_target_point.second - search_origin_y;
            ry *= ry;
            if (rx > ry) {
                speed_target_point.first = midpoint_x;
                speed_target_point.second = midpoint_y;
            }
        }
    }
}

inline void FindSpeedEndPointFromRight(int front_x, int front_y, int end_x, int end_y) {
    float temp_1 = front_x - end_x;
    float temp_2 = end_y - front_y;
    float vector_mag = std::sqrt(temp_1 * temp_1 + temp_2 * temp_2);
    float midpoint_x = midpoint_offset * (temp_2) / vector_mag + end_x;
    float midpoint_y = midpoint_offset * (temp_1) / vector_mag + end_y;
    float rx = midpoint_x - search_origin_x;
    rx *= rx;
    rx = search_distance * 3.24 - rx;
    float ry = midpoint_y - search_origin_y;
    ry *= ry;
    if (rx < ry && track_state != Tstate::RightObs) {
        right_speed_target_found = true;
        speed_target_point.first = left_speed_target_found ? (speed_target_point.first + midpoint_x) / 2 : midpoint_x;
        speed_target_point.second = left_speed_target_found ? (speed_target_point.second + midpoint_y) / 2 : midpoint_y;
        if (left_speed_target_found) {
            float rx = speed_target_point.first - search_origin_x;
            rx *= rx;
            rx = search_distance * 3.24 - rx;
            float ry = speed_target_point.second - search_origin_y;
            ry *= ry;
            if (rx > ry) {
                speed_target_point.first = midpoint_x;
                speed_target_point.second = midpoint_y;
            }
        }
    }
}

bool FindLeftEdge(int &edge_prev_dir) {
    int size = left_edge.size();
    if (size > 100 || !size) {
        return false;
    }
    coor edge_coor = left_edge.back();
    int x = edge_coor.x;
    int y = edge_coor.y;
    if (x < 4 || x > 184 || y < 4 || y >= 107) {
        return false;
    }
//    if (!left_end_point_found && size % 5 == 0) {
//        if (FindLeftEndPoint(x, y)) {
//            left_end_point_found = true;
//            left_end_point= {x,y};
//        }
//    }
    if (!left_target_found && size % 4 == 0) {
        FindEndPointFromLeft(img2world[left_edge[size - 4].x][left_edge[size - 4].y][0],
                             img2world[left_edge[size - 4].x][left_edge[size - 4].y][1],
                             img2world[left_edge[size - 1].x][left_edge[size - 1].y][0],
                             img2world[left_edge[size - 1].x][left_edge[size - 1].y][1]);
    }
    if (!left_speed_target_found && size % 4 == 0) {
        FindSpeedEndPointFromLeft(img2world[left_edge[size - 4].x][left_edge[size - 4].y][0],
                                  img2world[left_edge[size - 4].x][left_edge[size - 4].y][1],
                                  img2world[left_edge[size - 1].x][left_edge[size - 1].y][0],
                                  img2world[left_edge[size - 1].x][left_edge[size - 1].y][1]);
    }
    edge_prev_dir += 2;
    for (int i = 0; i < 5; i++) {
        edge_prev_dir =
                edge_prev_dir < 0 ? 8 + edge_prev_dir : edge_prev_dir > up_right ? edge_prev_dir - 8 : edge_prev_dir;
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

bool FindRightEdge(int &edge_prev_dir) {
    int size = right_edge.size();
    if (size > 100 || !size) {
        return false;
    }
    coor edge_coor = right_edge.back();
    int x = edge_coor.x;
    int y = edge_coor.y;
    if (x < 4 || x > 184 || y < 4 || y >= 107) {
        return false;
    }
//    if (!right_end_point_found && size % 5 == 0) {
//        if (FindRightEndPoint(x, y)) {
//            right_end_point_found = true;
//            right_end_point= {x,y};
//        }
//    }
    if (!right_target_found && size % 4 == 0) {
        FindEndPointFromRight(img2world[right_edge[size - 4].x][right_edge[size - 4].y][0],
                              img2world[right_edge[size - 4].x][right_edge[size - 4].y][1],
                              img2world[right_edge[size - 1].x][right_edge[size - 1].y][0],
                              img2world[right_edge[size - 1].x][right_edge[size - 1].y][1]);
    }
    if (!right_speed_target_found && size % 4 == 0) {
        FindSpeedEndPointFromRight(img2world[right_edge[size - 4].x][right_edge[size - 4].y][0],
                                   img2world[right_edge[size - 4].x][right_edge[size - 4].y][1],
                                   img2world[right_edge[size - 1].x][right_edge[size - 1].y][0],
                                   img2world[right_edge[size - 1].x][right_edge[size - 1].y][1]);
    }
    edge_prev_dir -= 2;
    for (int i = 0; i < 5; i++) {
        edge_prev_dir =
                edge_prev_dir < 0 ? 8 + edge_prev_dir : edge_prev_dir > up_right ? edge_prev_dir - 8 : edge_prev_dir;
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

void FindJumpStart(int &prev_edge_dir) {
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
                        temp = {x + 1, y - 1};
                        dir.push_back(7);
                    } else {
                        temp = {x, y - 1};
                        dir.push_back(i);
                    }
                    point.push_back(temp);
                    prev_white = !prev_white;
                }
                break;
            case up_left:
                if (prev_white != (SobelEdgeDetection(x - 1, y - 1) < edge_threshold)) {
                    if (prev_white) {
                        temp = {x, y - 1};
                        dir.push_back(0);
                    } else {
                        temp = {x - 1, y - 1};
                        dir.push_back(i);
                    }
                    point.push_back(temp);
                    prev_white = !prev_white;
                }
                break;
            case left:
                if (prev_white != (SobelEdgeDetection(x - 1, y) < edge_threshold)) {
                    if (prev_white) {
                        temp = {x - 1, y - 1};
                        dir.push_back(1);
                    } else {
                        temp = {x - 1, y};
                        dir.push_back(i);
                    }
                    point.push_back(temp);
                    prev_white = !prev_white;
                }
                break;
            case down_left:
                if (prev_white != (SobelEdgeDetection(x - 1, y + 1) < edge_threshold)) {
                    if (prev_white) {
                        temp = {x - 1, y};
                        dir.push_back(2);
                    } else {
                        temp = {x - 1, y + 1};
                        dir.push_back(i);
                    }
                    point.push_back(temp);
                    prev_white = !prev_white;
                }
                break;
            case down:
                if (prev_white != (SobelEdgeDetection(x, y + 1) < edge_threshold)) {
                    if (prev_white) {
                        temp = {x - 1, y + 1};
                        dir.push_back(3);
                    } else {
                        temp = {x, y + 1};
                        dir.push_back(i);
                    }
                    point.push_back(temp);
                    prev_white = !prev_white;
                }
                break;
            case down_right:
                if (prev_white != (SobelEdgeDetection(x + 1, y + 1) < edge_threshold)) {
                    if (prev_white) {
                        temp = {x, y + 1};
                        dir.push_back(4);
                    } else {
                        temp = {x + 1, y + 1};
                        dir.push_back(i);
                    }
                    point.push_back(temp);
                    prev_white = !prev_white;
                }
                break;
            case right:
                if (prev_white != (SobelEdgeDetection(x + 1, y) < edge_threshold)) {
                    if (prev_white) {
                        temp = {x + 1, y + 1};
                        dir.push_back(5);
                    } else {
                        temp = {x + 1, y};
                        dir.push_back(i);
                    }
                    point.push_back(temp);
                    prev_white = !prev_white;
                }
                break;
            case up_right:
                if (prev_white != (SobelEdgeDetection(x + 1, y - 1) < edge_threshold)) {
                    if (prev_white) {
                        temp = {x + 1, y};
                        dir.push_back(6);
                    } else {
                        temp = {x + 1, y - 1};
                        dir.push_back(i);
                    }
                    point.push_back(temp);
                    prev_white = !prev_white;
                }
                break;
        }
    }
    if (prev_edge_dir == left) {
        coor final = {5, 5};
        for (int i = 0; i < point.size(); i++) {
            if ((point[i].x > final.x) || (GetPoint(point[i].x, point[i].y) > GetPoint(final.x, final.y))) {
                final = point[i];
                prev_edge_dir = dir[i];
            }
        }
        left_edge.push_back(final);
//        if (point.front().x > point.back().x) {
//            left_edge.push_back(point.front());
//            prev_edge_dir = dir.front();
//        } else {
//            left_edge.push_back(point.back());
//            prev_edge_dir = dir.back();
//        }
    } else {
        coor final = {186, 5};
        for (int i = 0; i < point.size(); i++) {
            if ((point[i].x < final.x) || (GetPoint(point[i].x, point[i].y) > GetPoint(final.x, final.y))) {
                final = point[i];
                prev_edge_dir = dir[i];
            }
        }
//        if (point.front().x < point.back().x) {
//            right_edge.push_back(point.front());
//            prev_edge_dir = dir.front();
//        } else {
//            right_edge.push_back(point.back());
//            prev_edge_dir = dir.back();
//        }
    }
}

bool dist_corners(coor m, coor n) {
    int x1 = img2world[m.x][m.y][0];
    int y1 = img2world[m.x][m.y][1];
    int x2 = img2world[n.x][n.y][0];
    int y2 = img2world[n.x][n.y][1];
    return ((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2) <= dist_threshold);
}

int world_dist(coor m, coor n) {
    int x1 = img2world[m.x][m.y][0];
    int y1 = img2world[m.x][m.y][1];
    int x2 = img2world[n.x][n.y][0];
    int y2 = img2world[n.x][n.y][1];
    return std::sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}

bool check_corner(coor pivot, coor m, coor n) {
    if (left_jump || right_jump) {
        int a2 = (img2world[pivot.x][pivot.y][0] - img2world[m.x][m.y][0]) *
                 (img2world[pivot.x][pivot.y][0] - img2world[m.x][m.y][0]) +
                 (img2world[pivot.x][pivot.y][1] - img2world[m.x][m.y][1]) *
                 (img2world[pivot.x][pivot.y][1] - img2world[m.x][m.y][1]);
        a2 /= 100;
        int b2 = (img2world[pivot.x][pivot.y][0] - img2world[n.x][n.y][0]) *
                 (img2world[pivot.x][pivot.y][0] - img2world[n.x][n.y][0]) +
                 (img2world[pivot.x][pivot.y][1] - img2world[n.x][n.y][1]) *
                 (img2world[pivot.x][pivot.y][1] - img2world[n.x][n.y][1]);
        b2 /= 100;
        int c2 = (img2world[m.x][m.y][0] - img2world[n.x][n.y][0]) * (img2world[m.x][m.y][0] - img2world[n.x][n.y][0]) +
                 (img2world[m.x][m.y][1] - img2world[n.x][n.y][1]) * (img2world[m.x][m.y][1] - img2world[n.x][n.y][1]);
        c2 /= 100;
        int pythagoras = a2 + b2 - c2;
        if (pythagoras >= 0) {
            return true;
        }
        pythagoras = std::abs(pythagoras);
        uint64_t pythagoras_uint = (uint64_t) pythagoras;
        float value = ((((float) ((uint64_t) (pythagoras_uint * pythagoras_uint)) / 4) / a2) / b2);
        return value < 0.2;
    } else {
        int c2 = (m.x - n.x) * (m.x - n.x) + (m.y - n.y) * (m.y - n.y);
        if (c2 > 170) {
            return false;
        }
        int a2 = (pivot.x - m.x) * (pivot.x - m.x) + (pivot.y - m.y) * (pivot.y - m.y);
        int b2 = (pivot.x - n.x) * (pivot.x - n.x) + (pivot.y - n.y) * (pivot.y - n.y);
        int pythagoras = a2 + b2 - c2;
        if (pythagoras >= 0) {
            return true;
        }
        float value = (float) (pythagoras * pythagoras) / (4 * a2 * b2);
        if (value < 0.06) {
            return true;
        }
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

void LeftEdge(coor start_point, int &edge_prev_dir, bool append) {
    int direction = edge_prev_dir;
    if (!append) {
        left_edge.clear();
        left_edge_corner.clear();
        left_edge.push_back(start_point);
    }
    if (left_edge.size() && left_edge.back().y != start_point.y && left_edge.back().x != start_point.x) {
        left_edge.push_back(start_point);
    } else if (!left_edge.size()) {
        left_edge.push_back(start_point);
    }

    while (FindLeftEdge(edge_prev_dir)) {
        if ((direction == up && left_edge.back().y > start_point.y + 10) ||
            (direction == down && left_edge.back().y < start_point.y - 10) ||
            (direction == left && left_edge.back().x > start_point.x + 10) ||
            (direction == right && left_edge.back().x < start_point.x - 10)) {
                break;
        }
        if (left_edge.size() > 15 && left_edge[left_edge.size() - 11].y > 30 &&
            check_corner(left_edge[left_edge.size() - 11], left_edge[left_edge.size() - 1],
                         left_edge[left_edge.size() - 16])) {
            left_edge_corner.push_back(left_edge.size() - 11);
            break;
        }
        if (!left_jump && (track_state == Normal || (track_state == LeftObs && obstacle_state == Approach)) &&
            left_edge.size() > 7 && left_edge.back().y > 30) {
            float slope;
            if ((left_edge[left_edge.size() - 1].x - left_edge[left_edge.size() - 5].x) != 0) {
                slope = ((float) (left_edge[left_edge.size() - 1].y - left_edge[left_edge.size() - 5].y) /
                         (left_edge[left_edge.size() - 1].x - left_edge[left_edge.size() - 5].x));
            } else {
                slope = 100;
            }
            if (slope > slope_threshold || slope < -slope_threshold) { //slope>slope_threshold ||
                if (slope == 100) {
                    slope = 0;
                } else {
                    slope = (-1) / slope;
                }
                float constant = left_edge[left_edge.size() - 3].y - left_edge[left_edge.size() - 3].x * slope;
                int v1x = left_edge[left_edge.size() - 1].x - left_edge[left_edge.size() - 5].x;
                int v1y = left_edge[left_edge.size() - 1].y - left_edge[left_edge.size() - 5].y;

                int tx = left_edge[left_edge.size() - 3].x + distance_from_edge;
                int ty = tx * slope + constant;

                if (distance_from_edge * v1y < (ty - left_edge[left_edge.size() - 3].y) * v1x) {
                    int i = 0;
                    constant = tx - ty * slope;
                    for (; i < 3 && ty < height - 14 && ty > 3 && tx > 3 && tx < width - 4 &&
                           SobelEdgeDetection(tx, ty) < edge_threshold; i++) {
                        ty--;
                        tx = ty * slope + constant;
//                        lcd->SetRegion(libsc::St7735r::Rect(tx, ty, 1, 1));
//                        lcd->FillColor(lcd->kRed);
                    }
                    if (i < 3 && ty < height - 14 && ty > 3 && tx > 3 && tx < width - 4) {
                        left_obs = true;
                        left_obs_coor = {tx, ty};
                        break;
                    }
                }
            }
        }
    }
}

void RightEdge(coor start_point, int &edge_prev_dir, bool append) {
    int direction = edge_prev_dir;
    if (!append) {
        right_edge.clear();
        right_edge_corner.clear();
        right_edge.push_back(start_point);
    }
    if (right_edge.size() && right_edge[right_edge.size() - 1].y != start_point.y &&
        right_edge[right_edge.size() - 1].x != start_point.x) {
            right_edge.push_back(start_point);
    } else if (!right_edge.size()) {
            right_edge.push_back(start_point);
    }

    while (FindRightEdge(edge_prev_dir)) {
        if ((direction == up && right_edge[right_edge.size() - 1].y > start_point.y + 10) ||
            (direction == down && right_edge[right_edge.size() - 1].y < start_point.y - 10) ||
            (direction == left && right_edge[right_edge.size() - 1].x > start_point.x + 10) ||
            (direction == right && right_edge[right_edge.size() - 1].x < start_point.x - 10)) {
                break;
        }
        if (right_edge.size() > 15 && right_edge[right_edge.size() - 11].y > 30 &&
            check_corner(right_edge[right_edge.size() - 11], right_edge[right_edge.size() - 1],
                         right_edge[right_edge.size() - 16])) {
            right_edge_corner.push_back(right_edge.size() - 11);
            break;
        }
        if (!right_jump && (track_state == Normal || (track_state == RightObs && obstacle_state == Approach)) &&
            right_edge.size() > 7 && right_edge.back().y > 30) {
            float slope;
            if ((right_edge[right_edge.size() - 1].x - right_edge[right_edge.size() - 5].x) != 0) {
                slope = (float) (right_edge[right_edge.size() - 1].y - right_edge[right_edge.size() - 5].y) /
                        (right_edge[right_edge.size() - 1].x - right_edge[right_edge.size() - 5].x);
            } else {
                slope = -100;
            }

            if (slope > slope_threshold || slope < -slope_threshold) {
                if (slope == -100) {
                    slope = 0;
                } else {
                    slope = (-1) / (slope);
                }
                float constant = right_edge[right_edge.size() - 3].y - right_edge[right_edge.size() - 3].x * slope;
                int v1x = right_edge[right_edge.size() - 1].x - right_edge[right_edge.size() - 5].x;
                int v1y = right_edge[right_edge.size() - 1].y - right_edge[right_edge.size() - 5].y;

                int tx = right_edge[right_edge.size() - 3].x - distance_from_edge;
                int ty = tx * slope + constant;

                if (-distance_from_edge * v1y > (ty - right_edge[right_edge.size() - 3].y) * v1x) {
                    int i = 0;
                    constant = tx - ty * slope;
                    for (; i < 3 && ty < height - 14 && ty > 3 && tx > 3 && tx < width - 4 &&
                           SobelEdgeDetection(tx, ty) < edge_threshold; i++) {
                        ty--;
                        tx = ty * slope + constant;
//                        lcd->SetRegion(libsc::St7735r::Rect(tx, ty, 1, 1));
//                        lcd->FillColor(lcd->kBlue);
                    }
                    if (i < 3 && ty < height - 14 && ty > 3 && tx > 3 && tx < width - 4) {
                        right_obs = true;
                        right_obs_coor = {tx, ty};
                        break;
                    }
                }
            }
        }
    }
}

//for left loop
void RightLoopEdgeL(coor start_point, int &edge_prev_dir, coor &rightmostP, bool append) {
    if (!append) {
        empty_right();
    }
    right_edge.push_back(start_point);
    if (right_edge[0].x > rightmostP.x) {
        rightmostP = right_edge[0];
    }
    while (FindRightEdge(edge_prev_dir) && ((right_edge[right_edge.size() - 1].x - start_point.x) > -20)) {
        if (right_edge[right_edge.size() - 1].y > 35 && right_edge[right_edge.size() - 1].x > rightmostP.x) {
            rightmostP = right_edge[right_edge.size() - 1];
        }
    }
}

//for right loop
void RightLoopEdgeR(coor start_point, int &edge_prev_dir, coor &leftmostP, bool append) {
    if (!append) {
        empty_right();
    }

    right_edge.push_back(start_point);
    if (right_edge[0].x < leftmostP.x) {
        leftmostP = right_edge[0];
    }
    while (FindRightEdge(edge_prev_dir) && ((right_edge[right_edge.size() - 1].x - start_point.x) > -20)) {
        if (right_edge[right_edge.size() - 1].y > 35 && right_edge[right_edge.size() - 1].x < leftmostP.x) {
            leftmostP = right_edge[right_edge.size() - 1];
        }
    }
}

//for right loop
void LeftLoopEdgeR(coor start_point, int &edge_prev_dir, coor &leftmostP, bool append) {
    if (!append) {
        empty_left();
    }
    left_edge.push_back(start_point);
    if (left_edge[0].x < leftmostP.x) {
        leftmostP = left_edge[0];
    }
    while (FindLeftEdge(edge_prev_dir) && (left_edge[left_edge.size() - 1].x - start_point.x < 20)) {
        if (left_edge[left_edge.size() - 1].x > 35 && left_edge[left_edge.size() - 1].x < leftmostP.x) {
            leftmostP = left_edge[left_edge.size() - 1];
        }
    }
}

//for left loop
void LeftLoopEdgeL(coor start_point, int &edge_prev_dir, coor &rightmostP, bool append) {
    if (!append) {
        empty_left();
    }
    //if (left_edge.size() && left_edge[left_edge.size() - 1].y != start_point.y && left_edge[left_edge.size() - 1].x != start_point.x)
    left_edge.push_back(start_point);
    if (left_edge[0].x > rightmostP.x) {
        rightmostP = left_edge[0];
    }
    while (FindLeftEdge(edge_prev_dir) && (left_edge[left_edge.size() - 1].x - start_point.x < 20)) {
        if (left_edge[left_edge.size() - 1].y > 35 && left_edge[left_edge.size() - 1].x > rightmostP.x) {
            rightmostP = left_edge[left_edge.size() - 1];
        }
    }
}

bool jump(coor corner, coor point1, coor point2, coor &new_start,
          bool direction) { //jump from the corner to new edge. returns true if a edge is found.
    float slope = (float) (point1.x - corner.x) / (point1.y - corner.y);
    float constant = point2.x - slope * point2.y;
    new_start.y = point2.y - 15;
    new_start.x = (int16_t) (new_start.y * slope + constant);
    coor prev_start = {0, 0};
    while (new_start.x < width - 4 && new_start.x > 3 && new_start.y < height - 14 && new_start.y > 3 &&
           SobelEdgeDetection(new_start.x, new_start.y) < edge_threshold) {
//        lcd->SetRegion(libsc::St7735r::Lcd::Rect(new_start.x, new_start.y, 1, 1));
//        lcd->FillColor(lcd->kBlue);
        prev_start = new_start;
        new_start.y--;
        new_start.x = (new_start.y * slope + constant);
    }
    new_start = prev_start;
    if (new_start.x < width - 4 && new_start.x > 3 && new_start.y < height - 14 && new_start.y > 3) {
        if (direction) {
            right_jump = true;
        } else {
            left_jump = true;
        }
        return true;
    }
    return false;
}

bool Ljump(coor &new_start) {
    coor point1 = {0, 0};
    coor point2 = {0, 0};
    if (left_edge_corner[0] > 10) {
        point1 = left_edge[left_edge_corner[0] - 10];
    } else {
        point1 = left_edge[0];
    }
    int i = left_edge_corner[0];
    for (; FindLeftEdge(left_edge_prev_dir) &&
           value_corner(left_edge[left_edge.size() - 11], left_edge[left_edge.size() - 1],
                        left_edge[left_edge.size() - 16]) < 0.9; i++);
    point2 = left_edge[i + 3];
    return jump(left_edge[left_edge_corner[0]], point1, point2, new_start, false);
}

bool Rjump(coor &new_start) {
    coor point1 = {0, 0};
    coor point2 = {0, 0};
    if (right_edge_corner[0] > 10) {
        point1 = right_edge[right_edge_corner[0] - 10];
    } else {
        point1 = right_edge[0];
    }
    int i = right_edge_corner[0];
    for (; FindRightEdge(right_edge_prev_dir) &&
           value_corner(right_edge[right_edge.size() - 11], right_edge[right_edge.size() - 1],
                        right_edge[right_edge.size() - 16]) < 0.9; i++);
    point2 = right_edge[i + 3];
    return jump(right_edge[right_edge_corner[0]], point1, point2, new_start, true);
}

bool right_start_point(coor midpoint, coor &right_start, int threshold) {
    right_start = midpoint;
    while (right_start.x < width - 4 && right_start.x > 3 &&
           SobelEdgeDetection(right_start.x, right_start.y) < threshold)
        right_start.x++;
    return right_start.x < width - 4;
}

bool left_start_point(coor midpoint, coor &left_start, int threshold) {
    left_start = midpoint;
    while (left_start.x > 3 && left_start.x < width - 4 && SobelEdgeDetection(left_start.x, left_start.y) < threshold)
        left_start.x--;
    return left_start.x > 3;
}

void normal_left_corner_fsm(Tstate &track_state, coor &final_point, coor &midpoint, coor left_start, coor right_start,
                            int &edge_prev_dir) {
    align = left_align;
    coor new_start = {0, 0};
    if (Ljump(new_start)) {
        left_edge_prev_dir = right;
        left_edge.push_back(new_start);
//        FindJumpStart(left_edge_prev_dir);
        LeftEdge(left_edge.back(), left_edge_prev_dir, false);
        if (left_edge_corner.size() == 1) {
            track_state = Crossroad;
        } else {
            track_state = LeftLoop;
        }
    } else {
        if (right_edge.size()) {
            track_state = LeftLoop;
        } else {
            final_point = new_start;
            midpoint = {(left_start.x + right_start.x) / 2, 105};
        }
    }
}

void normal_right_corner_fsm(Tstate &track_state, coor &final_point, coor &midpoint, coor left_start, coor right_start,
                             int &edge_prev_dir) {
    align = right_align;
    coor new_start = {0, 0};
    if (Rjump(new_start)) {
        right_edge_prev_dir = left;
        right_edge.push_back(new_start);
//        FindJumpStart(right_edge_prev_dir);
        RightEdge(right_edge.back(), right_edge_prev_dir, false);
        if (right_edge_corner.size() == 1) {
            track_state = Crossroad;
        } else {
            track_state = RightLoop;
        }
    } else {
        if (left_edge.size()) {
            track_state = RightLoop;
        } else {
            final_point = new_start;
            midpoint = {(left_start.x + right_start.x) / 2, 105};
        }
    }
}


bool CheckLeftEdgeSmallerThan(int thres) {
    for (int i = 0; i < left_edge.size(); i++) {
        if (left_edge[i].y < thres) {
            return true;
        }
    }
    return false;
}

bool CheckRightEdgeSmallerThan(int thres) {
    for (int i = 0; i < right_edge.size(); i++) {
        if (right_edge[i].y < thres) {
            return true;
        }
    }
    return false;
}

int CheckStartLine(int y, int leftthres, int rightthres, int threshold) {
    int count = 0;
    int currx = 0;
    int prevx = 0;
    obs_width = 0;
    if (y > 108 || y < 4) {
        return false;
    }
    int x = leftthres;

    while (x < rightthres) {
        if (y < height - 12 && y > 3 && x > 3 && x < width - 4 &&
            SobelEdgeDetection(x, y) >= threshold) { //&& SobelEdgeDetection(x,y) >= threshold
//            lcd->SetRegion(libsc::Lcd::Rect(x, y, 2, 2));
//            lcd->FillColor(lcd->kRed);
            count++;
            currx = x;
            x = x + 3;

            coor temp1 = {prevx, y};
            coor temp2 = {currx, y};
            if (count > 1) {
                if (world_dist(temp1, temp2) > obs_width) {
                    obs_width = world_dist(temp1, temp2);
                }
            }
            prevx = currx;
        }
        x++;

    }

//    char buffer[50];
//        sprintf(buffer, "count %d %d",count,obs_width);
//        lcd->SetRegion(libsc::Lcd::Rect(0, 90, 100, 40));
//        writerP->WriteString(buffer);
    return count;
}


void CheckCoor(coor &coor) {
    int x = coor.x;
    int y = coor.y;
    if (y < height - 14 && y > 3 && x > 3 && x < width - 4) {
        return;
    }
    if (x <= 3) {
        coor.x = 4;
    } else if (x >= width - 4) {
        coor.x = width - 5;
    }
    if (y <= 3) {
        coor.y = 4;
    } else if (y >= height - 14) {
        coor.y = height - 15;
    }

}

void print(const std::vector<coor> &v) {
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
//    Tstate track_state = Normal;
//    Tstate prev_track_state = Normal;
//    Cstate crossroad_state = Detected;
//    Lstate loop_state = Entering;
    bool entertype; //true for enter with left/rightmostP; false for follow edge first
    coor midpoint = {99, 105};

    coor left_start = {0, 0};
    coor right_start = {0, 0};
    coor final_point = {0, 0};

    coor left_obs_target = {0, 0};
    coor right_obs_target = {0, 0};

    int no_movement_count = 0;
    int cum_error = 0;
    int prev_error = 0;
    float prev_count = 0;
    float error_1 = 0;
    float error_2 = 0;
    float error_3 = 0;

    int degree_1 = 0;
    int degree_2 = 0;
    int degree_3 = 0;
    int prev_angle = 0;

    bool debug = true;

    uint32_t sum_enc = 0;
    uint32_t enc_count = 0;

    InitBluetoothDebug(&debug);
//    PushTuneFloat("servo_D\n", (Byte*) &servo_D);
//    PushTuneInt32("speed\n", (Byte*) &target_speed);
//    PushTuneFloat("search_c\n", (Byte*) &search_const);
//    InitValueDebug();
    libsc::Timer::TimerInt motor_start_time = libsc::System::Time();
    bool motor_start = true; //false;
    int count_encoder = 0;
    int set_angle = 920;
    int current_angle = 920;
    float cal_servo_angle = 920;
    int servo_angle = 920;
    target_speed = 1200;
    libsc::Timer::TimerInt debug_end_time;
    libsc::Timer::TimerInt motor_stop_time;
    bool start_count = false;
    while (true) {
        if (camera->IsAvailable()) {
            if (!debug) {
//                if (target_speed < 610)
//                target_speed += 0.1;
//                search_distance = std::pow(target_speed * 0.4, 2);
//                char buffer[100] = { };
//                sprintf(buffer, "%.2f,%.2f\n", target_speed, search_distance);
//                bt->SendStr(buffer);
            }
            if (search_distance < 9200) {
                search_distance = 9200;
            }
//            float search_distance = std::pow(target_speed * search_const, 2);
            time_now = libsc::System::Time();
            buffer = camera->LockBuffer(); //Use GetPoint(x,y) to get the gradient of the point
            camera->UnlockBuffer();
            if (debug) {
                if (joystick->GetState() == libsc::Joystick::State::kSelect) {
                    debug = false;
                    encoder->Update();
                    debug_end_time = libsc::System::Time();
                }
            }
            if (debug) {
                for (uint16_t i = 0; i < height; i++) {
                    lcd->SetRegion(libsc::Lcd::Rect(0, i, 160, 1));
                    lcd->FillGrayscalePixel(buffer + camera->GetW() * i, 160);
                }
            }

            if (!debug) {

                if (servo->GetDegree() > current_angle) {
                    current_angle += std::min(20, servo->GetDegree() - current_angle);
                } else if (servo->GetDegree() < current_angle) {
                    current_angle += std::max(-20, servo->GetDegree() - current_angle);
                }

//                set_angle += (int) (current_angle - set_angle) * 0.4;
                double angle_degree = (current_angle - 920) * 0.1;
                double angle_degree_2 = std::atan((speed_target_point.first - search_origin_x) / (speed_target_point.second - search_origin_y)) * 180 / 3.14;
                int temp;
                int temp_2;
                float tuning_param = FlashStorage::data.slope_param;
                int min_speed = FlashStorage::data.min_speed;
                int max_speed = FlashStorage::data.max_speed;
                float x_shift = FlashStorage::data.x_shift;

                if (angle_degree < 0) {
                    angle_degree = -angle_degree;
                }
                angle_degree_2 = std::abs(angle_degree_2);

                temp = ((max_speed - min_speed) /
                        (1 + exp(tuning_param * ((angle_degree * x_shift / 40) - (x_shift - 1))))) + min_speed;
                temp_2 = ((max_speed - min_speed) /
                          (1 + exp(tuning_param * ((angle_degree_2 * x_shift / 40) - (x_shift - 1))))) + min_speed;

                if (temp > target_speed) {
                    if ((left_speed_target_found || right_speed_target_found) && (temp_2 < temp)) {
                        target_speed = 0.9 * target_speed + 0.1 * temp_2;
                    } else {
                        target_speed = 0.9 * target_speed + 0.1 * temp;
                    }
                } else {
                    if ((left_speed_target_found || right_speed_target_found) && (temp_2 < temp)) {
                        target_speed = temp_2;
                    } else {
                        target_speed = temp;
                    }
                }

                if (is_using_constant_mode) {
                    target_speed = FlashStorage::data.constant_speed;
                }

                if (track_state == Tstate::Stop) {
                    target_speed = 0;
                } else if (servo_angle < 350) {
//                    target_speed = 650;
                } else if (servo_angle > 1490) {
//                    target_speed = 650;
                } else if (servo->GetDegree() <= 500) {
                    target_speed = 750;
                } else if (servo->GetDegree() >= 1340) {
                    target_speed = 750;
                } else if (servo->GetDegree() > 805 && servo->GetDegree() < 865 && angle_degree_2 >= 5 &&
                           -prev_count > 1000 && track_state != StartLine && !is_using_constant_mode) {
                    target_speed = 800;
                }

                if ((libsc::System::Time() - debug_end_time < 2000)) {
                    target_speed = 750;
                }
                if (libsc::System::Time() - debug_end_time < 1000) {
                    target_speed = 0;
                }

//                char speed[10] = { };
//                sprintf(speed, "%d", (int) angle_degree_2);
//                console->Clear(true);
//                console->SetCursorRow(4);
//                console->WriteString(speed);

                search_distance = std::pow(target_speed * servo_P, 2);
                error_1 = error_2;
                error_2 = error_3;
                encoder->Update();
                float curr = encoder->GetCount();

                if (!start_count && track_state == StartLine && obstacle_state == Pass) {
                    start_count = true;
                } else if (start_count && track_state == Stop) {
                    start_count = false;
                }
                if (start_count) {
                    sum_enc -= curr;
                    enc_count++;
                }
                if (std::abs(curr) < 100) {
                    ++no_movement_count;
                    cum_error = 0;
                } else {
                    no_movement_count = 0;
                }
                if (std::abs(curr) > 9200) {
                    curr = target_speed;
                }
                curr = 0.9 * curr + 0.1 * prev_count;
                curr = curr > 9200 ? prev_error : curr;
                float error = target_speed + curr;    //curr is negative when forward
                cum_error += error;
                error_3 = -prev_count + curr;
                prev_count = curr;
                float volt = Kp * error + Ki * cum_error + Kd * (error_3 + error_2 + error_1) / 3.0;

                int speed_set = -(volt / battery_meter->GetVoltage() * 1000);
                if ((no_movement_count > 5 && target_speed) && libsc::System::Time() - debug_end_time > 2050 ||
                    !motor_start) {
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
                if (libsc::System::Time() - motor_stop_time > 3000 && track_state == Stop) {
                    motor->SetPower(0);
                }
                prev_error = error;
                if (-prev_count > target_speed) {
                    search_distance = std::pow(-prev_count * servo_P, 2);
                }
                if (search_distance < std::pow(min_speed * servo_P, 2)) {
                    search_distance = std::pow(min_speed * servo_P, 2);
                }

//                char buffer[50];
//                sprintf(buffer, "t %d ", curr);
//                lcd->SetRegion(libsc::Lcd::Rect(30, 60, 40, 15));
//                writerP->WriteString(buffer);
            } else {
                motor->SetPower(0);
                cum_error = 0;
            }

//            mpu->UpdateF();

            InitSearchDistance(0); //-(mpu->GetOmegaF()[2]));

            if (debug) {
//                char buff[10]={};
//                sprintf(buff,"%.2f",mpu->GetOmegaF()[2]);
//                console->Clear(false);
//                console->SetCursorRow(4);
//                console->WriteString(buff);
            }

            empty_left();
            empty_right();

            coor leftmostP = {188, 120};
            coor rightmostP = {0, 120};

            left_obs = false;
            right_obs = false;

            if (track_state == Normal) {
                left_edge_prev_dir = up;
                right_edge_prev_dir = up;
                if (left_start_point(midpoint, left_start, edge_threshold)) {
                    LeftEdge(left_start, left_edge_prev_dir, false);
                }
                if (right_start_point(midpoint, right_start, edge_threshold)) {
                    RightEdge(right_start, right_edge_prev_dir, false);
                }
                if (left_obs && !right_obs) {
                    int tempcount = CheckStartLine(left_obs_coor.y - 5, left_obs_coor.x - 15,
                                                   (left_start.x + right_start.x) / 2 + 10, edge_threshold);
                    if ((tempcount == 3 || tempcount == 4) && obs_width > 90 && obs_width < 140) {
                        track_state = LeftObs;
                    }

                } else if (!left_obs && right_obs) {
                    int tempcount = CheckStartLine(right_obs_coor.y - 5, (left_start.x + right_start.x) / 2 - 10,
                                                   right_obs_coor.x + 15, edge_threshold);
                    if ((tempcount == 3 || tempcount == 4) && obs_width > 90 && obs_width < 140) {
                        track_state = RightObs;
                    }

                } else if (left_obs && right_obs) {
                    track_state = StartLine;
                } else if (left_edge.size() && right_edge.size()) {
                    if (left_edge_corner.size() == 1 && right_edge_corner.size() == 1) {
                        if (dist_corners(left_edge[left_edge_corner[0]], right_edge[right_edge_corner[0]])) {
                            if ((right_edge[right_edge_corner[0]].y > 85) || left_edge[left_edge_corner[0]].y > 85) {
                                track_state = Crossroad;
                            } else {
                                final_point = {
                                        (left_edge[left_edge_corner[0]].x + right_edge[right_edge_corner[0]].x) / 2,
                                        (left_edge[left_edge_corner[0]].y + right_edge[right_edge_corner[0]].y) / 2};
                                align = center_align;
                                midpoint = {(left_start.x + right_start.x) / 2, 105};
                            }
                        } else if (left_edge[left_edge_corner[0]].y > right_edge[right_edge_corner[0]].y) {
                            if (left_edge[left_edge_corner[0]].y > 85) {
                                normal_left_corner_fsm(track_state, final_point, midpoint, left_start, right_start,
                                                       left_edge_prev_dir);
                            } else {
                                final_point = left_end_point.y > left_edge[left_edge_corner[0]].y
                                              ? left_edge[left_edge_corner[0]] : left_end_point;
                                align = left_align;
                                midpoint = {(left_start.x + right_start.x) / 2, 105};
                            }
                        } else if (left_edge[left_edge_corner[0]].y < right_edge[right_edge_corner[0]].y) {
                            if (right_edge[right_edge_corner[0]].y > 85) {
                                normal_right_corner_fsm(track_state, final_point, midpoint, left_start, right_start,
                                                        right_edge_prev_dir);
                            } else {
                                final_point = right_end_point.y > right_edge[right_edge_corner[0]].y
                                              ? right_edge[right_edge_corner[0]] : right_end_point;
                                align = right_align;
                                midpoint = {(left_start.x + right_start.x) / 2, 105};
                            }
                        }
                    } else if (left_edge_corner.size() == 1 && right_edge_corner.size() == 0) {
                        if (left_edge[left_edge_corner[0]].y > 85) {
                            normal_left_corner_fsm(track_state, final_point, midpoint, left_start, right_start,
                                                   left_edge_prev_dir);
                        } else {
                            final_point =
                                    left_end_point.y > left_edge[left_edge_corner[0]].y ? left_edge[left_edge_corner[0]]
                                                                                        : left_end_point;
                            align = left_align;
                            midpoint = {(left_start.x + right_start.x) / 2, 105};
                        }
                    } else if (left_edge_corner.size() == 0 && right_edge_corner.size() == 1) {
                        if (right_edge[right_edge_corner[0]].y > 85) {
                            normal_right_corner_fsm(track_state, final_point, midpoint, left_start, right_start,
                                                    right_edge_prev_dir);
                        } else {
                            final_point = right_end_point.y > right_edge[right_edge_corner[0]].y
                                          ? right_edge[right_edge_corner[0]] : right_end_point;
                            align = right_align;
                            midpoint = {(left_start.x + right_start.x) / 2, 105};
                        }
                    } else {
                        align = center_align;
                        final_point = {(left_end_point.x + right_end_point.x) / 2,
                                       (left_end_point.y + right_end_point.y) / 2};
                        midpoint = {(left_start.x + right_start.x) / 2, 105};
                    }
                } else if (right_edge.size()) {
                    if (right_edge_corner.size() == 1 && right_edge[right_edge_corner[0]].y > 85) {
                        normal_right_corner_fsm(track_state, final_point, midpoint, left_start, right_start,
                                                right_edge_prev_dir);
                    } else {
                        align = right_align;
                        final_point = right_end_point;
                        midpoint = {(left_start.x + right_start.x) / 2, 105};
                    }
                } else if (left_edge.size()) {
                    if (left_edge_corner.size() == 1 && left_edge[left_edge_corner[0]].y > 85) {
                        normal_left_corner_fsm(track_state, final_point, midpoint, left_start, right_start,
                                               right_edge_prev_dir);
                    } else {
                        align = left_align;
                        final_point = left_end_point;
                        midpoint = {(left_start.x + right_start.x) / 2, 105};
                    }
                }
            }
            if (track_state == Crossroad) {
                if (crossroad_state == Detected) {
                    if (!left_jump && !right_jump) {

                        coor new_start = {0, 0};

                        Rjump(new_start);
                        right_edge_prev_dir = left;
                        right_edge.push_back(new_start);
                        RightEdge(right_edge.back(), right_edge_prev_dir, false);

                        Ljump(new_start);
                        left_edge_prev_dir = right;
                        left_edge.push_back(new_start);
                        LeftEdge(left_edge.back(), left_edge_prev_dir, false);

                        if (left_edge_corner.size() == 1 && right_edge_corner.size() == 1) {
                            align = center_align;
                            crossroad_state = C_inside;
                            if (right_edge[right_edge_corner[0]].x + 15 < 185) {
                                right_start = {right_edge[right_edge_corner[0]].x + 15,
                                               right_edge[right_edge_corner[0]].y + 20};
                            } else {
                                right_start = {184, right_edge[right_edge_corner[0]].y + 20};
                            }

                            if (left_edge[left_edge_corner[0]].x - 15 > 5) {
                                left_start = {left_edge[left_edge_corner[0]].x - 15,
                                              left_edge[left_edge_corner[0]].y + 20};
                            } else {
                                left_start = {6, left_edge[left_edge_corner[0]].y + 20};
                            }
                            final_point = {
                                    (left_edge[left_edge_corner[0]].x + right_edge[right_edge_corner[0]].x) / 2,
                                    (left_edge[left_edge_corner[0]].y + right_edge[right_edge_corner[0]].y) / 2};
                        } else if (left_edge_corner.size() == 1) {
                            align = left_align;
                            crossroad_state = L_inside;
                            if (left_edge[left_edge_corner[0]].x - 15 > 5) {
                                left_start = {left_edge[left_edge_corner[0]].x - 15,
                                              left_edge[left_edge_corner[0]].y + 20};
                            } else {
                                left_start = {6, left_edge[left_edge_corner[0]].y + 20};
                            }
                            final_point = left_edge[left_edge_corner[0]];
                        } else if (right_edge_corner.size() == 1) {
                            align = right_align;
                            crossroad_state = R_inside;
                            if (right_edge[right_edge_corner[0]].x + 15 < 185) {
                                right_start = {right_edge[right_edge_corner[0]].x + 15,
                                               right_edge[right_edge_corner[0]].y + 20};
                            } else {
                                right_start = {184, right_edge[right_edge_corner[0]].y + 20};
                            }
                            final_point = right_edge[right_edge_corner[0]];
                        }
                    } else if (left_jump) {
                        align = left_align;
                        crossroad_state = L_inside;
                        if (left_edge[left_edge_corner[0]].x - 15 > 5) {
                            left_start = {left_edge[left_edge_corner[0]].x - 15, left_edge[left_edge_corner[0]].y + 20};
                        } else {
                            left_start = {6, left_edge[left_edge_corner[0]].y + 20};
                        }
                        final_point = left_edge[left_edge_corner[0]];
                    } else if (right_jump) {
                        align = right_align;
                        crossroad_state = R_inside;
                        if (right_edge[right_edge_corner[0]].x + 15 < 185) {
                            right_start = {right_edge[right_edge_corner[0]].x + 15,
                                           right_edge[right_edge_corner[0]].y + 20};
                        } else {
                            right_start = {184, right_edge[right_edge_corner[0]].y + 20};
                        }
                        final_point = right_edge[right_edge_corner[0]];
                    }
                } else if (crossroad_state == C_inside) {
                    right_jump = true;
                    while (SobelEdgeDetection(right_start.x, right_start.y) < edge_threshold && right_start.y > 10)
                        right_start.y--;
                    right_edge_prev_dir = left;
                    right_edge.push_back(right_start);
                    RightEdge(right_edge.back(), right_edge_prev_dir, false);

                    left_jump = true;
                    while (SobelEdgeDetection(left_start.x, left_start.y) < edge_threshold && left_start.y > 10)
                        left_start.y--;
                    left_edge_prev_dir = right;
                    left_edge.push_back(left_start);
                    LeftEdge(left_edge.back(), left_edge_prev_dir, false);

                    align = center_align;
                    if (right_edge[right_edge_corner[0]].x + 15 < 185) {
                        right_start = {right_edge[right_edge_corner[0]].x + 15,
                                       right_edge[right_edge_corner[0]].y + 20};
                    } else {
                        right_start = {184, right_edge[right_edge_corner[0]].y + 20};
                    }

                    if (left_edge[left_edge_corner[0]].x - 15 > 5) {
                        left_start = {left_edge[left_edge_corner[0]].x - 15, left_edge[left_edge_corner[0]].y + 20};
                    } else {
                        left_start = {6, left_edge[left_edge_corner[0]].y + 20};
                    }
                    final_point = {(left_edge[left_edge_corner[0]].x + right_edge[right_edge_corner[0]].x) / 2,
                                   (left_edge[left_edge_corner[0]].y + right_edge[right_edge_corner[0]].y) / 2};

                    if (!FindRightEndPoint(right_edge.back().x, right_edge.back().y) &&
                        !FindLeftEndPoint(left_edge.back().x, left_edge.back().y)) {
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
                    LeftEdge(left_edge.back(), left_edge_prev_dir, false);

                    align = left_align;
                    if (left_edge[left_edge_corner[0]].x - 15 > 5) {
                        left_start = {left_edge[left_edge_corner[0]].x - 15, left_edge[left_edge_corner[0]].y + 20};
                    } else {
                        left_start = {6, left_edge[left_edge_corner[0]].y + 20};
                    }
                    final_point = left_edge[left_edge_corner[0]];

                    if (!FindLeftEndPoint(left_edge.back().x, left_edge.back().y)) {
                        crossroad_state = Outside;
                        if (final_point.x + 20 < 185) {
                            midpoint = {final_point.x + 20, final_point.y - 10};
                        } else {
                            midpoint = {184, final_point.y - 10};
                        }
                    }
                } else if (crossroad_state == R_inside) {
                    right_jump = true;
                    while (SobelEdgeDetection(right_start.x, right_start.y) < edge_threshold && right_start.y > 10)
                        right_start.y--;
                    right_edge_prev_dir = left;
                    right_edge.push_back(right_start);
                    RightEdge(right_edge.back(), right_edge_prev_dir, false);

                    align = right_align;
                    if (right_edge[right_edge_corner[0]].x + 15 < 185) {
                        right_start = {right_edge[right_edge_corner[0]].x + 15,
                                       right_edge[right_edge_corner[0]].y + 15};
                    } else {
                        right_start = {184, right_edge[right_edge_corner[0]].y + 15};
                    }
                    final_point = right_edge[right_edge_corner[0]];

                    if (!FindRightEndPoint(right_edge.back().x, right_edge.back().y)) {
                        crossroad_state = Outside;
                        if (final_point.x - 20 > 5) {
                            midpoint = {final_point.x - 20, final_point.y - 10};
                        } else {
                            midpoint = {6, final_point.y - 15};
                        }
                    }
                } else if (crossroad_state == Outside) {
                    left_edge_prev_dir = down;
                    right_edge_prev_dir = down;
                    left_jump = true;
                    right_jump = true;
                    if (left_start_point(midpoint, left_start, edge_threshold)) {
                        RightEdge(left_start, right_edge_prev_dir, false);
                    }
                    if (right_start_point(midpoint, right_start, edge_threshold)) {
                        LeftEdge(right_start, left_edge_prev_dir, false);
                    }

                    left_target_found = false;
                    right_target_found = false;
                    left_speed_target_found = false;
                    right_speed_target_found = false;

                    if (midpoint.y < 95) {
                        if (right_edge_corner.size() && left_edge_corner.size()) {
                            midpoint = {(right_edge[right_edge_corner[0]].x + left_edge[left_edge_corner[0]].x) / 2,
                                        (right_edge[right_edge_corner[0]].y + left_edge[left_edge_corner[0]].y) / 2 -
                                        10};
                        } else if (left_edge_corner.size()) {
                            midpoint = {left_edge[left_edge_corner[0]].x - 40, left_edge[left_edge_corner[0]].y - 10};
                        } else if (right_edge_corner.size()) {
                            midpoint = {right_edge[right_edge_corner[0]].x + 40,
                                        right_edge[right_edge_corner[0]].y - 10};
                        } else {
                            midpoint = {(right_edge.back().x + left_edge.back().x) / 2,
                                        ((right_edge.back().y + left_edge.back().y) / 2 - 10)};
                        }
                        left_jump = false;
                        right_jump = false;
                        left_edge_prev_dir = up;
                        right_edge_prev_dir = up;
                        RightEdge(right_start, right_edge_prev_dir, false);
                        LeftEdge(left_start, left_edge_prev_dir, false);
                    } else {
                        left_edge_prev_dir = up;
                        right_edge_prev_dir = up;
                        RightEdge(right_start, right_edge_prev_dir, false);
                        LeftEdge(left_start, left_edge_prev_dir, false);
                        track_state = Normal;
                        crossroad_state = Detected;
                        midpoint.y = 105;
                    }
                }
            }
            if (track_state == RightLoop) {
                //follow the right edge when Entering
                //right align
                if (loop_state == Entering) {
                    if (prev_track_state == Normal) {
                        if (right_jump) {
                            for (int i = right_edge.size() - 1; i > 0; i--) {
                                if (right_edge[i].x < leftmostP.x && right_edge[i].y > 30) {
                                    leftmostP = right_edge[i];
                                }
                            }

                            final_point = {leftmostP.x, leftmostP.y};
                            align = right_align;
                            midpoint = {leftmostP.x - 10, leftmostP.y};
                            entertype = true;
                        } else {
                            midpoint.y = 84;
                            entertype = false;
                        }

                    } else if (prev_track_state == RightLoop) {
                        if (entertype) {
                            left_edge_prev_dir = down;
                            right_edge_prev_dir = down;
                            if (left_start_point({midpoint.x, 85}, left_start, edge_threshold)) {
                                //as the direction is down, use rightedge function
                                //and the left edge will be stored in right_edge
                                RightEdge(left_start, left_edge_prev_dir, false);
                            }
                            if (right_start_point(midpoint, right_start, edge_threshold)) {
                                //opposite as well
                                LeftLoopEdgeR(right_start, right_edge_prev_dir, leftmostP, false);
                            }
                            if (leftmostP.y != 120) {
                                midpoint = {leftmostP.x - 10, leftmostP.y};
                            }
                            CheckCoor(midpoint);

                            if (right_edge_corner.size() == 1 && right_edge[right_edge_corner[0]].y > 95) {
                                loop_state = In;
                                midpoint = {(left_start.x + right_start.x) / 2, 85};
                                CheckCoor(midpoint);
                            } else {
                                right_edge_prev_dir = up;
                                left_end_point_found = false;
                                right_end_point_found = false;
                                left_target_found = false;
                                right_target_found = false;
                                coor tempmidpoint = {midpoint.x, midpoint.y};
                                if (leftmostP.y > 105 && leftmostP.y != 120) {
                                    tempmidpoint.y = 105;
                                }

                                if (right_start_point(tempmidpoint, right_start, edge_threshold)) {
                                    RightLoopEdgeR(right_start, right_edge_prev_dir, leftmostP, false);
                                }
                                if (leftmostP.y != 120) {
                                    midpoint = {leftmostP.x - 10, leftmostP.y};
                                } else {
                                    midpoint = {midpoint.x, midpoint.y - 10};
                                }
                                CheckCoor(midpoint);

                                align = right_align;
                                if (right_end_point_found) {
                                    final_point = right_end_point;                            //right_edge.back();
                                } else if (right_edge.size()) {
                                    final_point = right_edge.back();
                                }
                                final_point = {final_point.x, final_point.y - 10};
                            }
                        } else {
                            if (right_start_point(midpoint, right_start, edge_threshold)) {
                                left_edge_prev_dir = down;
                                right_edge_prev_dir = down;
                                if (left_start_point({midpoint.x, 85}, left_start, edge_threshold)) {
                                    //as the direction is down, use rightedge function
                                    //and the left edge will be stored in right_edge
                                    RightEdge(left_start, left_edge_prev_dir, false);
                                }

                                LeftLoopEdgeR(right_start, right_edge_prev_dir, leftmostP, false);

                                midpoint = {leftmostP.x - 10, leftmostP.y};
                                CheckCoor(midpoint);

                                if (right_edge_corner.size() == 1 && right_edge[right_edge_corner[0]].y > 95) {
                                    loop_state = In;
                                    midpoint = {(left_start.x + right_start.x) / 2, 85};
                                    CheckCoor(midpoint);
                                } else {
                                    right_edge_prev_dir = up;
                                    left_end_point_found = false;
                                    right_end_point_found = false;
                                    left_target_found = false;
                                    right_target_found = false;
                                    coor tempmidpoint = {leftmostP.x - 10, leftmostP.y};
                                    if (leftmostP.y > 105) {
                                        tempmidpoint.y = 105;
                                    }

                                    if (right_start_point(tempmidpoint, right_start, edge_threshold)) {
                                        RightLoopEdgeR(right_start, right_edge_prev_dir, leftmostP, false);
                                    }
                                    midpoint = {leftmostP.x - 10, leftmostP.y};
                                    CheckCoor(midpoint);

                                }
                            } else {
                                if (left_start_point({midpoint.x, 85}, left_start, edge_threshold)) {
                                    //as the direction is down, use rightedge function
                                    //and the left edge will be stored in right_edge
                                    LeftEdge(left_start, left_edge_prev_dir, false);
                                }
                            }
                        }
                    }
                }
                //follow the normal way when In
                if (loop_state == In) {
                    left_edge_prev_dir = up;
                    right_edge_prev_dir = up;
                    if (left_start_point(midpoint, left_start, edge_threshold)) {
                        LeftEdge(left_start, left_edge_prev_dir, false);
                    }
                    if (right_start_point({midpoint.x, 105}, right_start, edge_threshold)) {
                        RightEdge(right_start, right_edge_prev_dir, false);
                    }
                    if (left_edge_corner.size() == 1 && left_edge[left_edge_corner[0]].y < 85) {
                        midpoint = {(left_start.x + right_start.x) / 2, 105};
                    } else if (midpoint.y != 105) {
                        midpoint = {(left_start.x + right_start.x) / 2, 85};
                    } else {
                        midpoint = {(left_start.x + right_start.x) / 2, 105};
                    }

                    //&& left_edge[left_edge_corner[0]].y>100
                    if (left_edge_corner.size() == 1 && right_edge_corner.size() == 0 &&
                        !FindLeftEndPoint(left_edge[left_edge_corner[0]].x, left_edge[left_edge_corner[0]].y)) {
                        loop_state = Leaving;
                        midpoint = {(left_start.x + right_start.x) / 2, 85};
                    } else {
                        if (left_edge.size() && right_edge.size()) {
                            align = center_align;

                            final_point = {(left_end_point.x + right_end_point.x) / 2,
                                           (left_end_point.y + right_end_point.y) / 2};
                        } else if (left_edge.size()) {
                            align = left_align;
                            if (left_end_point_found) {
                                final_point = left_end_point;
                            } else {
                                final_point = left_edge.back();
                            }
                        } else if (right_edge.size()) {
                            align = right_align;
                            if (left_end_point_found) {
                                final_point = right_end_point;
                            } else {
                                final_point = right_edge.back();
                            }
                        }

                    }

                }
                //follow the right edges when Leaving
                if (loop_state == Leaving) {
                    left_edge_prev_dir = up;
                    right_edge_prev_dir = up;
                    if (left_start_point(midpoint, left_start, edge_threshold)) {
                        LeftEdge(left_start, left_edge_prev_dir, false);
                    }
                    std::pair<float, float> temp_target = target;
                    left_target_found = false;
                    right_target_found = false;
                    if (right_start_point({midpoint.x, 105}, right_start, edge_threshold)) {
                        RightEdge(right_start, right_edge_prev_dir, false);
                    }
                    midpoint = {(left_start.x + right_start.x) / 2, 85};
                    if (left_edge.size() > 75 && left_edge_corner.size() == 0 &&
                        std::abs(temp_target.first - target.first) < 100 &&
                        std::abs(temp_target.second - target.second) < 100) {
                            loop_state = Finished;

                        } else {
                        //set control
                        align = right_align;
                        if (right_end_point_found) {
                            final_point = right_end_point;//right_edge.back();
                        } else if (right_edge.size()) {
                            final_point = right_edge.back();
                        }
                    }
                }
                //follow the left edges when Finished
                if (loop_state == Finished) {
                    //increase the midpoint height for detecting the corner?
                    midpoint = {midpoint.x, 85};
                    left_edge_prev_dir = up;
                    right_edge_prev_dir = down;
                    std::vector<coor> temp_edge;
                    std::vector<int> temp_edge_corner;
                    if (left_start_point(midpoint, left_start, edge_threshold)) {
                        LeftEdge(left_start, left_edge_prev_dir, false);
                        temp_edge = left_edge;
                        temp_edge_corner = left_edge_corner;
                    }
                    if (right_start_point(midpoint, right_start, edge_threshold)) {
                        LeftEdge(right_start, right_edge_prev_dir, false);
                    } else {

                        left_edge_corner.clear();
                        left_edge.clear();
                    }
                    right_edge = left_edge;
                    right_edge_corner = left_edge_corner;
                    left_edge = temp_edge;
                    left_edge_corner = temp_edge_corner;
                    midpoint = {(left_start.x + right_start.x) / 2, (left_start.y + right_start.y) / 2};

                    if (right_edge_corner.size() == 1 && right_edge[right_edge_corner[0]].y > 95) {
                        track_state = Normal;
                        loop_state = Entering;
                        midpoint = {midpoint.x, 105};
                    } else {
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
                        if (left_jump) {
                            for (int i = left_edge.size() - 1; i > 0; i--) {
                                if (left_edge[i].x > rightmostP.x && left_edge[i].y > 30) {
                                    rightmostP = left_edge[i];
                                }
                            }
                            final_point = {rightmostP.x, rightmostP.y};
                            align = left_align;
                            midpoint = {rightmostP.x + 10, rightmostP.y};
                            entertype = true;
                        } else {
                            midpoint.y = 84;
                            entertype = false;
                        }

                    } else if (prev_track_state == LeftLoop) {
                        if (entertype) {

                            right_edge_prev_dir = down;
                            left_edge_prev_dir = down;
                            if (right_start_point({midpoint.x, 85}, right_start, edge_threshold)) {
                                //as the direction is down, use rightedge function
                                //and the left edge will be stored in right_edge
                                LeftEdge(right_start, right_edge_prev_dir, false);
                            }
                            if (left_start_point(midpoint, left_start, edge_threshold)) {
                                //opposite as well
                                RightLoopEdgeL(left_start, left_edge_prev_dir, rightmostP, false);
                            }
                            if (rightmostP.y != 120) {
                                midpoint = {rightmostP.x + 10, rightmostP.y};
                            }

                            if (left_edge_corner.size() == 1 && left_edge[left_edge_corner[0]].y > 95) {
                                loop_state = In;
                                midpoint = {(left_start.x + right_start.x) / 2, 85};
                            } else {
                                left_edge_prev_dir = up;
                                left_end_point_found = false;
                                right_end_point_found = false;
                                left_target_found = false;
                                right_target_found = false;
                                coor tempmidpoint = {midpoint.x, midpoint.y};
                                if (rightmostP.y > 105 && rightmostP.y != 120) {
                                    tempmidpoint.y = 105;
                                }
                                if (left_start_point(tempmidpoint, left_start, edge_threshold)) {
                                    LeftLoopEdgeL(left_start, left_edge_prev_dir, rightmostP, false);
                                }
                                align = left_align;
                                if (rightmostP.y != 120) {
                                    midpoint = {rightmostP.x + 10, rightmostP.y};
                                } else {
                                    midpoint = {midpoint.x, midpoint.y - 10};
                                }
                                if (left_end_point_found) {
                                    final_point = left_end_point;                            //right_edge.back();
                                } else if (left_edge.size()) {
                                    final_point = left_edge.back();
                                }
                                final_point = {final_point.x, final_point.y - 10};
                            }
                        } else {
                            if (left_start_point(midpoint, left_start, edge_threshold)) {
                                left_edge_prev_dir = down;
                                right_edge_prev_dir = down;
                                if (right_start_point({midpoint.x, 85}, right_start, edge_threshold)) {
                                    //as the direction is down, use rightedge function
                                    //and the left edge will be stored in right_edge
                                    LeftEdge(right_start, right_edge_prev_dir, false);
                                }
                                RightLoopEdgeL(left_start, left_edge_prev_dir, rightmostP, false);

                                midpoint = {rightmostP.x + 10, rightmostP.y};

                                if (left_edge_corner.size() == 1 && left_edge[left_edge_corner[0]].y > 95) {
                                    loop_state = In;
                                    midpoint = {(left_start.x + right_start.x) / 2, 85};
                                } else {
                                    left_edge_prev_dir = up;
                                    left_end_point_found = false;
                                    right_end_point_found = false;
                                    left_target_found = false;
                                    right_target_found = false;
                                    coor tempmidpoint = {rightmostP.x + 10, rightmostP.y};
                                    if (rightmostP.y > 105) {
                                        tempmidpoint.y = 105;
                                    }
                                    if (left_start_point(tempmidpoint, left_start, edge_threshold)) {
                                        LeftLoopEdgeL(left_start, left_edge_prev_dir, rightmostP, false);
                                    }

                                    midpoint = {rightmostP.x + 10, rightmostP.y};

                                }

                            } else {
                                if (right_start_point({midpoint.x, 85}, right_start, edge_threshold)) {
                                    //as the direction is down, use rightedge function
                                    //and the left edge will be stored in right_edge
                                    RightEdge(right_start, right_edge_prev_dir, false);
                                }
                            }
                        }
                    }
                }
                //follow the normal way when In
                if (loop_state == In) {
                    left_edge_prev_dir = up;
                    right_edge_prev_dir = up;
                    if (left_start_point({midpoint.x, 105}, left_start, edge_threshold)) {
                        LeftEdge(left_start, left_edge_prev_dir, false);
                    }
                    if (right_start_point(midpoint, right_start, edge_threshold)) {
                        RightEdge(right_start, right_edge_prev_dir, false);
                    }

                    if (right_edge_corner.size() == 1 && right_edge[right_edge_corner[0]].y < 85) {
                        midpoint = {(left_start.x + right_start.x) / 2, 105};
                    } else if (midpoint.y != 105) {
                        midpoint = {(left_start.x + right_start.x) / 2, 85};
                    } else {
                        midpoint = {(left_start.x + right_start.x) / 2, 105};
                    }

                    if (right_edge_corner.size() == 1 && left_edge_corner.size() == 0 &&
                        !FindRightEndPoint(right_edge[right_edge_corner[0]].x, right_edge[right_edge_corner[0]].y)) {
                        loop_state = Leaving;
                        midpoint = {(left_start.x + right_start.x) / 2, 85};
                    } else {
                        if (left_edge.size() && right_edge.size()) {
                            align = center_align;

                            final_point = {(left_end_point.x + right_end_point.x) / 2,
                                           (left_end_point.y + right_end_point.y) / 2};
                        } else if (left_edge.size()) {
                            align = left_align;
                            if (left_end_point_found) {
                                final_point = left_end_point;
                            } else {
                                final_point = left_edge.back();
                            }
                        } else if (right_edge.size()) {
                            align = right_align;
                            if (left_end_point_found) {
                                final_point = right_end_point;
                            } else {
                                final_point = right_edge.back();
                            }
                        }

                    }
                }
                //follow the right edges when Leaving
                if (loop_state == Leaving) {
                    left_edge_prev_dir = up;
                    right_edge_prev_dir = up;
                    if (left_start_point({midpoint.x, 105}, left_start, edge_threshold)) {
                        LeftEdge(left_start, left_edge_prev_dir, false);
                    }
                    std::pair<float, float> temp_target = target;
                    left_target_found = false;
                    right_target_found = false;
                    if (right_start_point(midpoint, right_start, edge_threshold)) {
                        RightEdge(right_start, right_edge_prev_dir, false);
                    }
                    midpoint = {(left_start.x + right_start.x) / 2, 85};
                    if (right_edge.size() > 70 && right_edge_corner.size() == 0 &&
                        std::abs(temp_target.first - target.first) < 100 &&
                        std::abs(temp_target.second - target.second) < 100) {
                            loop_state = Finished;
                        } else {
                        //set control
                        align = left_align;
                        if (left_end_point_found) {
                            final_point = left_end_point;//right_edge.back();
                        } else if (left_edge.size()) {
                            final_point = left_edge.back();
                        }
                    }
                    target = temp_target;
                }
                //follow the left edges when Finished
                if (loop_state == Finished) {
                    //increase the midpoint height for detecting the corner?
                    midpoint = {midpoint.x, 85};
                    right_edge_prev_dir = up;
                    left_edge_prev_dir = down;
                    std::vector<coor> temp_edge;
                    std::vector<int> temp_edge_corner;
                    if (right_start_point(midpoint, right_start, edge_threshold)) {
                        RightEdge(right_start, right_edge_prev_dir, false);
                        temp_edge = right_edge;
                        temp_edge_corner = right_edge_corner;
                    }
                    if (left_start_point(midpoint, left_start, edge_threshold)) {
                        RightEdge(left_start, left_edge_prev_dir, false);
                    } else {
                        right_edge_corner.clear();
                        right_edge.clear();

                    }
                    left_edge = right_edge;
                    left_edge_corner = right_edge_corner;
                    right_edge = temp_edge;
                    right_edge_corner = temp_edge_corner;
                    midpoint = {(left_start.x + right_start.x) / 2, (left_start.y + right_start.y) / 2};
                    if (left_edge_corner.size() == 1 && left_edge[left_edge_corner[0]].y > 95) {
                        track_state = Normal;
                        loop_state = Entering;
                        midpoint = {midpoint.x, 105};
                    } else {
                        //set control
                        final_point = right_end_point;
                        align = right_align;

                    }
                }
            }

            if (track_state == LeftObs) {
                if (obstacle_state == Approach) {
                    //if (prev_track_state == LeftObs) {
                    left_edge_prev_dir = up;
                    right_edge_prev_dir = up;
                    if (left_start_point(midpoint, left_start, edge_threshold)) {
                        LeftEdge(left_start, left_edge_prev_dir, false);
                    }
                    if (right_start_point(midpoint, right_start, edge_threshold)) {
                        RightEdge(right_start, right_edge_prev_dir, false);
                    }
                    //    }
                    midpoint.x = (left_start.x + right_start.x) / 2;
                    CheckCoor(midpoint);

                    if (left_obs) {
                        left_obs_target = left_obs_coor;
                        CheckCoor(left_obs_target);
                    }
                    if (left_obs_target.y > 85) {
                        obstacle_state = Pass;
                        midpoint = {(left_obs_target.x + right_start.x) / 2, 85};
                        CheckCoor(midpoint);
                    } else {
                        track_state = LeftObs;
                        obstacle_state = Approach;
                        final_point = left_obs_target;
                        align = left_align;
                    }

                }
                if (obstacle_state == Pass) {
                    left_edge_prev_dir = up;
                    right_edge_prev_dir = up;
                    if (left_start_point(midpoint, left_start, edge_threshold)) {
                        LeftEdge(left_start, left_edge_prev_dir, false);
                    }
                    if (right_start_point(midpoint, right_start, edge_threshold)) {
                        RightEdge(right_start, right_edge_prev_dir, false);
                    }

                    if (left_edge.size() < 20) {
                        midpoint = {(left_start.x + right_start.x) / 2, 105};
                    } else if (midpoint.y != 105) {
                        midpoint = {(left_start.x + right_start.x) / 2, 85};
                    } else {
                        midpoint = {(left_start.x + right_start.x) / 2, 105};
                    }

                    if (CheckLeftEdgeSmallerThan(65) && midpoint.y == 105) {
                        track_state = Normal;
                        obstacle_state = Approach;
                        midpoint = {(left_start.x + right_start.x) / 2, 105};
                    } else if (left_edge_corner.size() == 1) {
                        left_obs_target = left_edge[left_edge_corner[0]];
                    }

                }
            }

            if (track_state == RightObs) {
                if (obstacle_state == Approach) {
                    left_edge_prev_dir = up;
                    right_edge_prev_dir = up;
                    if (left_start_point(midpoint, left_start, edge_threshold)) {
                        LeftEdge(left_start, left_edge_prev_dir, false);
                    }
                    if (right_start_point(midpoint, right_start, edge_threshold)) {
                        RightEdge(right_start, right_edge_prev_dir, false);
                    }

                    midpoint.x = (left_start.x + right_start.x) / 2;
                    CheckCoor(midpoint);
                    if (right_obs) {
                        right_obs_target = right_obs_coor;
                        CheckCoor(right_obs_target);
                    }
                    if (right_obs_target.y > 85) {
                        obstacle_state = Pass;
                        midpoint = {(right_obs_target.x + left_start.x) / 2, 85};
                        CheckCoor(midpoint);
                    } else {
                        track_state = RightObs;
                        obstacle_state = Approach;
                        final_point = right_obs_target;
                        align = right_align;
                    }

                }
                if (obstacle_state == Pass) {
                    left_edge_prev_dir = up;
                    right_edge_prev_dir = up;
                    if (left_start_point(midpoint, left_start, edge_threshold)) {
                        LeftEdge(left_start, left_edge_prev_dir, false);
                    }
                    if (right_start_point(midpoint, right_start, edge_threshold)) {
                        RightEdge(right_start, right_edge_prev_dir, false);
                    }

                    if (right_edge.size() < 20) {
                        midpoint = {(left_start.x + right_start.x) / 2, 105};
                    } else if (midpoint.y != 105) {
                        midpoint = {(left_start.x + right_start.x) / 2, 85};
                    } else {
                        midpoint = {(left_start.x + right_start.x) / 2, 105};
                    }

                    if (CheckRightEdgeSmallerThan(65) && midpoint.y == 105) {
                        track_state = Normal;
                        obstacle_state = Approach;
                        midpoint = {(left_start.x + right_start.x) / 2, 105};
                    } else if (right_edge_corner.size() == 1) {
                        right_obs_target = right_edge[right_edge_corner[0]];
                    }

                }
            }

            if (track_state == StartLine) {
                if (prev_track_state != StartLine) {
                    startline_count++;
                }
                if (startline_count <= 2) {
                    if (obstacle_state == Approach) {
                        left_edge_prev_dir = up;
                        right_edge_prev_dir = up;
                        left_target_found = false;
                        right_target_found = false;
                        if (left_start_point(midpoint, left_start, edge_threshold)) {
                            LeftEdge(left_start, left_edge_prev_dir, false);
                        }
                        if (right_start_point(midpoint, right_start, edge_threshold)) {
                            RightEdge(right_start, right_edge_prev_dir, false);
                        }
                        int tempcheck = CheckStartLine(95, 4, 185, edge_threshold);
                        if (tempcheck > 15 && tempcheck < 30) {
                            obstacle_state = Pass;
                            midpoint.y = 65;
                        } else {
                            midpoint = {(left_start.x + right_start.x) / 2, (left_start.y + right_start.y) / 2};
                        }

                    }
                    if (obstacle_state == Pass) {
                        left_edge_prev_dir = up;
                        right_edge_prev_dir = up;
                        if (left_start_point(midpoint, left_start, edge_threshold)) {
                            LeftEdge(left_start, left_edge_prev_dir, false);
                        }
                        if (right_start_point(midpoint, right_start, edge_threshold)) {
                            RightEdge(right_start, right_edge_prev_dir, false);
                        }
                        if (CheckStartLine(95, 4, 185, edge_threshold) < 10 &&
                            CheckStartLine(107, 4, 185, edge_threshold) < 4) {
                            if (startline_count <= 1) {
                                track_state = Normal;
                                obstacle_state = Approach;
                                midpoint.y = 105;
                            } else if (startline_count == 2) {
                                track_state = Stop;
                                motor_stop_time = libsc::System::Time();
                                midpoint.y = 100;
                            }
                        } else {
                            midpoint = {(left_start.x + right_start.x) / 2, 65};
                            CheckCoor(midpoint);
                        }
                    }
                }
            }
            if (track_state == Tstate::Stop) {
                left_target_found = false;
                right_target_found = false;
                left_edge_prev_dir = up;
                right_edge_prev_dir = up;
                if (left_start_point(midpoint, left_start, edge_threshold)) {
                    LeftEdge(left_start, left_edge_prev_dir, false);
                }
                if (right_start_point(midpoint, right_start, edge_threshold)) {
                    RightEdge(right_start, right_edge_prev_dir, false);
                }
                midpoint = {(left_start.x + right_start.x) / 2, (left_start.y + right_start.y) / 2};

//                char buffer[50];
//                sprintf(buffer, "ave %f \n time %f", sum_enc * 0.017 / enc_count / 5, enc_count / 200.0);
//                lcd->SetRegion(libsc::Lcd::Rect(0, 0, 160, 40));
//                writerP->WriteString(buffer);
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
                    lcd->SetRegion(libsc::St7735r::Lcd::Rect(left_edge[left_edge_corner[i]].x,
                                                             left_edge[left_edge_corner[i]].y, 5, 5));
                    lcd->FillColor(lcd->kPurple);
                }
                for (int i = 0; i < right_edge_corner.size(); i++) {

                    lcd->SetRegion(libsc::St7735r::Lcd::Rect(right_edge[right_edge_corner[i]].x,
                                                             right_edge[right_edge_corner[i]].y, 5, 5));
                    lcd->FillColor(lcd->kRed);
                }

                lcd->SetRegion(libsc::St7735r::Lcd::Rect(1, 105, 188, 1));
                lcd->FillColor(lcd->kRed);

                lcd->SetRegion(libsc::St7735r::Lcd::Rect(midpoint.x, midpoint.y, 4, 4));
                lcd->FillColor(lcd->kBlue);
                char buffer[50];
                sprintf(buffer, "t %d o %d b %f \n %d %d", track_state, obstacle_state, battery_meter->GetVoltage(),
                        right_edge.front().x, left_edge.front().x);
                lcd->SetRegion(libsc::Lcd::Rect(0, 0, 160, 40));
                writerP->WriteString(buffer);
            }
            //            lcd->Clear();
            //            if (left_end_point_found) {
            //                lcd->SetRegion(libsc::St7735r::Rect(left_end_point.x, left_end_point.y, 1, 1));
            //                lcd->FillColor(lcd->kGreen);
            //            }
            //            if (right_end_point_found) {
            //                lcd->SetRegion(libsc::St7735r::Rect(right_end_point.x, right_end_point.y, 1, 1));
            //                lcd->FillColor(lcd->kRed);
            //            }
            degree_1 = degree_2;
            degree_2 = degree_3;
            coor destination{};
            if (track_state == Tstate::Normal || (track_state == Tstate::LeftLoop && loop_state == Lstate::In) ||
                (track_state == Tstate::RightLoop && loop_state == Lstate::In) ||
                (track_state == Tstate::Crossroad && crossroad_state == Cstate::Outside)) {
                if (left_edge_corner.size() && right_edge_corner.size() &&
                    !FindLeftEndPoint(left_edge[left_edge_corner.front()].x, left_edge[left_edge_corner.front()].y) &&
                    !FindRightEndPoint(right_edge[right_edge_corner.front()].x,
                                       right_edge[right_edge_corner.front()].y)) {
                    destination = final_point;
                    servo_angle = 920 + std::atan(
                            1.0 * (img2world[destination.x][destination.y][0] - img2world[99][110][0]) /
                            (img2world[destination.x][destination.y][1] - img2world[99][110][1])) * 1800 / 3.14;
                } else if (left_edge_corner.size() && !FindLeftEndPoint(left_edge[left_edge_corner.front()].x,
                                                                        left_edge[left_edge_corner.front()].y)) {
                    destination = left_edge[left_edge_corner.front()];
                    servo_angle = 920 + std::atan(
                            1.0 * (img2world[destination.x][destination.y][0] - img2world[48][110][0]) /
                            (img2world[destination.x][destination.y][1] - img2world[48][110][1])) * 1800 / 3.14;
                } else if (right_edge_corner.size() && !FindRightEndPoint(right_edge[right_edge_corner.front()].x,
                                                                          right_edge[right_edge_corner.front()].y)) {
                    destination = right_edge[right_edge_corner.front()];
                    servo_angle = 920 + std::atan(
                            1.0 * (img2world[destination.x][destination.y][0] - img2world[150][110][0]) /
                            (img2world[destination.x][destination.y][1] - img2world[150][110][1])) * 1800 / 3.14;
                } else if (left_target_found || right_target_found) {
                    servo_angle = 920 + std::atan(
                            1.0 * (target.first - img2world[99][110][0]) / (target.second - img2world[99][110][1])) *
                                        1800 / 3.14;
                } else if (left_edge.size()) {
                    destination = left_edge.back();
                    servo_angle = 920 + std::atan(
                            1.0 * (img2world[destination.x][destination.y][0] - img2world[48][110][0]) /
                            (img2world[destination.x][destination.y][1] - img2world[48][110][1])) * 1800 / 3.14;
                } else if (right_edge.size()) {
                    destination = right_edge.back();
                    servo_angle = 920 + std::atan(
                            1.0 * (img2world[destination.x][destination.y][0] - img2world[150][110][0]) /
                            (img2world[destination.x][destination.y][1] - img2world[150][110][1])) * 1800 / 3.14;
                }



            } else if (track_state == Tstate::LeftObs || track_state == Tstate::RightObs) {
                servo_angle = 920 + std::atan(
                        1.0 * (target.first - img2world[99][110][0]) / (target.second - img2world[99][110][1])) * 1800 /
                                    3.14;
            } else if (track_state == Tstate::Crossroad) {
                switch (align) {
                    case left_align:
                        servo_angle = 920 + std::atan(
                                1.0 * (img2world[final_point.x][final_point.y][0] - img2world[48][110][0]) /
                                (img2world[final_point.x][final_point.y][1] - img2world[48][110][1])) * 1800 / 3.14;
//                    servo_angle += servo_D * (servo_angle - prev_servo_angle);
                        break;
                    case center_align:
                        servo_angle = 920 + std::atan(
                                1.0 * (img2world[final_point.x][final_point.y][0] - img2world[99][110][0]) /
                                (img2world[final_point.x][final_point.y][1] - img2world[99][110][1])) * 1800 / 3.14;
//                    servo_angle += servo_D * (servo_angle - prev_servo_angle);
                        break;
                    case right_align:
                        servo_angle = 920 + std::atan(
                                1.0 * (img2world[final_point.x][final_point.y][0] - img2world[150][110][0]) /
                                (img2world[final_point.x][final_point.y][1] - img2world[150][110][1])) * 1800 / 3.14;
//                    servo_angle += servo_D * (servo_angle - prev_servo_angle);
                        break;
                }
            } else if (track_state == Tstate::RightLoop) {
                servo_angle = 920 + std::atan(
                        1.0 * (target.first - img2world[99][110][0]) / (target.second - img2world[99][110][1])) * 1800 /
                                    3.14;

            } else if (track_state == Tstate::LeftLoop) {
                servo_angle = 920 + std::atan(
                        1.0 * (target.first - img2world[99][110][0]) / (target.second - img2world[99][110][1])) * 1800 /
                                    3.14;
            } else if (track_state == Tstate::StartLine ||
                       track_state == Tstate::Stop) {//|| track_state == Tstate::Stop
                if (left_target_found || right_target_found) {
                    servo_angle = 920 + std::atan(
                            1.0 * (target.first - img2world[99][110][0]) / (target.second - img2world[99][110][1])) *
                                        1800 / 3.14;
                } else if (left_edge.size()) {
                    destination = left_edge.back();
                    servo_angle = 920 + std::atan(
                            1.0 * (img2world[destination.x][destination.y][0] - img2world[48][110][0]) /
                            (img2world[destination.x][destination.y][1] - img2world[48][110][1])) * 1800 / 3.14;
                } else if (right_edge.size()) {
                    destination = right_edge.back();
                    servo_angle = 920 + std::atan(
                            1.0 * (img2world[destination.x][destination.y][0] - img2world[150][110][0]) /
                            (img2world[destination.x][destination.y][1] - img2world[150][110][1])) * 1800 / 3.14;
                } else {
                    servo_angle = 920;
                }
            }
            degree_3 = servo_angle - prev_angle;
            prev_angle = servo_angle;
            servo_angle += servo_D * ((degree_1 + degree_2 + degree_3) / 3.0);
            servo->SetDegree(libutil::Clamp(500, servo_angle, 1340));

            if (debug) {
                lcd->SetRegion(libsc::St7735r::Lcd::Rect(destination.x - 2, destination.y - 2, 5, 5));
                lcd->FillColor(lcd->kGreen);
            }


            if (track_state == RightObs) {
                midpoint_offset = 100;
            } else if (track_state == LeftObs) {
                midpoint_offset = 100;
            } else {
                midpoint_offset = 200;
            }

            buzzer->SetBeep(track_state == RightObs || track_state == LeftObs);
            char buffer[100] = {};
            sprintf(buffer, "%d , %d, %d\n", (int) track_state, (int) left_edge_corner.size(),
                    (int) right_edge_corner.size());
//            sprintf(buffer, "%.1f+%.2f+%.2f\n", 1.0, (float) target_speed, (float) servo_angle);
//            const Byte speedByte = 85;
//            while (!bt->SendBuffer(&speedByte, 1))
//                ;
            if (track_state != prev_track_state) {
                bt->SendStr(buffer);
            }
            if (!debug) {
//                char buffer[50];
//                sprintf(buffer, "speed %d", (int)target_speed);
//                lcd->SetRegion(libsc::Lcd::Rect(0, 100, 160, 40));
//                writerP->WriteString(buffer);
            }
            prev_track_state = track_state;
        }

    }
}
