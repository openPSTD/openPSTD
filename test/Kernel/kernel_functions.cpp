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
#include "../../kernel/core/kernel_functions.h"
#include <kernel/PSTDKernel.h>
#include <cmath>

using namespace OpenPSTD::Kernel;
using namespace std;
using namespace Eigen;
BOOST_AUTO_TEST_SUITE(kernel_functions)

    BOOST_AUTO_TEST_CASE(test_next_2_power) {
        BOOST_CHECK_EQUAL(next_2_power(42), 64);
        BOOST_CHECK_EQUAL(next_2_power(3.4), 4);
        BOOST_CHECK_EQUAL(next_2_power(0.1), 1);
    }

    BOOST_AUTO_TEST_CASE(test_rho_array_one_neighbour) {
        float air_dens = 1.2;
        float max_rho = 1E10;
        Array<float, 4, 2> velocity;
        Array<float, 4, 2> pressure;
        velocity << -1, 1, 0, 0, 2, 0, 1, 1;
        pressure << 1, -1, 0, 0, 2, 0, 1, 1;
        RhoArray rhoArray = get_rho_array(max_rho, air_dens, air_dens);
        BOOST_CHECK(rhoArray.pressure.isApprox(pressure));
        BOOST_CHECK(rhoArray.velocity.isApprox(velocity));
    }

    BOOST_AUTO_TEST_CASE(test_rho_array_two_neighbour) {
        float air_dens = 1.2;
        Array<float, 4, 2> velocity;
        Array<float, 4, 2> pressure;
        velocity << 0, 0, 0, 0, 1, 1, 1, 1;
        pressure << 0, 0, 0, 0, 1, 1, 1, 1;
        RhoArray rhoArray = get_rho_array(air_dens, air_dens, air_dens);
        BOOST_CHECK(rhoArray.pressure.isApprox(pressure));
        BOOST_CHECK(rhoArray.velocity.isApprox(velocity));
    }

    BOOST_AUTO_TEST_CASE(test_get_grid_spacing) {
        PSTDSettings settings;
        settings.SetSoundSpeed(340);
        settings.SetMaxFrequency(84000);
        BOOST_CHECK(get_grid_spacing(settings) <= 0.0020238);
        settings.SetSoundSpeed(340);
        settings.SetMaxFrequency(20000);
        BOOST_CHECK(get_grid_spacing(settings) <= 0.0085);
        settings.SetSoundSpeed(340);
        settings.SetMaxFrequency(200);
        BOOST_CHECK(get_grid_spacing(settings) <= 0.85);
        settings.SetSoundSpeed(340);
        settings.SetMaxFrequency(20);
        BOOST_CHECK(get_grid_spacing(settings) <= 8.5);
    }

    BOOST_AUTO_TEST_CASE(test_spatderp3) {
        //TODO
        Eigen::ArrayXXf d1, d2, d3;
        Eigen::ArrayXcf derfact;
        Eigen::ArrayXf real, imag;

        d1 << 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0;

        d2 << 1.61399787e-207,4.56817714e-207,6.46184092e-207,4.56817714e-207, 1.61399787e-207,2.84994399e-208,2.51502926e-209,1.10923382e-210, 2.44498678e-212,2.69341499e-214,1.48287102e-216,4.08015658e-219, 5.61078503e-222,3.85605995e-225,1.32445520e-228,2.27354793e-232, 1.95049258e-236,8.36291802e-241,1.79202772e-245,1.91913248e-250, 1.02716103e-255,2.74754908e-261,3.67304063e-267,2.45402709e-273, 8.19419550e-280,1.36743483e-286,1.14046064e-293,4.75365118e-301, 9.90256421e-309,1.03095893e-316,0.00000000e+000,0.00000000e+000, 0.00000000e+000,0.00000000e+000,0.00000000e+000,0.00000000e+000, 0.00000000e+000,0.00000000e+000,0.00000000e+000,0.00000000e+000, 0.00000000e+000,0.00000000e+000,0.00000000e+000,0.00000000e+000, 0.00000000e+000,0.00000000e+000,0.00000000e+000,0.00000000e+000, 0.00000000e+000,0.00000000e+000,
                1.59468819e-196,4.51352402e-196,6.38453224e-196,4.51352402e-196, 1.59468819e-196,2.81584761e-197,2.48493975e-198,1.09596308e-199, 2.41573525e-201,2.66119129e-203,1.46513013e-205,4.03134208e-208, 5.54365828e-211,3.80992651e-214,1.30860957e-217,2.24634747e-221, 1.92715712e-225,8.26286507e-230,1.77058812e-234,1.89617221e-239, 1.01487220e-244,2.71467774e-250,3.62909682e-256,2.42466741e-262, 8.09616114e-269,1.35107500e-275,1.12681630e-282,4.69677906e-290, 9.78409113e-298,1.01862468e-305,5.30006561e-314,1.38338381e-322, 0.00000000e+000,0.00000000e+000,0.00000000e+000,0.00000000e+000, 0.00000000e+000,0.00000000e+000,0.00000000e+000,0.00000000e+000, 0.00000000e+000,0.00000000e+000,0.00000000e+000,0.00000000e+000, 0.00000000e+000,0.00000000e+000,0.00000000e+000,0.00000000e+000, 0.00000000e+000,0.00000000e+000,
                7.87448115e-186,2.22875293e-185,3.15264633e-185,2.22875293e-185, 7.87448115e-186,1.39044981e-186,1.22704936e-187,5.41180444e-189, 1.19287656e-190,1.31408138e-192,7.23473067e-195,1.99065418e-197, 2.73742747e-200,1.88132041e-203,6.46184092e-207,1.10923382e-210, 9.51619414e-215,4.08015658e-219,8.74306514e-224,9.36319235e-229, 5.01138218e-234,1.34049269e-239,1.79202772e-245,1.19728721e-251, 3.99783911e-258,6.67153283e-265,5.56415591e-272,2.31924325e-279, 4.83132950e-287,5.02991172e-295,2.61714277e-303,6.80562186e-312, 8.84377506e-321,0.00000000e+000,0.00000000e+000,0.00000000e+000, 0.00000000e+000,0.00000000e+000,0.00000000e+000,0.00000000e+000, 0.00000000e+000,0.00000000e+000,0.00000000e+000,0.00000000e+000, 0.00000000e+000,0.00000000e+000,0.00000000e+000,0.00000000e+000, 0.00000000e+000,0.00000000e+000,
                1.94330722e-175,5.50023750e-175,7.78027181e-175,5.50023750e-175, 1.94330722e-175,3.43142754e-176,3.02817903e-177,1.33555449e-178, 2.94384556e-180,3.24296140e-182,1.78542612e-184,4.91264449e-187, 6.75557216e-190,4.64282468e-193,1.59468819e-196,2.73742747e-200, 2.34845807e-204,1.00692320e-208,2.15766109e-213,2.31069945e-218, 1.23673611e-223,3.30814065e-229,4.42246332e-235,2.95473040e-241, 9.86608445e-248,1.64643710e-254,1.37315261e-261,5.72355443e-269, 1.19230173e-276,1.24130893e-284,6.45872705e-293,1.67952832e-301, 2.18273533e-310,1.41772137e-319,0.00000000e+000,0.00000000e+000, 0.00000000e+000,0.00000000e+000,0.00000000e+000,0.00000000e+000, 0.00000000e+000,0.00000000e+000,0.00000000e+000,0.00000000e+000, 0.00000000e+000,0.00000000e+000,0.00000000e+000,0.00000000e+000, 0.00000000e+000,0.00000000e+000;

        d3 << 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0;

        //todo fill real and imag from kc (first python hit)
        derfact.real() = real;
        derfact.imag() = imag;

        Eigen::ArrayXXf spatresult = spatderp3(d1, d2, d3, derfact,
                                  RhoArray rho_array, Eigen::ArrayXf window, int wlen,
                                  CalculationType::PRESSURE, CalcDirection::X);
        BOOST_CHECK(true);
    }


BOOST_AUTO_TEST_SUITE_END()
