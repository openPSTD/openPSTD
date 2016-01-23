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
// Date: 29-9-15
//
//
// Authors: Omar Richardson
//
//
// Purpose: Discretize wave numbers dynamically,
// and optimize them for FFT computations.
//
//
//////////////////////////////////////////////////////////////////////////
#ifndef OPENPSTD_WAVE_NUMBERS_H
#define OPENPSTD_WAVE_NUMBERS_H

#include <map>
#include <math.h>
#include <complex>
#include <memory>
#include <iostream>
#include <Eigen/Dense>

namespace Kernel {
    class WaveNumberDiscretizer {
    public:
        struct Discretization {
            Eigen::ArrayXf wave_numbers;
            Eigen::ArrayXcf complex_factors;
            Eigen::ArrayXcf pressure_deriv_factors;
            Eigen::ArrayXcf velocity_deriv_factors;
        };

        /*
         * Obtain the discretization for the given grid size and number of grid points.
         * If discretization is unknown, it is computed and stored for future reference.
         * @param dx: grid size
         * @param N: number of grid points
         * @return: Struct with wave discretization values.
         */
        Discretization get_discretization(float dx, int N); //Todo: should we include dx here?

        /**
         * Initializer for wave number discretizator. Initialize only a single instance to optimize computations.
         */
        WaveNumberDiscretizer();

        std::map<int, Discretization> computed_discretization; // Should be private! public for debugging purposes

    private:

        /**
         * Compute discretization for the given grid size and number of grid points.
         * @param dx: grid size
         * @param N: number of grid points
         * @return: Struct with wave discretization values.
         */
        Discretization discretize_wave_numbers(float dx, int N); //Todo: Needs a better name

        /**
         * Internal storage of wave number discretizations.
         */

        /**
         * Compute the rounded up 2log of the number of grid cells.
         */
        int match_number(int n);
    };

    std::ostream &operator<<(std::ostream &str, WaveNumberDiscretizer const &v);
}

#endif //OPENPSTD_WAVE_NUMBERS_H
