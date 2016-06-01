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
// Date: 1-6-2016
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
#include <kernel/DG/Lift.h>

using namespace OpenPSTD::Kernel::DG;
using namespace std;
using namespace Eigen;

BOOST_AUTO_TEST_SUITE(DG)
    BOOST_AUTO_TEST_SUITE(Lift)
        BOOST_AUTO_TEST_CASE(DMatrix_1D_2) {
            int N = 3;
            MatrixX<double> V(N+1, N+1);
            MatrixX<double> LiftExpected(N+1,2);

            V <<
            0.707106781186547,  -1.224744871391589,   1.581138830084190,  -1.870828693386972,
            0.707106781186547,  -0.547722557505166,  -0.316227766016838,   0.836660026534076,
            0.707106781186547,   0.547722557505166,  -0.316227766016838,  -0.836660026534076,
            0.707106781186547,   1.224744871391589,   1.581138830084190,   1.870828693386972;

            LiftExpected <<
                     8.000000000000004,  -2.000000000000003,
                    -0.894427190999917,   0.894427190999917,
                     0.894427190999917,  -0.894427190999917,
                    -2.000000000000003,   8.000000000000004;

            MatrixX<double> Lift = Lift1D<double>(N+1, 2, V);
            BOOST_CHECK(Lift.isApprox(LiftExpected));
        }
    BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
