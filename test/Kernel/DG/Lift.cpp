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

        BOOST_AUTO_TEST_CASE(DMatrix_1D_6) {
            int N = 6;
            MatrixX<double> V(N+1, N+1);
            MatrixX<double> LiftExpected(N+1,2);

            V <<
                    0.707106781186547	,	-1.224744871391580	,	1.581138830084190	,	-1.870828693386970	,	2.121320343559640	,	-2.345207879911710	,	2.549509756796390	,
                    0.707106781186547	,	-1.016812459073910	,	0.844182001556940	,	-0.346643573228288	,	-0.278372647949331	,	0.807538898284274	,	-1.057410345369820	,
                    0.707106781186547	,	-0.574220155261391	,	-0.269222426980870	,	0.833675471702112	,	-0.504704283282048	,	-0.365166265456773	,	0.846707059684174	,
                    0.707106781186547	,	0.000000000000000	,	-0.790569415042095	,	0.000000000000000	,	0.795495128834866	,	0.000000000000000	,	-0.796721798998873	,
                    0.707106781186547	,	0.574220155261392	,	-0.269222426980870	,	-0.833675471702112	,	-0.504704283282048	,	0.365166265456773	,	0.846707059684174	,
                    0.707106781186547	,	1.016812459073910	,	0.844182001556939	,	0.346643573228285	,	-0.278372647949334	,	-0.807538898284277	,	-1.057410345369820	,
                    0.707106781186547	,	1.224744871391580	,	1.581138830084190	,	1.870828693386970	,	2.121320343559640	,	2.345207879911710	,	2.549509756796390	;

            LiftExpected <<
                    24.500000000000000	,	3.500000000000000	,
                    -1.451626611323410	,	-1.451626611323440	,
                    1.162370412976330	,	1.162370412976330	,
                    -1.093750000000000	,	-1.093750000000000	,
                    1.162370412976330	,	1.162370412976330	,
                    -1.451626611323440	,	-1.451626611323430	,
                    3.500000000000000	,	24.500000000000000	;


            MatrixX<double> Lift = Lift1D<double>(N+1, 2, V);
            BOOST_CHECK(Lift.isApprox(LiftExpected));
        }
    BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
