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
// Authors: Omar Richardson, Louis van Harten
//
//
//////////////////////////////////////////////////////////////////////////
#ifndef OPENPSTD_WISDOMCACHE_H
#define OPENPSTD_WISDOMCACHE_H

#include <map>
#include <math.h>
#include <fftw3.h>
#include <complex>
#include <memory>
#include <iostream>
#include <Eigen/Dense>

namespace OpenPSTD {
    namespace Kernel {

        /**
         * Storage of the accumulated wisdom in the simulation.
         *
         * Purpose: Discretize wave numbers dynamically,
         * and optimize them for FFT computations.
         * Also contains the plans for FFTW.
         */
        class WisdomCache {
        public:

            /**
             * Storage of the wave number discretizations
             */
            struct Discretization {
                Eigen::ArrayXf wave_numbers;
                Eigen::ArrayXcf complex_factors;
                Eigen::ArrayXcf pressure_deriv_factors;
                Eigen::ArrayXcf velocity_deriv_factors;
            };

            /**
             * Storage of the plans used in the Fast Fourier Transform
             */
            struct Planset_FFTW {
                fftwf_plan plan;
                fftwf_plan plan_inv;
            };

            /**
             * Obtain the discretization for the given grid size and number of grid points.
             * If discretization is unknown, it is computed and stored for future reference.
             * @param dx: grid size
             * @param N: number of grid points
             * @return: Struct with wave discretization values.
             */
            Discretization get_discretization(float dx, int N); //Todo: should we include dx here?

            /**
             * Obtain an FFTW plan for the given fft length and batch size.
             * If the plan does not exist yet, it is created and cached.
             * @param fft_length: Length of the planned FFT
             * @param fft_batch_size: Batch size of the planned FFT
             */
            Planset_FFTW get_fftw_planset(int fft_length, int fft_batch_size);

            /**
             * Initializer for the cache. Initialize only a single instance to optimize computations.
             */
            WisdomCache();

            std::map<int, Discretization> computed_discretization; // Should be private! public for debugging purposes
            std::map<std::string, Planset_FFTW> cached_fftw_plans; // Should be private! public for debugging purposes

        private:

            /**
             * Compute discretization for the given grid size and number of grid points.
             * @param dx: grid size
             * @param N: number of grid points
             * @return: Struct with wave discretization values.
             */
            Discretization discretize_wave_numbers(float dx, int N); //Todo: Needs a better name

            /**
             * Create new planset for given fftw length, batch size. Not thread safe!
             * @param: fft_length: Length of the planned FFT
             * @param fft_batch_size: Batch size of the planned FFT
             */
            Planset_FFTW create_fftw_planset(int fft_length, int fft_batch_size);

            /**
             * Internal storage of wave number discretizations.
             */

            /**
             * Compute the rounded up @f$\log_2@f$ of the number of grid cells.
             */
            int match_number(int n);
        };

        std::ostream &operator<<(std::ostream &str, WisdomCache const &v);
    }
}

#endif //OPENPSTD_WISDOMCACHE_H
