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
// Date: 31-5-2016
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

BOOST_AUTO_TEST_SUITE(Jacobi_P)
    BOOST_AUTO_TEST_CASE(JacobiP_10) {
        VectorX<double> x = VectorX<double>::LinSpaced(10, -1, 1);
        auto result = JacobiP<double>(x, 0, 0, 10);

        VectorX<double> expected(10);
        expected <<
        3.240370349203935,
        1.003567253751228,
        -0.870728918581012,
        0.746148379111006,
        -0.311787881060553,
        -0.311787881060553,
        0.746148379111007,
        -0.870728918581012,
        1.003567253751228,
        3.240370349203935;

        BOOST_CHECK(result.isApprox(expected));
    }

    BOOST_AUTO_TEST_CASE(JacobiP_0) {
        VectorX<double> x = VectorX<double>::LinSpaced(10, -1, 1);
        auto result = JacobiP<double>(x, 0, 0, 0);

        VectorX<double> expected(10);
        expected <<
        0.707106781186547,
        0.707106781186547,
        0.707106781186547,
        0.707106781186547,
        0.707106781186547,
        0.707106781186547,
        0.707106781186547,
        0.707106781186547,
        0.707106781186547,
        0.707106781186547;

        BOOST_CHECK(result.isApprox(expected));
    }
BOOST_AUTO_TEST_SUITE_END()