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
#include "../../kernel/core/Domain.h"
#include <cmath>
#include <kernel/PSTDKernel.h>

using namespace OpenPSTD;
using namespace std;
using namespace Eigen;
BOOST_AUTO_TEST_SUITE(domain)

    shared_ptr<Kernel::Scene> create_a_scene() {
        shared_ptr<Kernel::PSTDConfiguration> config = Kernel::PSTDConfiguration::CreateDefaultConf();
        Kernel::DomainConf domain1;
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
        config->Speakers.clear();
        config->Speakers.push_back(QVector3D(4, 5, 0));
        BOOST_CHECK(config->Domains.size() == 1);
        Kernel::PSTDKernel kernel = Kernel::PSTDKernel();

        kernel.initialize_kernel(config);
        auto scene = kernel.get_scene();
        return scene;
    }

    shared_ptr<Kernel::Domain> create_a_domain(int point_x, int point_y, int size_x, int size_y) {
        using namespace Kernel;
        Point top_left(point_x, point_y);
        Point size(size_x, size_y);
        shared_ptr<WisdomCache> wnd(new WisdomCache());
        EdgeParameters standard = {};
        standard.locally_reacting = true;
        standard.alpha = 1;
        shared_ptr<PSTDSettings> settings(new PSTDSettings());
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
            cout << *domain << endl;
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
        shared_ptr<Kernel::Domain> left_domain = domain->get_neighbours_at(Kernel::Direction::LEFT).at(0);
        shared_ptr<Kernel::Domain> right_domain = domain->get_neighbours_at(Kernel::Direction::RIGHT).at(0);
        shared_ptr<Kernel::Domain> top_domain = domain->get_neighbours_at(Kernel::Direction::TOP).at(0);

        int index1 = domain->id * left_domain->id * right_domain->id;
        ArrayXXf correct_center_domain_velocity(4, 2);
        correct_center_domain_velocity << 0, 0, 0, 0, 1, 1, 1, 1;
        BOOST_CHECK(correct_center_domain_velocity.isApprox(domain->rho_arrays[index1].velocity));

        int index2 = domain->id * top_domain->id;
        ArrayXXf correct_top_pml_domain_pressure(4, 2);
        correct_top_pml_domain_pressure << 0, 0, 1, -1, 1, 1, 2, 0;
        BOOST_CHECK(correct_top_pml_domain_pressure.isApprox(top_domain->rho_arrays[index2].pressure));
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

    BOOST_AUTO_TEST_CASE(compute_pml_matrices) {
        auto domain = create_a_domain(0, 0, 50, 60);
        domain->compute_pml_matrices();
        BOOST_CHECK(true);
    }

    BOOST_AUTO_TEST_CASE(apply_pml_matrices) {
        auto domain = create_a_domain(0, 0, 50, 60);
        domain->compute_pml_matrices(); // Finish
        BOOST_CHECK(true);
    }

    BOOST_AUTO_TEST_CASE(domain_neighbours) {
        auto scene = create_a_scene();
        auto domain = scene->domain_list.at(0);
        BOOST_CHECK_EQUAL(domain->number_of_neighbours(false), 0);
        BOOST_CHECK_EQUAL(domain->number_of_neighbours(true), 4);
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
        auto scene = create_a_scene();
        auto domain = scene->domain_list.at(0);
        BOOST_CHECK(true);
    }

    BOOST_AUTO_TEST_CASE(test_initial_values) {
        auto scene = create_a_scene();
        auto domain = scene->domain_list.at(0);
        vector<float> speaker_loc = scene->speaker_list.at(0)->location;
        // Speakers at location (20,25)
        // Domain is (250,300)
        // Bandwidth is 21 491;

        // Check center to be 1
        BOOST_CHECK_EQUAL(domain->current_values.p0(20, 25), 1);
        // Check symmetry
        BOOST_CHECK_EQUAL(domain->current_values.p0(20, 24), domain->current_values.p0(21, 25));
        // Check next grid point to be 0.71 (analytically confirmed value)
        BOOST_CHECK(Kernel::is_approx(domain->current_values.p0(20, 24), 0.706947));

    }

BOOST_AUTO_TEST_SUITE_END()