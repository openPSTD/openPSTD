//
// Created by omar on 10-9-15.
//

#include "Geometry.h"
namespace Kernel {
    Point::Point(int x, int y, int z = 0) :x(x),y(y),z(z){
        this->array = {x, y, z};
    }

    Point operator+(Point a, Point b) {
        return Point(a.x + b.x, a.y + b.y, a.z + b.z);
    }

    Point operator-(Point a, Point b) {
        return Point(a.x - b.x, a.y - b.y, a.z - b.z);
    }
}