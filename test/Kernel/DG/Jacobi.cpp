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
// Date: 30-5-2016
//
//
// Authors: M. R. Fortuin
//
//
// Purpose:
//
//
//////////////////////////////////////////////////////////////////////////

#ifdef STAND_ALONE
#   define BOOST_TEST_MODULE Main
#endif

#include <boost/test/unit_test.hpp>
#include <kernel/DG/Jacobi.h>

using namespace OpenPSTD::Kernel::DG;
using namespace std;
using namespace Eigen;

BOOST_AUTO_TEST_SUITE(domain)
    BOOST_AUTO_TEST_CASE(JacobiGQ_10) {
        VectorX<double> x(11);
        VectorX<double> w(11);

        x <<
        -0.978228658146057,
        -0.887062599768095,
        -0.730152005574049,
        -0.519096129206812,
        -0.269543155952345,
        -0.000000000000000,
        0.269543155952345,
        0.519096129206812,
        0.730152005574049,
        0.887062599768095,
        0.978228658146057;

        w <<
        0.055668567116174,
        0.125580369464905,
        0.186290210927734,
        0.233193764591990,
        0.262804544510247,
        0.272925086777900,
        0.262804544510247,
        0.233193764591990,
        0.186290210927733,
        0.125580369464905,
        0.055668567116174;

        auto result = JacobiGQ(0, 0, 10, true);
        BOOST_CHECK(result.X.isApprox(x));
        BOOST_CHECK(result.W.isApprox(w));
    }

    BOOST_AUTO_TEST_CASE(JacobiGQ_0) {
        VectorX<double> x(1);
        VectorX<double> w(1);

        w << 2;
        x << 0;
        auto result = JacobiGQ(0, 0, 0, true);

        BOOST_CHECK(result.X.isApprox(x));
        BOOST_CHECK(result.W.isApprox(w));
    }

    BOOST_AUTO_TEST_CASE(JacobiGQ_10_1_2) {
        VectorX<double> x(11);
        VectorX<double> w(11);

        w <<
        0.001149899262705,
        0.011115234598547,
        0.043695749536401,
        0.106664964259534,
        0.188071472702823,
        0.255700412200876,
        0.274608570896453,
        0.231527186198693,
        0.146741924678359,
        0.062045473719344,
        0.012012445279598;

        x <<
        -0.922750750150548,
        -0.797015360241449,
        -0.625356093379147,
        -0.417646665061861,
        -0.185937243641642,
        0.056312853705647,
        0.295027635808207,
        0.516334958645484,
        0.707373276746721,
        0.857038210757503,
        0.956619176811086;

        auto result = JacobiGQ(1, 2, 10, true);

        BOOST_CHECK(result.X.isApprox(x));
        BOOST_CHECK(result.W.isApprox(w));
    }

    BOOST_AUTO_TEST_CASE(JacobiGQ_0_1_2) {
        VectorX<double> x(1);
        VectorX<double> w(1);

        w << 2;
        x << 0.2;

        auto result = JacobiGQ(1, 2, 0, true);

        BOOST_CHECK(result.X.isApprox(x));
        BOOST_CHECK(result.W.isApprox(w));
    }

BOOST_AUTO_TEST_SUITE_END()