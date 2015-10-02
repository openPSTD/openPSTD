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

//
// Created by omar on 3-9-15.
//

#ifndef OPENPSTD_KERNEL_FUNCTIONS_H
#define OPENPSTD_KERNEL_FUNCTIONS_H

#include <eigen/Eigen/Dense>
#include <iostream>
#include <vector>
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
    enum BoundaryType {
        HORIZONTAL, VERTICAL
    };

    /**
     * Helper enums - used to distinguish pressure computations from velocity computations
     */
    enum CalculationType {
        PRESSURE, VELOCITY
    };

    struct rMatrices2D {
        Eigen::Matrix<double, 4, 4> pressure;
        Eigen::Matrix<double, 4, 4> velocity;
    };

    struct rMatrices1D {
        Eigen::Matrix<double, 4, 2> pressure;
        Eigen::Matrix<double, 4, 2> velocity;
    };

    /**
     * Computes and return reflection and transmission matrices for pressure and velocity
     * based on density of a domain and 2 opposite neighbours(?)
     * @param rho1 density of first neighbour
     * @param rho2 density of domain
     * @param rho density of opposite neighbour
     * return struct containing pressure and velocity matrix (4x2)
     */
    rMatrices1D getRMatrices1D(const double rho1, const double rho2, const double rho);

    /**
     * Computes the largest grid spacing possible based
     * on the speed of the medium and the maximum frequency
     * Throws an exception if no compatible grid size can be found
     * @param cnf config object containing the properties of the geometry
     * @return double corresponding to the grid size
    */
    double getGridSpacing(PSTDFileSettings cnf);

    /**
     * Computes the smallest power of 2 larger or equal to n
     * @param n
     * return 2^k >= n
     */
    int next2Power(double n);

    /**
     * Computes the attenuation coefficients of PML cells (pressure and velocity) as per the formula:
     * coef = alpha(point_distance/PML_thickness)^4 (Hornikx et al. 2010)
     * @param cnf config object containing the properties of the geometry
     * @return a tuple of vectors, the first being the coefficients for pressure,
     * the second for velocity.
     */
    std::tuple<std::vector<double>, std::vector<double>> PML(PSTDFileSettings cnf); //TODO Louis change cnf argument

}
#endif //OPENPSTD_KERNEL_FUNCTIONS_H
