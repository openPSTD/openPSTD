//////////////////////////////////////////////////////////////////////////
// This file is part of openPSTD.                                       //
//                                                                      //
// openPSTD is free software: you can redistribute it and/or modify     //
// it under the terms of the GNU General Public License as published by //
// the Free Software Foundation, either version 3 of the License, or    //
// (at your option) any later version.                                  //
//                                                                      //
// openPSTD is distributed in the hope that it will be useful,          //
// but WITHOUT ANY WARRANTY; without even the implied warranty of       //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        //
// GNU General Public License for more details.                         //
//                                                                      //
// You should have received a copy of the GNU General Public License    //
// along with openPSTD.  If not, see <http://www.gnu.org/licenses/>.    //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
// Date:
//      10-9-15
//
// Author:
//      omar
//
//////////////////////////////////////////////////////////////////////////

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