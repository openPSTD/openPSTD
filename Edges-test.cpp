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
// Purpose: Test cases for de Edge class
//
//
//////////////////////////////////////////////////////////////////////////


#ifdef STAND_ALONE
#   define BOOST_TEST_MODULE Main
#endif
#include <boost/test/unit_test.hpp>
#include "Edges.h"

BOOST_AUTO_TEST_SUITE(GUI_Edges)

    BOOST_AUTO_TEST_CASE(TestHorizontal)
    {
        BOOST_CHECK(Edge(QVector2D(0, 10), QVector2D(10, 10), 0, false).IsHorizontal());
        BOOST_CHECK(!Edge(QVector2D(10, 0), QVector2D(10, 10), 0, false).IsHorizontal());
    }

    BOOST_AUTO_TEST_CASE(TestVertical)
    {
        BOOST_CHECK(Edge(QVector2D(10, 0), QVector2D(10, 10), 0, false).IsVertical());
        BOOST_CHECK(!Edge(QVector2D(0, 10), QVector2D(10, 10), 0, false).IsVertical());
    }

    BOOST_AUTO_TEST_CASE(TestGetStart)
    {
        BOOST_CHECK(Edge(QVector2D(0, 10), QVector2D(10, 10), 0, false).GetStart() == QVector2D(0, 10));
    }

    BOOST_AUTO_TEST_CASE(TestGetEnd)
    {
        BOOST_CHECK(Edge(QVector2D(0, 10), QVector2D(10, 10), 0, false).GetEnd() == QVector2D(10, 10));
    }

    BOOST_AUTO_TEST_CASE(TestOnSameLine)
    {
        BOOST_CHECK(Edge::OnSameLine(Edge(QVector2D(0, 10), QVector2D(10, 10), 0, false), Edge(QVector2D(0, 10), QVector2D(10, 10), 0, false)));
        BOOST_CHECK(!Edge::OnSameLine(Edge(QVector2D(0, 11), QVector2D(10, 11), 0, false), Edge(QVector2D(0, 10), QVector2D(10, 10), 0, false)));
        BOOST_CHECK(!Edge::OnSameLine(Edge(QVector2D(10, 0), QVector2D(10, 10), 0, false), Edge(QVector2D(0, 10), QVector2D(10, 10), 0, false)));
        BOOST_CHECK(Edge::OnSameLine(Edge(QVector2D(0, 10), QVector2D(10, 10), 0, false), Edge(QVector2D(10, 10), QVector2D(20, 10), 0, false)));
        BOOST_CHECK(!Edge::OnSameLine(Edge(QVector2D(0, 0), QVector2D(10, 10), 0, false), Edge(QVector2D(10, 10), QVector2D(20, 10), 0, false)));
    }

    BOOST_AUTO_TEST_CASE(TestSubstract)
    {
        BOOST_CHECK(Edge(QVector2D(0, 10), QVector2D(10, 10), 0, false).Substract(Edge(QVector2D(0, 10), QVector2D(10, 10), 0, false)).empty());

        BOOST_CHECK(Edge(QVector2D(0, 10), QVector2D(10, 10), 0, false).Substract(Edge(QVector2D(0, 10), QVector2D(5, 10), 0, false))[0].GetStart() == QVector2D(5, 10));
        BOOST_CHECK(Edge(QVector2D(0, 10), QVector2D(10, 10), 0, false).Substract(Edge(QVector2D(0, 10), QVector2D(5, 10), 0, false))[0].GetEnd() == QVector2D(10, 10));

        BOOST_CHECK(Edge(QVector2D(0, 10), QVector2D(10, 10), 0, false).Substract(Edge(QVector2D(5, 10), QVector2D(10, 10), 0, false))[0].GetStart() == QVector2D(0, 10));
        BOOST_CHECK(Edge(QVector2D(0, 10), QVector2D(10, 10), 0, false).Substract(Edge(QVector2D(5, 10), QVector2D(10, 10), 0, false))[0].GetEnd() == QVector2D(5, 10));
    }


BOOST_AUTO_TEST_SUITE_END()