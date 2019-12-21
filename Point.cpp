//
// Created by ehardy on 12/4/19.
//

#include "Point.hpp"
#include <cmath>

Point::Point(int _x, int _y): x(_x), y(_y) {};

Point::~Point(){};

double Point::calculDistance(Point p){
    return sqrt((this->x-p.x)*(this->x-p.x)+(this->y-p.y)*(this->y-p.y));
}

void Point::setLocation(int _x, int _y){
    this->x = _x;
    this->y = _y;
}