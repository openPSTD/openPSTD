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
#include "../core/Domain.h"

#include <cmath>

using namespace Kernel;
using namespace std;
using namespace Eigen;
BOOST_AUTO_TEST_SUITE(domain)

    shared_ptr<Kernel::Domain> create_a_domain(int size_x,int size_y) {
        Point top_left(0, 0);
        Point size(size_x,size_y);
        shared_ptr<WaveNumberDiscretizer> wnd(new WaveNumberDiscretizer());
        EdgeParameters standard = {};
        standard.locally_reacting = true;
        standard.alpha = 1;
        shared_ptr<PSTDFileSettings> settings(new PSTDFileSettings());
        map<Direction, EdgeParameters> edge_param_map = {{Direction::LEFT,   standard},
                                                         {Direction::RIGHT,  standard},
                                                         {Direction::TOP,    standard},
                                                         {Direction::BOTTOM, standard}};
        shared_ptr<Kernel::Domain> test_domain(
                new Kernel::Domain(settings, "test_domain", 1, top_left, size, false, wnd,
                                   edge_param_map, nullptr));
        return test_domain;
    }
    BOOST_AUTO_TEST_CASE(domain_initialization) {
        auto domain = create_a_domain(100,150);
        BOOST_CHECK(true);
    }

    BOOST_AUTO_TEST_CASE(domain_dimensions) {
        auto domain = create_a_domain(100,150);
        BOOST_CHECK_EQUAL(domain->size.x,100);
        BOOST_CHECK_EQUAL(domain->size.y,150);

    }


    BOOST_AUTO_TEST_CASE(domain_pml_checks) {
        //Check whether we have the right number of pml domains with the right dimensions
        BOOST_CHECK(true);

    }

    BOOST_AUTO_TEST_CASE(domain_rho_arrays) {
        //Test the rho arrays
        BOOST_CHECK(true);
    }

    BOOST_AUTO_TEST_CASE(domain_clear_methods) {
        //Test the clear matrix/arrays methods
        BOOST_CHECK(true);
    }

    BOOST_AUTO_TEST_CASE(domain_push_values) {
        //Test data pushing
        BOOST_CHECK(true);
    }

    BOOST_AUTO_TEST_CASE(domain_pml_matrices) {
        //Test the computation and application of pmlmatrices
        BOOST_CHECK(true);
    }

    BOOST_AUTO_TEST_CASE(domain_neighbours) {
        //Test whether we have the right number of neighbours in the right places
        BOOST_CHECK(true);
    }

    BOOST_AUTO_TEST_CASE(domain_get_points) {
        //Test the points method of the domain.
        BOOST_CHECK(true);
    }

    BOOST_AUTO_TEST_CASE(domain_test_calc) {
        //test the calc method
        BOOST_CHECK(true);
    }

    BOOST_AUTO_TEST_CASE(domain_get_vacant_range) {
        //Test the vacant range method
        BOOST_CHECK(true);
    }

BOOST_AUTO_TEST_SUITE_END()