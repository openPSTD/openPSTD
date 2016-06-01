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
#include <kernel/DG/DMatrix.h>

using namespace OpenPSTD::Kernel::DG;
using namespace std;
using namespace Eigen;

BOOST_AUTO_TEST_SUITE(DG)
    BOOST_AUTO_TEST_SUITE(D_Matrix)
        BOOST_AUTO_TEST_CASE(DMatrix_1D_2) {
            int N = 3;
            VectorX<double> r(N+1);
            MatrixX<double> V(N+1, N+1);
            MatrixX<double> DrExpected(N+1,N+1);

            r <<
            -1.000000000000000,
            -0.447213595499958,
            0.447213595499958,
            1.000000000000000;

            V <<
            0.707106781186547,  -1.224744871391589,   1.581138830084190,  -1.870828693386972,
            0.707106781186547,  -0.547722557505166,  -0.316227766016838,   0.836660026534076,
            0.707106781186547,   0.547722557505166,  -0.316227766016838,  -0.836660026534076,
            0.707106781186547,   1.224744871391589,   1.581138830084190,   1.870828693386972;

            DrExpected <<
            -2.999999999999999,   4.045084971874736,  -1.545084971874736,   0.500000000000000,
            -0.809016994374947,  -0.000000000000000,   1.118033988749895,  -0.309016994374947,
             0.309016994374947,  -1.118033988749895,   0.000000000000000,   0.809016994374947,
            -0.500000000000000,   1.545084971874736,  -4.045084971874735,   2.999999999999999;

            MatrixX<double> Dr = DMatrix1D<double>(N, r, V);
            BOOST_CHECK(Dr.isApprox(DrExpected));
        }
    BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
