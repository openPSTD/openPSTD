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
//      3-9-2015
//
// Authors:
//      Omar Richardson
//      Louis van Harten
//
// Purpose:
//      Contains procedural functions that don't fit with any one class.
//
//////////////////////////////////////////////////////////////////////////


#ifndef OPENPSTD_KERNEL_FUNCTIONS_H
#define OPENPSTD_KERNEL_FUNCTIONS_H

#include <Eigen/Dense>
#include <complex>
#include <fftw3.h>
#include <iostream>
#include <vector>
#include <map>
#include <math.h>
#include <algorithm>
#include "../KernelInterface.h"
#include "Geometry.h"

namespace OpenPSTD {
    namespace Kernel {

        /**
         * Helper function equivalent to numpy.arange()
         */
        template<typename T>
        std::vector<T> arange(T start, T stop, T step = 1) {
            std::vector<T> list;
            for (T i = start; i < stop; i += step)
                list.push_back(i);
            return list;
        }

        /**
         * Small positive number to facilitate division and other numerical computations
         */
        const float EPSILON = 1E-5; // Depends on float/double


        /**
         * Helper enums - used to distinguish horizontal boundaries from vertical boundaries
         */
        enum class CalcDirection {
            X = 0, Y = 1
        };

        /**
         * Helper enums - used to distinguish pressure computations from velocity computations
         */
        enum class CalculationType {
            PRESSURE, VELOCITY, PRESSURE_nonstag, VELOCITY_nonstag, VELOCITY_dvydx, VELOCITY_stagdown, VELOCITY_stagright
        };

        /**
         * Enum representing directions among domains.
         */
        enum class Direction {
            LEFT, RIGHT, TOP, BOTTOM
        };

        /**
         * Convenience vectors
         */
        const std::vector<Direction> all_directions = {Direction::LEFT, Direction::RIGHT, Direction::TOP,
                                                       Direction::BOTTOM};

        const std::vector<CalcDirection> all_calc_directions = {CalcDirection::X, CalcDirection::Y};
       /* const std::vector<CalculationType> all_calculation_types = {CalculationType::PRESSURE,
                                                                    CalculationType::VELOCITY,
                                                                    CalculationType::PRESSURE_nonstag,
                                                                    CalculationType::VELOCITY_nonstag,
                                                                    CalculationType::VELOCITY_dvydx,
                                                                    CalculationType::VELOCITY_stagdown,
                                                                    CalculationType::VELOCITY_stagright
        };*/
        const std::vector<CalculationType> all_calculation_types = {CalculationType::PRESSURE,
                                                                    CalculationType::VELOCITY
        };
        /**
         * Coefficients for a six stage RK time integration
         */
        const std::vector<float> rk_coefficients = {1.179799016570605e-1f, 1.846469664911166e-1f, 2.466236043095944e-1f,
                                                    3.318395427360000e-1f, 5e-1, 1.f}; // Temporary until bugfix


        /**
         * Return the opposite direction of the provided direction
         * @param direction: Direction enum
         * @return: opposite direction
         */
        Direction get_opposite(Direction direction);

        /**
         * Get the direction orthogonal to the provided calculation direction.
         * @param direction: Calculation direction enum
         * @return: Axis orthogonal to calculation direction
         */
        CalcDirection get_orthogonal(CalcDirection direction);

        /**
         * Obtain corresponding Calculation Direction Axis from direction
         * @param direction: Direction Enum
         * @return Calculation direction with axis corresponding to direction
         */
        CalcDirection direction_to_calc_direction(Direction direction);

        /**
         * Struct with arrays containing the reflection and transmission
         * coefficients of the pressure and the velocity
         */
        struct RhoArray {
            Eigen::ArrayXXf pressure;
            Eigen::ArrayXXf pressure_nonstag;
            Eigen::ArrayXXf velocity;
            Eigen::ArrayXXf velocity_nonstag;
            Eigen::ArrayXXf velocity_dvydx;
            Eigen::ArrayXXf velocity_stagdown;
            Eigen::ArrayXXf velocity_stagright;
        };

        /**
         * Function computing the spatial derivatives of the domains.
         *
         * The domain for which the derivative is being computed is p2. p1 and p3 are its neighbours.     *
         *
         * //TODO (remove the warning on the next line when everything is ported)
         * WARNING - order in Python code:  (p2,derfact,Wlength,A,Ns2,N1,N2,Rmatrix,p1,p3,var,direct)
         *           order in the C++ code: (p1,p2,p3,derfact,Rmatrix,window,wlen,var_index,direct)
         *           var_index=var and N1, N2 and Ns2 are derived from other inputs
         *
         * @param p1 variable matrix subdomain 1
         * @param p2 variable matrix subdomain 2
         * @param p3 variable matrix subdomain 3
         * @param fft_length
         * @param derfact factor to compute derivative in wavenumber domain
         * @param Rmatrix matrix of reflection coefficients
         * @param var_index variable index: 0 for pressure, 1,2,3, for respectively x, z and y (in 3rd dimension) velocity
         * @param direct direction for computation of derivative
         * @return a 2d array containing the derivative of p2
         */
        Eigen::ArrayXXf spatderp3(Eigen::ArrayXXf p1, Eigen::ArrayXXf p2,
                                  Eigen::ArrayXXf p3, Eigen::ArrayXcf derfact,
                                  RhoArray rho_array, Eigen::ArrayXf window, int wlen,
                                  CalculationType ct, CalcDirection direct);

        /**
         * Version of spatderp3 that takes cached plans as input.
         * @see spatderp3(9)
         */
        Eigen::ArrayXXf spatderp3(Eigen::ArrayXXf p1, Eigen::ArrayXXf p2,
                                  Eigen::ArrayXXf p3, Eigen::ArrayXcf derfact,
                                  RhoArray rho_array, Eigen::ArrayXf window, int wlen,
                                  CalculationType ct, CalcDirection direct,
                                  fftwf_plan plan, fftwf_plan plan_inv);

        /**
         * Computes and return reflection and transmission matrices for pressure and velocity
         * based on density of a domain and 2 opposite neighbours in any direction
         * @param rho1 density of first neighbour
         * @param rho2 density of domain
         * @param rho density of opposite neighbour
         * return struct containing pressure and velocity matrix (4x2)
         */
        RhoArray get_rho_array(const float rho1, const float rho_self, const float rho2);

        /**
         * Computes the largest grid spacing possible based
         * on the speed of the medium and the maximum frequency
         * Throws an exception if no compatible grid size can be found
         * @param cnf config object containing the properties of the geometry
         * @return float corresponding to the grid size
        */
        float get_grid_spacing(PSTDSettings cnf);

        /**
         * Gives a two-sided array of window coefficients for a given window size and patch error
         * @param window_size length of the window
         * @param patch_error given patch error
         * @return Eigen::ArrayXf Eigen array of length window_size*2 containing window coefficients
         */
        Eigen::ArrayXf get_window_coefficients(int window_size, int patch_error);

        /**
         * Computes the smallest power of 2 larger or equal to n if n positive, and 1 otherwise
         * @param n
         * @return 2^k >= n
         */
        int next_2_power(float n);

        /**
         * Perform a numerical check whether a approximately equals b.
         * Returns
         * @see EPSILON
         * @param a: float
         * @param b: float
         * @return True if relative or absolute difference is smaller than the specified EPSILON, False otherwise
         */
        bool is_approx(float a, float b);

        void debug(std::string msg);

        void write_array_to_file(Eigen::ArrayXXf array, std::string filename, unsigned long kk, int id);

    }
}
#endif //OPENPSTD_KERNEL_FUNCTIONS_H
