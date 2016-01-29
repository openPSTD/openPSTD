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
#include "../../kernel/core/Speaker.h"
#include <cmath>

using namespace Kernel;
using namespace std;
using namespace Eigen;
BOOST_AUTO_TEST_SUITE(speaker)

    BOOST_AUTO_TEST_CASE(test_speaker_contribution_in_bounds) {
//        shared_ptr<Point> top_left(new Point(0, 0));
//        shared_ptr<Point> size(new Point(100, 150));
//        shared_ptr<WisdomCache> wnd(new WisdomCache());
//        EdgeParameters standard = {};
//        standard.locally_reacting = true;
//        standard.alpha = 1;
//        map<Direction, EdgeParameters> edge_param_map = {{Direction::LEFT,   standard},
//                                                         {Direction::RIGHT,  standard},
//                                                         {Direction::TOP,    standard},
//                                                         {Direction::BOTTOM, standard}};
//        shared_ptr<Kernel::Domain> test_domain(new Kernel::Domain(nullptr, "test_domain", 1, top_left, size, false, wnd,
//                                                                  edge_param_map, nullptr));
        BOOST_CHECK(true);

    }

    BOOST_AUTO_TEST_CASE(test_values_case1) {
        //Implement when domains can be initialized
        BOOST_CHECK(true);
    }

    BOOST_AUTO_TEST_CASE(test_values_case2) {
        //Implement when domains can be initialized
        BOOST_CHECK(true);
    }

BOOST_AUTO_TEST_SUITE_END()