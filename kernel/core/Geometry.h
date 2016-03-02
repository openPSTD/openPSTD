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
// Date: 17-9-15
//
//
// Authors: omar
//
//
// Purpose:
//
//
//////////////////////////////////////////////////////////////////////////
#ifndef OPENPSTD_GEOMETRY_H
#define OPENPSTD_GEOMETRY_H

#include <ostream>
#include <vector>

namespace OpenPSTD {
    namespace Kernel {
        /**
         * The points of the grid, represented by 2D integer vectors.
         * Origin (0,0) is top left.
         */
        class Point {
        public:
            int x, y, z;
            std::vector<int> array;

            Point() : x(0), y(0), z(0) { };

            /**
             * Constructor for a point in grid coordinates.
             * Points only hold integer grid coordinates.
             */
            Point(int x, int y, int z = 0);

            /**
             * Addition operator for points
             */
            friend Point operator+(Point a, Point b);

            /**
             * Subtraction operator for points.
             * Points with negative entries are well defined.
             */
            friend Point operator-(Point a, Point b);
        };

        /** String representation for points
         *
         */
        std::ostream &operator<<(std::ostream &str, Point const &v);
    }
}
#endif //OPENPSTD_GEOMETRY_H
