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

BOOST_AUTO_TEST_SUITE(Jacobi_GL)
    BOOST_AUTO_TEST_CASE(JacobiGL_1_D) {
        VectorX<double> x(2);

        x << -1, 1;

        auto result = JacobiGL<double>(0, 0, 1);
        BOOST_CHECK(result.isApprox(x));
    }

    BOOST_AUTO_TEST_CASE(JacobiGL_2_D) {
        VectorX<double> x(3);

        x << -1, 0, 1;

        auto result = JacobiGL<double>(0, 0, 2);
        BOOST_CHECK(result.isApprox(x));
    }

    BOOST_AUTO_TEST_CASE(JacobiGL_10_D) {
        VectorX<double> x(11);

        x <<
        -1.000000000000000,
                -0.934001430408059,
                -0.784483473663144,
                -0.565235326996205,
                -0.295758135586939,
                0.000000000000000,
                0.295758135586939,
                0.565235326996205,
                0.784483473663144,
                0.934001430408059,
                1.000000000000000;

        auto result = JacobiGL<double>(0, 0, 10);
        BOOST_CHECK(result.isApprox(x));
    }

    BOOST_AUTO_TEST_CASE(JacobiGL_10_1_2_D) {
        VectorX<double> x(11);

        x <<
        -1.000000000000000,
                -0.860343493384206,
                -0.683403146500607,
                -0.461212632078871,
                -0.208395349171663,
                0.057942830126262,
                0.319697619638539,
                0.559057859234526,
                0.759746364347958,
                0.908214295614149,
                1.000000000000000;

        auto result = JacobiGL<double>(1, 2, 10);
        BOOST_CHECK(result.isApprox(x));
    }
BOOST_AUTO_TEST_SUITE_END()