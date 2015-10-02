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
#include "PSTDFile.h"
#include "Geometry.h"
#include <string>
#include "kernel_functions.h"

namespace Kernel {
    class Receiver {
        /*
        * Receiver class. This class measures and stores the sound pressure and velocity on each time step.
        * Note that receiver locations (just like speaker locations) are defined on the grid,
        * but don't need to lie on grid points; their coordinates are not rounded off.
         * If the Receiver is not located on a grid point, the sound values are interpolated,
         * either from the nearest grid point or using a spectral interpolation method.
        */
    public:
        const double x;
        const double y;
        const double z;
        std::string id;
        enum InterpolationType {
            NearestNeighbour, SpectralInterpolation
        };
        std::vector<double> location;
        std::shared_ptr<PSTDFileSettings> config;
        std::shared_ptr<Point> grid_location; //Todo (0mar): Should this be rounded down or rather rounded off?
        std::vector<double> grid_offset;
        std::shared_ptr<Domain> container_domain;
        std::vector<double> received_values;

        /**
         * Initializes a receiver on coordinates (x,y,z) in grid space (not fixed to integers)
         * @param location double coordinates in 3D grid space. For 2D, leave z=0
         * @param config: Pointer to configuration object
         * @param id: Unique receiver identifier
         * @param container: The domain in which the receiver is located. This should not be a PML-domain.
         */
        Receiver(std::vector<double> location, std::shared_ptr<PSTDFileSettings> config, std::string id,
                 std::shared_ptr<Domain> container);

        /**
         * Calculates the sound pressure at the receiver at the current time step.
         * Depending on config, this method uses the nearest neighbour value (fast, inaccurate)
         * or spectral interpolation (slower, more accurate)
         * @see spatderp3
         * @see config
         */
        double compute_local_pressure();

    private:
        /**
         * Computes the fft_factor //todo: whut exactly?
         */
        double compute_fft_factor(Point size, BoundaryType bt);

        /**
         * Computes the pressure from the nearest neighbour
         * @return nearest neighbour pressure approximation
         */
        double compute_with_nn();

        /**
         * Computes the pressure using spectral interpolation
         * @return spectral interpolated pressure approximation
         */
        double compute_with_si();

    };

}
#endif //OPENPSTD_RECEIVER_H
