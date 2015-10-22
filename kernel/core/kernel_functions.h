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

#include <eigen/Eigen/Dense>
#include <iostream>
#include <vector>
#include <map>
#include <math.h>
#include "PSTDFile.h"

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
     * Helper enums - used to distinguish horizontal boundaries from vertical boundaries
     */
    enum class BoundaryType {
        HORIZONTAL = 0, VERTICAL = 1
    };

    /**
     * Helper enums - used to distinguish pressure computations from velocity computations
     */
    enum class CalculationType {
        PRESSURE, VELOCITY
    };

    enum class Direction {
        LEFT, RIGHT, TOP, BOTTOM
    };

    /**
     * Return the opposite direction of the provided direction
     * @param direction: Direction enum
     * @return: opposite direction
     */
    Direction get_opposite(Direction direction);

    struct RhoArray {
        Eigen::Array<float, 4, 2> pressure;
        Eigen::Array<float, 4, 2> velocity;
    };

    /**
     * Function computing the spatial derivatives of the domains.
     *
     * The domain for which the derivative is being computed is p2. p1 and p3 are its neighbours.     *
     *
     * @param p2 variable matrix subdomain 2
     * @param derfact factor to compute derivative in wavenumber domain
     * @param Wlength length of window function
     * @param N1 batch (domain size in non-dominant direction)
     * @param N2 dimension of the ffts ( next_2pow(len(p2)+2*Wlenght) )
     * @param Rmatrix matrix of reflection coefficients
     * @param p1 variable matrix subdomain 1
     * @param p3 variable matrix subdomain 3
     * @param var_index variable index: 0 for pressure, 1,2,3, for respectively x, z and y (in 3rd dimension) velocity
     * @param direction direction for computation of derivative: 0,1 for z, x direction respectively
     * @return a 2d array containing the derivative of p2
     */
    Eigen::ArrayXXf spatderp3(std::shared_ptr<Eigen::ArrayXXf> p2, std::shared_ptr<Eigen::ArrayXcf> derfact,
                              int Wlength, int N1, int N2, Eigen::Matrix<float, 1, 4> Rmatrix,
                              std::shared_ptr<Eigen::ArrayXXf> p1,
                              std::shared_ptr<Eigen::ArrayXXf> p3, int var_index, int direction);

    /**
     * Computes and return reflection and transmission matrices for pressure and velocity
     * based on density of a domain and 2 opposite neighbours(?)
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
    float getGridSpacing(PSTDFileSettings cnf);

    /**
     * Computes the smallest power of 2 larger or equal to n
     * @param n
     * return 2^k >= n
     */
    int next2Power(float n);

    /**
     * Computes the attenuation coefficients of PML cells (pressure and velocity) as per the formula:
     * coef = alpha(point_distance/PML_thickness)^4 (Hornikx et al. 2010)
     * @param cnf config object containing the properties of the geometry
     * @return a tuple of vectors, the first being the coefficients for pressure,
     * the second for velocity.
     */
    //std::tuple<std::vector<float>, std::vector<float>> PML(PSTDFileSettings cnf); //TODO Probably unused

}
#endif //OPENPSTD_KERNEL_FUNCTIONS_H
