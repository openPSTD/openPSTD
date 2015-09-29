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
// Authors: 0mar
//
//
// Purpose:
//
//
//////////////////////////////////////////////////////////////////////////
#include "wave_numbers.h"

using namespace kernel;

WaveNumberDiscretizer::WaveNumberDiscretizer() {

}

Discretization WaveNumberDiscretizer::discretize_wave_numbers(double dx, int N) {
    double max_wave_number = M_PI / dx;
    int two_power = pow(2, N - 1);
    double dka = max_wave_number / two_power;
    Discretization discr;
    double wave = 0;
    std::vector<double> wave_numbers;
    while (wave < max_wave_number + dka) {
        wave_numbers.push_back(wave);
        wave += dka;
    }
    wave = max_wave_number - dka;
    while (wave < 0) {
        wave_numbers.push_back(wave);
        wave -= dka;
    }
    discr.wave_numbers(wave_numbers.data());
    std::complex<double> i(0, 1);
    Eigen::ArrayXd im_part = Eigen::ArrayXd::Constant(two_power + 1, 1) * i;
    Eigen::ArrayXd re_part = Eigen::ArrayXd::Constant(two_power - 1, -1);
    discr.imag_factors(im_part.rows() + re_part.rows()); // Right init?
    discr.imag_factors << im_part, re_part;
    return discr;
}
