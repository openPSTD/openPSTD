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
#include "../../kernel/core/Geometry.h"
#include <cmath>

using namespace OpenPSTD::Kernel;
using namespace std;
BOOST_AUTO_TEST_SUITE(geometry)

    BOOST_AUTO_TEST_CASE(point_addition) {
        Point point1 = Point(2, 3);
        Point point2(5, 2);
        Point resultpoint(7, 5);
        BOOST_CHECK_EQUAL((point1 + point2).x, resultpoint.x);
        BOOST_CHECK_EQUAL((point1 + point2).y, resultpoint.y);
    }

    BOOST_AUTO_TEST_CASE(point_substraction) {
        Point point1 = Point(2, 3);
        Point point2(5, 2);
        Point resultpoint(-3, 1);
        BOOST_CHECK_EQUAL((point1 - point2).x, resultpoint.x);
        BOOST_CHECK_EQUAL((point1 - point2).y, resultpoint.y);
    }


BOOST_AUTO_TEST_SUITE_END()