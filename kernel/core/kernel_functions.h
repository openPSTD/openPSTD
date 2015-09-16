//
// Created by omar on 3-9-15.
//

#ifndef OPENPSTD_KERNEL_FUNCTIONS_H
#define OPENPSTD_KERNEL_FUNCTIONS_H

#include <eigen/Eigen/Dense>
#include <iostream>
#include <vector>
#include <math.h>

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

struct rMatrices2D {
    Eigen::Matrix<double, 4, 4> pressure;
    Eigen::Matrix<double, 4, 4> velocity;
};

struct rMatrices1D {
    Eigen::Matrix<double, 4, 2> pressure;
    Eigen::Matrix<double, 4, 2> velocity;
};

struct Config {
    double c1;
    double freqMax;
    int PML_attenuation; //Attenuation of PML cells, "ampmax" in original python code
    int PML_n_cells; //Number of PML cells
    double medium_density; //"rho" in original python code
}; //Todo (0mar): Create a configuration data structure

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
 * Computes and return reflection and transmission matrices for pressure and velocity
 * based on density of a domain and 4 surrounding neighbours(?)
 * @param rhoLeft density of first neighbour
 * @param rhoRight density of first neighbour
 * @param rhoLower density of first neighbour
 * @param rhoUpper density of domain
 * @param rho density of opposite neighbour
 * return  struct containing pressure and velocity matrix (4x4)
 */
rMatrices2D getRMatrices2D(const double rhoLeft, const double rhoRight, const double rhoUpper,
                                const double rhoLower,const double rho);

/**
 * Computes the largest grid spacing possible based
 * on the speed of the medium and the maximum frequency
 * Throws an exception if no compatible grid size can be found
 * @param cnf config object containing the properties of the geometry
 * @return double corresponding to the grid size
 */
double getGridSpacing(const Config cnf);

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
std::tuple<std::vector<double>, std::vector<double>> PML(const Config cnf); //TODO Louis change cnf argument

#endif //OPENPSTD_KERNEL_FUNCTIONS_H
