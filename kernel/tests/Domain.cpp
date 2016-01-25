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
#include <kernel/PSTDKernel.h>


using namespace std;
using namespace Eigen;
BOOST_AUTO_TEST_SUITE(domain)

    shared_ptr<Kernel::Scene> create_a_scene() {
        shared_ptr<PSTDFileConfiguration> config = PSTDFile::CreateDefaultConf();
        Domain domain1;
        domain1.TopLeft = QVector2D(0, 0);
        domain1.Size = QVector2D(50, 60);
        domain1.T.Absorption = 0;
        domain1.B.Absorption = 0.25;
        domain1.L.Absorption = 0.35;
        domain1.R.Absorption = 0.7;
        domain1.T.LR = false;
        domain1.B.LR = false;
        domain1.L.LR = true;
        domain1.R.LR = false;
        config->Domains.clear();
        config->Domains.push_back(domain1);
        BOOST_CHECK(config->Domains.size() == 1);
        PSTDKernel kernel = PSTDKernel();

        kernel.start_kernel(config);
        auto scene = kernel.get_scene();
        return scene;
    }

    shared_ptr<Kernel::Domain> create_a_domain(int point_x, int point_y, int size_x, int size_y) {
        using namespace Kernel;
        Point top_left(point_x, point_y);
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
                new Kernel::Domain(settings, 1, 1, top_left, size, false, wnd,
                                   edge_param_map, nullptr));
        return test_domain;
    }

    BOOST_AUTO_TEST_CASE(domain_initialization_from_kernel) {
        auto domain = create_a_domain(-50, -25, 100, 150);
        BOOST_CHECK(!domain->is_pml);
    }

    BOOST_AUTO_TEST_CASE(domain_initialization_from_config) {
        auto scene = create_a_scene();
        BOOST_CHECK_EQUAL(scene->domain_list.size(), 5);
        BOOST_CHECK_EQUAL(scene->domain_list.at(0)->top_left.x, 0);
        BOOST_CHECK_EQUAL(scene->domain_list.at(0)->size.x, 250);
        BOOST_CHECK_EQUAL(scene->domain_list.at(0)->bottom_right.y, 300);
        BOOST_CHECK_EQUAL(scene->domain_list.at(0)->edge_param_map[Kernel::Direction::LEFT].locally_reacting, true);
        BOOST_CHECK(Kernel::is_approx(scene->domain_list.at(0)->edge_param_map[Kernel::Direction::RIGHT].alpha, 0.7));

    }

    BOOST_AUTO_TEST_CASE(domain_dimensions) {
        auto domain = create_a_domain(-50, -25, 100, 150);
        BOOST_CHECK_EQUAL(domain->bottom_right.x, 50);
        BOOST_CHECK_EQUAL(domain->bottom_right.y, 125);
    }


    BOOST_AUTO_TEST_CASE(domain_pml_checks) {
        auto scene = create_a_scene();
        auto standard_domain = scene->domain_list.at(0);
        for (auto domain:scene->domain_list) {
            if (domain != standard_domain) {
                BOOST_CHECK(domain->is_pml);
            } else {
                BOOST_CHECK(!domain->is_pml);
            }
        }
    }

    BOOST_AUTO_TEST_CASE(domain_pml_checks_connected) {
        auto scene = create_a_scene();
        auto standard_domain = scene->domain_list.at(0);
        for (auto domain:scene->domain_list) {
            if (domain != standard_domain) {
                BOOST_CHECK(domain->is_pml);
                BOOST_CHECK(standard_domain->is_neighbour_of(domain));
            }

        }
    }

    BOOST_AUTO_TEST_CASE(domain_rho_arrays) {
        auto scene = create_a_scene();
        auto domain = scene->domain_list.at(0);
        cout << domain->rho_arrays[scene->domain_list.at(3)->id + domain->id + scene->domain_list.at(1)->id].pressure <<
        endl;
        BOOST_CHECK(true);
    }

    BOOST_AUTO_TEST_CASE(domain_clear_methods) {
        auto domain = create_a_domain(-50, -25, 100, 150);
        domain->clear_matrices();
        BOOST_CHECK(domain->current_values.p0.isZero());
        BOOST_CHECK(domain->current_values.u0.isZero());
        BOOST_CHECK(domain->current_values.w0.isZero());
        BOOST_CHECK(domain->l_values.Lpx.isZero());
        BOOST_CHECK(domain->l_values.Lpy.isZero());

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
        using namespace Kernel;
        auto domain = create_a_domain(-50, -25, 100, 150);
        BOOST_CHECK(domain->contains_point(Point(20, 20)));
        BOOST_CHECK(!domain->contains_point(Point(-60, 20)));
    }

    BOOST_AUTO_TEST_CASE(domain_test_calc) {
        //test the calc method // Todo: Louis
        BOOST_CHECK(true);
    }

    BOOST_AUTO_TEST_CASE(domain_get_vacant_range) {
        //Test the vacant range method
        BOOST_CHECK(true);
    }

BOOST_AUTO_TEST_SUITE_END()