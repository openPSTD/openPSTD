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
#include <boost/lexical_cast.hpp>

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

        std::string Point::ToString() const
        {
            return "Point(" + boost::lexical_cast<std::string>(x) + "," + boost::lexical_cast<std::string>(y) + ")";
        }

        std::ostream &operator<<(std::ostream &str, Point const &v)
        {
            str << v.ToString();
            return str;
        }
    }
}