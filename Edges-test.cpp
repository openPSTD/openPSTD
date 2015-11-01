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
// Date: 1-11-2015
//
//
// Authors: M. R. Fortuin
//
//
// Purpose:
//
//
//////////////////////////////////////////////////////////////////////////


#define BOOST_TEST_DYN_LINK
#ifdef STAND_ALONE
#   define BOOST_TEST_MODULE Main
#endif
#include <boost/test/unit_test.hpp>
#include "Edges.h"

BOOST_AUTO_TEST_SUITE(GUI_Edges)

    BOOST_AUTO_TEST_CASE(TestVertical)
    {
        Edge e(QVector2D(0, 10), QVector2D(10, 10), 0, false);
        BOOST_REQUIRE(e.IsHorizontal());
    }

    BOOST_AUTO_TEST_CASE(TestVertical2)
    {
        Edge e(QVector2D(10, 0), QVector2D(10, 10), 0, false);
        BOOST_REQUIRE(e.IsHorizontal());
    }

    BOOST_AUTO_TEST_CASE(TestHorizontal)
    {
        Edge e(QVector2D(10, 0), QVector2D(10, 10), 0, false);
        BOOST_REQUIRE(e.IsVertical());
    }

    BOOST_AUTO_TEST_CASE(TestHorizontal2)
    {
        Edge e(QVector2D(0, 10), QVector2D(10, 10), 0, false);
        BOOST_REQUIRE(!e.IsVertical());
    }

BOOST_AUTO_TEST_SUITE_END()