//
// Created by omar on 3-9-15.
//

#ifndef OPENPSTD_KERNEL_FUNCTIONS_H
#define OPENPSTD_KERNEL_FUNCTIONS_H

#include <eigen/Eigen/Dense>
#include <iostream>
#include <math.h>


struct rMatrices2D {
    Matrix<double, 4, 4> pressure;
    Matrix<double, 4, 4> velocity;
};

struct rMatrices1D {
    Matrix<double, 4, 2> pressure;
    Matrix<double, 4, 2> velocity;
};

struct rMatrices2D;
struct Config {
    double c1;
    double freqMax;
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
#endif //OPENPSTD_KERNEL_FUNCTIONS_H
