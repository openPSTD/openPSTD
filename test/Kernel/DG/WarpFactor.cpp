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
// Date: 22-6-2016
//
//
// Authors: M. R. Fortuin
//
//
// Purpose:
//
//
//////////////////////////////////////////////////////////////////////////

#include "Macros.h"

#include <boost/test/unit_test.hpp>
#include <kernel/DG/WarpFactor.h>
#include <iostream>

using namespace OpenPSTD::Kernel::DG;
using namespace std;
using namespace Eigen;

BOOST_AUTO_TEST_SUITE(DG)
    BOOST_AUTO_TEST_SUITE(WarpFactor_tests)
        BOOST_AUTO_TEST_CASE(WarpFactor_1)
        {
            int N = 5;
            VectorX<double> rout(21);
            rout << -1,-0.60000000000000009,-0.19999999999999996,0.19999999999999996,0.60000000000000009,1,-0.80000000000000004,-0.40000000000000008,0,0.39999999999999991,0.80000000000000004,-0.59999999999999998,-0.19999999999999996,0.20000000000000007,0.59999999999999998,-0.40000000000000002,0,0.40000000000000002,-0.19999999999999996,0.20000000000000007,0;

            ArrayX<double> warpExpected(21);
            warpExpected << 0,-0.25789894363978888,-0.088782829667338789,0.088782829667338747,0.25789894363978838,0,-0.33400745526378611,-0.17545327299412078,0,0.17545327299412047,0.33400745526378539,-0.25789894363978888,-0.088782829667338789,0.088782829667338817,0.25789894363978844,-0.17545327299412078,0,0.17545327299412056,-0.088782829667338789,0.088782829667338817,-1.3877787807814457e-17;

            ArrayX<double> warp = WarpFactor<double>(N, rout);

            BOOST_CHECK(warp.isApprox(warpExpected, 1e-8));
        }

        BOOST_AUTO_TEST_CASE(WarpFactor_2)
        {
            int N = 5;
            VectorX<double> rout(21);
            rout << 0,-0.20000000000000001,-0.40000000000000002,-0.59999999999999998,-0.80000000000000004,-1,0.20000000000000001,0,-0.20000000000000001,-0.39999999999999997,-0.60000000000000009,0.40000000000000002,0.20000000000000001,0,-0.19999999999999996,0.59999999999999998,0.39999999999999997,0.19999999999999996,0.80000000000000004,0.60000000000000009,1;

            ArrayX<double> warpExpected(21);
            warpExpected << 0,-0.088782829667338817,-0.17545327299412078,-0.25789894363978888,-0.33400745526378611,0,0.088782829667338761,0,-0.088782829667338817,-0.17545327299412072,-0.25789894363978888,0.17545327299412056,0.088782829667338761,0,-0.088782829667338789,0.25789894363978844,0.17545327299412047,0.088782829667338747,0.33400745526378539,0.25789894363978838,0;

            ArrayX<double> warp = WarpFactor<double>(N, rout);

            BOOST_CHECK(warp.isApprox(warpExpected, 1e-8));
        }

        BOOST_AUTO_TEST_CASE(WarpFactor_3)
        {
            int N = 5;
            VectorX<double> rout(21);
            rout << 1,0.80000000000000004,0.59999999999999998,0.40000000000000002,0.19999999999999996,0,0.60000000000000009,0.40000000000000008,0.20000000000000001,5.5511151231257827e-17,-0.20000000000000001,0.19999999999999996,-5.5511151231257827e-17,-0.20000000000000007,-0.40000000000000002,-0.19999999999999996,-0.39999999999999997,-0.59999999999999998,-0.60000000000000009,-0.80000000000000004,-1;

            ArrayX<double> warpExpected(21);
            warpExpected << 0,0.33400745526378539,0.25789894363978844,0.17545327299412056,0.088782829667338747,0,0.25789894363978838,0.17545327299412058,0.088782829667338761,0,-0.088782829667338817,0.088782829667338747,-1.3877787807814457e-17,-0.088782829667338817,-0.17545327299412078,-0.088782829667338789,-0.17545327299412072,-0.25789894363978888,-0.25789894363978888,-0.33400745526378611,0;

            ArrayX<double> warp = WarpFactor<double>(N, rout);

            BOOST_CHECK(warp.isApprox(warpExpected, 1e-8));
        }
    BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()