////////////////////////////////////////////////////////////////////////
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
// Purpose: This class models a receiver; a way to measure the sound
// pressure and velocity on a fixed location within the domain.
//
//
//////////////////////////////////////////////////////////////////////////
#ifndef OPENPSTD_RECEIVER_H
#define OPENPSTD_RECEIVER_H

#include <vector>
#include <iostream>
#include <memory>
#include "../KernelInterface.h"
#include "Geometry.h"
#include "Domain.h"
#include <string>
#include "kernel_functions.h"
#include "WisdomCache.h"

namespace OpenPSTD {
    namespace Kernel {
        /**
         * Measure pressure values on a fixed location in the scene.
         *
         * This class measures and stores the sound pressure on each time step.
         * Note that receiver locations (just like speaker locations) are defined on the grid,
         * but don't need to lie on grid points; their coordinates are not rounded off.
         * If the Receiver is not located on a grid point, the sound values are interpolated,
         * either from the nearest grid point or using a spectral interpolation method.
         */
        class Receiver {

        public:
            const float x;
            const float y;
            const float z;
            int id;

            /**
             * Unrounded (grid) location
             */
            std::vector<float> location;

            /**
             * Integer grid location. <=> Cell containing the receiver.
             */
            Point grid_location;

            /**
             * Distance from the receiver to the top left of the cell
             */
            std::vector<float> grid_offset;

            /**
             * Config file containing the receiver parameters
             */
            std::shared_ptr<PSTDSettings> config;

            /**
             * Domain containing the receiver
             */
            std::shared_ptr<Domain> container_domain;

            /**
             * Vector of observed pressure values in the receiver
             */

            std::vector<float> received_values;

            /**
             * Initializes a receiver on coordinates (x,y,z) in grid space (not fixed to integers)
             * @param location float coordinates in 3D grid space. For 2D, leave z=0
             * @param config: Pointer to configuration object
             * @param id: Unique receiver identifier
             * @param container: The domain in which the receiver is located. This should not be a PML-domain.
             */
            Receiver(std::vector<float> location, std::shared_ptr<PSTDSettings> config, int id,
                     std::shared_ptr<Domain> container);

            /**
             * Calculates the sound pressure at the receiver at the current time step.
             * Depending on config, this method uses the nearest neighbour value (fast, inaccurate)
             * or spectral interpolation (slower, more accurate)
             * @see spatderp3
             * @see config
             * @return float approximation of the sound pressure in receiver location
             */
            float compute_local_pressure();

        private:
            /**
             * Computes the fft_factors along the provided boundary
             */
            Eigen::ArrayXcf get_fft_factors(Point size, CalcDirection bt);

            /**
             * Computes the pressure from the nearest neighbour
             * @return nearest neighbour pressure approximation
             */
            float compute_with_nn();

            /**
             * Computes the pressure using spectral interpolation
             * @return spectral interpolated pressure approximation
             */
            float compute_with_si();

            /**
             * Compute the pressure for the receiver.
             */
            Eigen::ArrayXXf calc_domain_fields(std::shared_ptr<Domain> container, CalcDirection bt);

        };

    }
}

#endif //OPENPSTD_RECEIVER_H
