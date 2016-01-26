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
// Date: 2-1-2016
//
//
// Authors: Omar Richardson
//
//
// Purpose: Test suite for kernel functions
//
//
//////////////////////////////////////////////////////////////////////////


#ifdef STAND_ALONE
#   define BOOST_TEST_MODULE Main
#endif

#include <boost/test/unit_test.hpp>
#include "../core/wave_numbers.h"
#include <cmath>
#include <kernel/core/kernel_functions.h>

using namespace Kernel;
using namespace std;
using namespace Eigen;

BOOST_AUTO_TEST_SUITE(wave_numbers)

    BOOST_AUTO_TEST_CASE(test_matching) {
        int size1 = 115;
        int size2 = 178;
        int size3 = 227;
        float dx = 0.2;
        WisdomCache wnd = WisdomCache();
        WisdomCache::Discretization discr1 = wnd.get_discretization(dx, size1);
        WisdomCache::Discretization discr2 = wnd.get_discretization(dx, size2);
        WisdomCache::Discretization discr3 = wnd.get_discretization(dx, size3);

        BOOST_CHECK_EQUAL(discr2.wave_numbers.size(), discr3.wave_numbers.size());
        BOOST_CHECK_EQUAL(discr2.wave_numbers.size(), 2 * discr1.wave_numbers.size());
        //Check maximum of wavenumbers
        //check all wavenumbers bigger zero
        //Check several values

    }

    BOOST_AUTO_TEST_CASE(test_wavenumber_bounds) {
        int size1 = 115;
        float dx = 0.2;
        WisdomCache wnd = WisdomCache();
        WisdomCache::Discretization discr1 = wnd.get_discretization(dx, size1);
        BOOST_CHECK(discr1.wave_numbers.maxCoeff() <= 15.8);
        BOOST_CHECK(discr1.wave_numbers.minCoeff() >= 0);
        // Value from default python run.
        // Computed by linear stepping from 0 with minimum wave length to middle of array
    }


    BOOST_AUTO_TEST_CASE(test_discretized_values) {
        int size1 = 115;
        float dx = 0.2;
        WisdomCache wnd = WisdomCache();
        WisdomCache::Discretization discr1 = wnd.get_discretization(dx, size1);
        BOOST_CHECK(is_approx(discr1.wave_numbers.coeff(1), 0.245437));
        BOOST_CHECK(is_approx(discr1.wave_numbers.coeff(99), 7.1176707));

        BOOST_CHECK(is_approx(discr1.complex_factors.imag().coeff(64), 1));
        BOOST_CHECK(is_approx(discr1.complex_factors.imag().coeff(65), -1));
        BOOST_CHECK(is_approx(discr1.complex_factors.real().coeff(64), 0));
        BOOST_CHECK(is_approx(discr1.complex_factors.real().coeff(65), 0));

        BOOST_CHECK(is_approx(discr1.pressure_deriv_factors.real().coeff(39), 7.8259545));
        BOOST_CHECK(is_approx(discr1.pressure_deriv_factors.imag().coeff(39), 5.511659));

        BOOST_CHECK(is_approx(discr1.velocity_deriv_factors.real().coeff(56), -13.480372));
        BOOST_CHECK(is_approx(discr1.velocity_deriv_factors.imag().coeff(56), 2.681413));

        // Values from default python run. analytical reproduction should be possible.
    }


BOOST_AUTO_TEST_SUITE_END()