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
// Authors: Omar Richardson
//
//
// Purpose: This class models a speaker: the source of sound propagation
// in the domain.
//
//
//////////////////////////////////////////////////////////////////////////
#ifndef OPENPSTD_SPEAKER_H
#define OPENPSTD_SPEAKER_H

#include "Domain.h"
#include <string>
#include <memory>
#include <vector>
#include <Eigen/Dense>
#include <math.h>

namespace OpenPSTD
{
    namespace Kernel
    {
        /**
         * Initial sound pressure. This class is a 'wrapper' around a gaussian kernel contribution.
         * Note that speaker locations (just like receiver locations) are defined on the grid,
         * but don't need to lie on grid points; their coordinates are not rounded off.
         */
        class Speaker
        {

        public:
            const float x;
            const float y;
            const float z;
            std::vector<float> location;
            Point grid_point = Point((int) x, (int) y, (int) z);
            std::vector<float> grid_offset;

            /**
             * Speaker initialization with coordinates from scene.
             * @param location: vector of world coordinates
             */
            Speaker(std::vector<float> location);

            /**
             * Adds the initial sound pressure to the domain values.
             * The initial pressure shape is a Gaussian, defined as
             * @f$p_0(x,y) = e^{-\beta((x-x_s)^2+(y-y_s)^2)}@f$
             * with bandwidth @f$\beta = -3e^{-6}c^2/dx^2@f$
             * and speaker location @f$(x_s,y_s)@f$.
             * @param domain: domain to compute sound pressure contribution for
             */
            void addDomainContribution(std::shared_ptr<Domain> domain);

        };
    }
}

#endif //OPENPSTD_SPEAKER_H
