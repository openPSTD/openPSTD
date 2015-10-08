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

using namespace Kernel;

WaveNumberDiscretizer::WaveNumberDiscretizer() {

}

WaveNumberDiscretizer::Discretization WaveNumberDiscretizer::get_discretization(double dx, int N) {
    int matched_int = this->match_number(N);
    auto search = this->computed_discretization.find(matched_int);
    if (search != this->computed_discretization.end()) {
        return search->second;
    } else {
        Discretization new_wave_discretizer = get_discretization(dx, matched_int);
        computed_discretization[matched_int] = new_wave_discretizer;
        return new_wave_discretizer;
    }
}


WaveNumberDiscretizer::Discretization WaveNumberDiscretizer::discretize_wave_numbers(double dx, int N) {
    double max_wave_number = M_PI / dx;
    int two_power = pow(2, N - 1);
    double dka = max_wave_number / two_power;
    Discretization discr;
    double wave = 0;
    int wave_number_amount1 = ceil(max_wave_number / dka);
    int wave_number_amount2 = ceil((max_wave_number - dka) / dka);
    Eigen::ArrayXXcd wave_number_array(wave_number_amount1 + wave_number_amount2, 1);
    while (wave < max_wave_number + dka) {
        wave_number_array << wave;
        wave += dka;
    }
    wave = max_wave_number - dka;
    while (wave < 0) {
        wave_number_array << wave;
        wave -= dka;
    }
    discr.wave_numbers = std::make_shared<Eigen::ArrayXXcd>(wave_number_array);
    Eigen::ArrayXXcd complex_factor_array(2 * two_power, 1);
    std::complex<double> i(0, 1);
    for (int j = 0; j < two_power + 1; j++) {
        complex_factor_array.real() << 1;
    }
    for (int j = 0; j < two_power - 1; j++) {
        complex_factor_array.imag() << -1;
    }

    discr.complex_factors = std::make_shared<Eigen::ArrayXXcd>(complex_factor_array);
    Eigen::ArrayXXcd pderfact_array(complex_factor_array.rows(), 1);
    Eigen::ArrayXXcd vderfact_array(complex_factor_array.rows(), 1);
    for (int j = 0; j < complex_factor_array.rows(); j++) {
        pderfact_array << exp(-complex_factor_array(j) * wave_number_array(j) * dx / 2.)
                          * complex_factor_array(j) * wave_number_array(j);
        vderfact_array << exp(complex_factor_array(j) * wave_number_array(j) * dx / 2.)
                          * complex_factor_array(j) * wave_number_array(j);
    }
    discr.pressure_deriv_factors = std::make_shared<Eigen::ArrayXXcd>(pderfact_array);
    discr.velocity_deriv_factors = std::make_shared<Eigen::ArrayXXcd>(vderfact_array);

    return discr;
}

int WaveNumberDiscretizer::match_number(int n) {
    return (int) (ceil(log2(n)));
}
