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
#include "../core/Scene.h"
#include <cmath>
#include <kernel/core/kernel_functions.h>

using namespace Kernel;
using namespace std;
using namespace Eigen;

BOOST_AUTO_TEST_SUITE(wave_numbers)

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

    BOOST_AUTO_TEST_CASE(scene_test_prime_numbers) {
        BOOST_CHECK(!domain->is_pml);
    }


    BOOST_AUTO_TEST_CASE(scene_test_scene_dimensions) {
        BOOST_CHECK(true);
    }

    BOOST_AUTO_TEST_CASE(connected_domains) {
        BOOST_CHECK(true);
    }


BOOST_AUTO_TEST_SUITE_END()