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
#include "../core/kernel_functions.h"
#include <cmath>

using namespace Kernel;
using namespace std;
using namespace Eigen;
BOOST_AUTO_TEST_SUITE(kernel_functions)

    BOOST_AUTO_TEST_CASE(test_next_2_power) {
        BOOST_CHECK_EQUAL(next_2_power(42), 64);
        BOOST_CHECK_EQUAL(next_2_power(3.4), 4);
        BOOST_CHECK_EQUAL(next_2_power(0.1), 1);
    }

    BOOST_AUTO_TEST_CASE(test_rho_array_one_neighbour) {
        float air_dens = 1.2;
        float max_rho = 1E10;
        Array<float, 4, 2> velocity;
        Array<float, 4, 2> pressure;
        velocity << -1, 1, 0, 0, 2, 0, 1, 1;
        pressure << 1, -1, 0, 0, 2, 0, 1, 1;
        RhoArray rhoArray = get_rho_array(max_rho, air_dens, air_dens);
        BOOST_CHECK(rhoArray.pressure.isApprox(pressure));
        BOOST_CHECK(rhoArray.velocity.isApprox(velocity));
    }

    BOOST_AUTO_TEST_CASE(test_rho_array_two_neighbour) {
        float air_dens = 1.2;
        Array<float, 4, 2> velocity;
        Array<float, 4, 2> pressure;
        velocity << 0, 0, 0, 0, 1, 1, 1, 1;
        pressure << 0, 0, 0, 0, 1, 1, 1, 1;
        RhoArray rhoArray = get_rho_array(air_dens, air_dens, air_dens);
        BOOST_CHECK(rhoArray.pressure.isApprox(pressure));
        BOOST_CHECK(rhoArray.velocity.isApprox(velocity));
    }

    BOOST_AUTO_TEST_CASE(test_get_grid_spacing) {
        //Todo: Implement when PSTDFILESettings cnf is implemented
        BOOST_CHECK(true);
    }


BOOST_AUTO_TEST_SUITE_END()