//
// Created by omar on 10-9-15.
//

#include "Geometry.h"

namespace OpenPSTD
{
    namespace Kernel
    {
        Point::Point(int x, int y, int z) : x(x), y(y), z(z)
        {
            this->array = {x, y, z};
        }

        Point operator+(Point a, Point b)
        {
            return Point(a.x + b.x, a.y + b.y, a.z + b.z);
        }

        Point operator-(Point a, Point b)
        {
            return Point(a.x - b.x, a.y - b.y, a.z - b.z);
        }

        std::ostream &operator<<(std::ostream &str, Point const &v)
        {
            str << "Point(" << v.x << "," << v.y << ")";
            return str;
        }
    }
}