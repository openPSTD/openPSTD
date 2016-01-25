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
//////////////////////////////////////////////////////////////////////////

#include "wave_numbers.h"

using namespace Kernel;
using namespace std;

WaveNumberDiscretizer::WaveNumberDiscretizer() {
    //I believe this does not need any init
}

WaveNumberDiscretizer::Discretization WaveNumberDiscretizer::get_discretization(float dx, int N) {
    int matched_int = this->match_number(N);
    auto search = this->computed_discretization.find(matched_int);
    if (search != this->computed_discretization.end()) {
        return search->second;
    } else {
        Discretization new_wave_discretizer = discretize_wave_numbers(dx, matched_int);
        computed_discretization[matched_int] = new_wave_discretizer;
        return new_wave_discretizer;
    }
}


WaveNumberDiscretizer::Discretization WaveNumberDiscretizer::discretize_wave_numbers(float dx, int N) {
    float max_wave_number = (float) M_PI / dx;
    int two_power = (int) pow(2, N - 1);

    float dka = max_wave_number / two_power;
    Discretization discr;
    float wave = 0;
    discr.wave_numbers = Eigen::ArrayXf(2 * two_power);
    unsigned long index = 0;

    discr.wave_numbers.head(two_power + 1) = Eigen::ArrayXf::LinSpaced(two_power + 1, 0, max_wave_number);
    discr.wave_numbers.tail(two_power - 1) = Eigen::ArrayXf::LinSpaced(two_power - 1, max_wave_number - dka, dka);
    discr.complex_factors = Eigen::ArrayXcf(2 * two_power);
    Eigen::ArrayXf partial_ones = Eigen::ArrayXf::Ones(2 * two_power);
    partial_ones.tail(two_power - 1) = -Eigen::ArrayXf::Ones(two_power - 1);
    discr.complex_factors.imag() = partial_ones;
    discr.complex_factors.real() = Eigen::ArrayXf::Zero(2 * two_power);
    Eigen::ArrayXcf complex_wave_numbers = discr.complex_factors * discr.wave_numbers;
    discr.pressure_deriv_factors = (-complex_wave_numbers * (dx * 0.5)).exp() * complex_wave_numbers;
    discr.velocity_deriv_factors = (complex_wave_numbers * (dx * 0.5)).exp() * complex_wave_numbers;
    return discr;
}

int WaveNumberDiscretizer::match_number(int n) {
    return (int) ceil(log2(n));
}


ostream &::Kernel::operator<<(ostream &str, WaveNumberDiscretizer const &v) {
    string number_repr;
    for (auto iterator = v.computed_discretization.begin(); iterator != v.computed_discretization.end(); iterator++) {
        number_repr += "n = 2^" + to_string(iterator->first) + " ";
    }
    return str << "Wavenumberdiscretizations: " << number_repr;
}
