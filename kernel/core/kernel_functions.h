//
// Created by omar on 3-9-15.
//

#ifndef OPENPSTD_KERNEL_FUNCTIONS_H
#define OPENPSTD_KERNEL_FUNCTIONS_H

#include <eigen/Eigen/Dense>
#include <iostream>
#include <math.h>

using namespace Eigen;
using namespace std;

struct rMatrices1D;
struct rMatrices2D;
struct Config {
    double c1;
    double freqMax;
}; //Todo (0mar): Create a configuration data structure

rMatrices1D getRMatrices1D(const double rho1, const double rho2, const double rho);
rMatrices2D getRMatrices2D(const double rhoLeft, const double rhoRight, const double rhoUpper,
                                const double rhoLower,const double rho);
Array<double,9,1> getGridSpacing(const Config cnf);
int next2Power(double n);
#endif //OPENPSTD_KERNEL_FUNCTIONS_H
