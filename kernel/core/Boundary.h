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
// Authors: 0mar Richardson
//
//
// Purpose: Modelling boundaries between domains.
//
//
//////////////////////////////////////////////////////////////////////////
#ifndef OPENPSTD_BOUNDARY_H
#define OPENPSTD_BOUNDARY_H

#include <string>

#include "Domain.h"

namespace OpenPSTD
{
    namespace Kernel
    {
        /**
         * Models a boundary between two domains.
         *
         * Boundaries are represented as line segments parallel to one of the two (three) axes in the coordinate system.
         * Each boundary is adjacent to at most two domains.
         */
        class Boundary
        {
        public:
            /** First of the domains separated by the boundary **/
            std::shared_ptr<Domain> domain1;

            /** Second of the domains separated by the boundary **/
            std::shared_ptr<Domain> domain2;

            /** Whether the boundary is used for horizontal or vertical computations **/
            CalcDirection type;

            /**
             * Creates a boundary of given type between the given domains, together with its orientation.
             * For boundaries between an upper and a lower domain, the calculation direction is vertical.
             * For boundaries between a left and a right domain, the calculation direction is horizontal.
             * @param domain1: Pointer to first domain
             * @param domain2: Pointer to second domain
             * @param Enum containing the direction of calculation (horizontal or vertical)
             * @return: new Boundary object
             */
            Boundary(std::shared_ptr<Domain> domain1, std::shared_ptr<Domain> domain2, CalcDirection type);

        };
    }
}

#endif //OPENPSTD_BOUNDARY_H
