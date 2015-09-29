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
// and optimize them for FFT computations
//
//
//////////////////////////////////////////////////////////////////////////
#ifndef OPENPSTD_WAVE_NUMBERS_H
#define OPENPSTD_WAVE_NUMBERS_H

#include <map>
#include <math.h>
#include <complex>
#include <eigen/Eigen/Dense>

using namespace kernel;

class WaveNumberDiscretizer {
public:
    struct Discretization {
        Eigen::ArrayXd wave_numbers;
        Eigen::ArrayXd imag_factors;
    };
    std::map<int, Discretization> computed_discretization;

    Discretization get_discretization(double dx, int N); // Method get


    WaveNumberDiscretizer();
    //~WaveNumberDiscretizer();

private:
    Discretization discretize_wave_numbers(double dx, int N); //Todo: Needs a better name
    void compute_wave_number_discretization(double dx, int N); // Method calc
};

#endif //OPENPSTD_WAVE_NUMBERS_H
