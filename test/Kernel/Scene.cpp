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
#include "../../kernel/core/Scene.h"
#include <cmath>
#include <kernel/core/kernel_functions.h>
#include <kernel/PSTDKernel.h>

using namespace OpenPSTD;
using namespace std;
using namespace Eigen;

BOOST_AUTO_TEST_SUITE(scene)

    shared_ptr<Kernel::Scene> create_a_reflecting_scene(int pml_size) {
        shared_ptr<Kernel::PSTDConfiguration> config = Kernel::PSTDConfiguration::CreateDefaultConf();
        Kernel::DomainConf domain1;
        domain1.TopLeft = QVector2D(0, 0);
        domain1.Size = QVector2D(40, 60);
        // Partly absorbing walls: makes for 4 primary and 4 secondary pmls
        domain1.T.Absorption = 0.1;
        domain1.B.Absorption = 0.1;
        domain1.L.Absorption = 0.1;
        domain1.R.Absorption = 0.1;
        domain1.T.LR = false;
        domain1.B.LR = false;
        domain1.L.LR = false;
        domain1.R.LR = false;
        config->Settings.SetPMLCells(pml_size);
        config->Domains.clear();
        config->Domains.push_back(domain1);
        config->Speakers.clear();
        config->Speakers.push_back(QVector3D(4.1, 5.1, 0));
        BOOST_CHECK(config->Domains.size() == 1);
        Kernel::PSTDKernel kernel = Kernel::PSTDKernel(false, false);
        kernel.initialize_kernel(config, std::make_shared<OpenPSTD::Kernel::KernelCallbackLog>());
        auto scene = kernel.get_scene();
        return scene;
    }

    BOOST_AUTO_TEST_CASE(creation_of_pmls) {
        auto scene = create_a_reflecting_scene(50);
        // Domain + 4 pml domains for each direction + 4 secondary pmls for absorption
        BOOST_CHECK_EQUAL(scene->domain_list.size(), 1 + 4 + 4);

    }


    BOOST_AUTO_TEST_CASE(creation_of_sec_pmls) {
        int pml_size = 50;
        auto scene = create_a_reflecting_scene(pml_size);
        int num_pmls = 0;
        int num_sec_pmls = 0;
        for (unsigned long i = 0; i < scene->domain_list.size(); i++) {
            auto domain = scene->domain_list.at(i);
            if (domain->is_pml) {
                num_pmls++;
                if (domain->is_secondary_pml) {
                    num_sec_pmls++;
                }
            }
        }
        BOOST_CHECK_EQUAL(num_pmls, 4 + 4);
        BOOST_CHECK_EQUAL(num_sec_pmls, 4);
    }

    BOOST_AUTO_TEST_CASE(size_of_pml) {
        int pml_size = 50;
        auto scene = create_a_reflecting_scene(pml_size);
        for (unsigned long i = 0; i < scene->domain_list.size(); i++) {
            auto domain = scene->domain_list.at(i);
            if (domain->is_pml) {
                BOOST_CHECK(domain->size.x == pml_size or domain->size.y == pml_size);
            }

        }
    }

    BOOST_AUTO_TEST_CASE(size_of_sec_pml) {
        int pml_size = 50;
        auto scene = create_a_reflecting_scene(pml_size);
        for (unsigned long i = 0; i < scene->domain_list.size(); i++) {
            auto domain = scene->domain_list.at(i);
            if (domain->is_secondary_pml) {
                BOOST_CHECK(domain->size.x == pml_size and domain->size.y == pml_size);
            }

        }
    }


BOOST_AUTO_TEST_SUITE_END()