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
#ifndef OPENPSTD_RECEIVER_H
#define OPENPSTD_RECEIVER_H
#include "Geometry.h"
namespace Kernel {
    class Receiver {
    public:
        const double x;
        const double y;
        const double z;

        /**
         * Initializes a receiver on coordinates (x,y,z) in world space (not fixed to grid)
         * @param x coordinate in world space
         * @param y coordinate in world space
         * @param z coordinate in world space
         */
        Receiver(const double x, const double y, const double z);

        /**
         * Calculates the pressure at the receiver. Method depends on config:nearest_neighbour:
         * if it is True, this method will return the pressure of the closest gridpoint
         * If nearest_neighbour is False, interpolation will be used (slower, but more accurate).
         * @see spatderp3
         */
        void calc_local_pressure();
    };

}
#endif //OPENPSTD_RECEIVER_H
