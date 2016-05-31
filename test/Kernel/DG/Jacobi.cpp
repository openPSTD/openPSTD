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


    BOOST_AUTO_TEST_CASE(JacobiGQ_10_F) {
        VectorX<float> x(11);
        VectorX<float> w(11);

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

        auto result = JacobiGQ<float>(0, 0, 10, true);
        BOOST_CHECK(result.Points.isApprox(x, 1e-4));
        BOOST_CHECK(result.Weights.isApprox(w, 1e-4));
    }

    BOOST_AUTO_TEST_CASE(JacobiGQ_0_F) {
        VectorX<float> x(1);
        VectorX<float> w(1);

        w << 2;
        x << 0;
        auto result = JacobiGQ<float>(0, 0, 0, true);

        BOOST_CHECK(result.Points.isApprox(x, 1e-4));
        BOOST_CHECK(result.Weights.isApprox(w, 1e-4));
    }

    BOOST_AUTO_TEST_CASE(JacobiGQ_10_1_2_F) {
        VectorX<float> x(11);
        VectorX<float> w(11);

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

        JacobiGQResult<float> result;
        result = JacobiGQ<float>(1, 2, 10, true);
        BOOST_CHECK(result.Points.isApprox(x, 1e-4));
        BOOST_CHECK(result.Weights.isApprox(w, 1e-4));
    }

    BOOST_AUTO_TEST_CASE(JacobiGQ_0_1_2_F) {
        VectorX<float> x(1);
        VectorX<float> w(1);

        w << 2;
        x << 0.2;

        auto result = JacobiGQ<float>(1, 2, 0, true);

        BOOST_CHECK(result.Points.isApprox(x, 1e-4));
        BOOST_CHECK(result.Weights.isApprox(w, 1e-4));
    }

    BOOST_AUTO_TEST_CASE(JacobiGQ_10_D) {
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

        auto result = JacobiGQ<double>(0, 0, 10, true);
        BOOST_CHECK(result.Points.isApprox(x));
        BOOST_CHECK(result.Weights.isApprox(w));
    }

    BOOST_AUTO_TEST_CASE(JacobiGQ_0_D) {
        VectorX<double> x(1);
        VectorX<double> w(1);

        w << 2;
        x << 0;
        auto result = JacobiGQ<double>(0, 0, 0, true);

        BOOST_CHECK(result.Points.isApprox(x));
        BOOST_CHECK(result.Weights.isApprox(w));
    }

    BOOST_AUTO_TEST_CASE(JacobiGQ_10_1_2_D) {
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

        auto result = JacobiGQ<double>(1, 2, 10, true);

        BOOST_CHECK(result.Points.isApprox(x));
        BOOST_CHECK(result.Weights.isApprox(w));
    }

    BOOST_AUTO_TEST_CASE(JacobiGQ_0_1_2_D) {
        VectorX<double> x(1);
        VectorX<double> w(1);

        w << 2;
        x << 0.2;

        auto result = JacobiGQ<double>(1, 2, 0, true);

        BOOST_CHECK(result.Points.isApprox(x));
        BOOST_CHECK(result.Weights.isApprox(w));
    }

BOOST_AUTO_TEST_SUITE_END()