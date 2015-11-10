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
// Date:
//      1-9-2015
//
// Authors:
//      Omar Richardson
//      Louis van Harten
//
//////////////////////////////////////////////////////////////////////////

#include "kernel_functions.h"
#include <stdexcept>

namespace Kernel {
    RhoArray get_rho_array(const float rho1, const float rho_self, const float rho2) {
        // 0mar: I switched the order of parameters in this function. Remember when porting classes.py.
        float zn1 = rho1 / rho_self;
        float inv_zn1 = rho_self / rho1;
        float rlw1 = (zn1 - 1) / (zn1 + 1);
        float rlw2 = (inv_zn1 - 1) / (inv_zn1 + 1);
        float tlw1 = (2 * zn1) / (zn1 + 1);
        float tlw2 = (2 * inv_zn1) / (inv_zn1 + 1);

        float zn2 = rho2 / rho_self;
        float inv_zn2 = rho_self / rho2;
        float rrw1 = (zn2 - 1) / (zn2 + 1);
        float rrw2 = (inv_zn2 - 1) / (inv_zn2 + 1);
        float trw1 = (2 * zn2) / (zn2 + 1);
        float trw2 = (2 * inv_zn2) / (inv_zn2 + 1);
        RhoArray result;
        result.pressure << rlw1, rlw2, rrw1, rrw2,
                tlw1, tlw2, trw1, trw2;
        result.velocity << -rlw1, -rlw2, -rrw1, -rrw2,
                tlw1, tlw2, trw1, trw2;
        return result;
    }

    int next2Power(float n) {

        return (int) pow(2, ceil(log2(n)));

    }

    float getGridSpacing(PSTDFileSettings cnf) {
        Eigen::Array<float, 9, 1> dxv;
        dxv << 0.002, 0.005, 0.01, 0.02, 0.05, 0.1, 0.2, 0.5, 1; //Louis: is there a good reason to disallow other vals?
        float waveLength = cnf.GetSoundSpeed() / cnf.GetMaxFrequency() / 2;
        if (waveLength < 0.002) {
            throw std::invalid_argument("Wavelength (speed/frequency) is too small");
        }
        for (int i = 0; i < dxv.size(); i++) {
            if (dxv[i] >= waveLength) {
                return i - 1;
            }
        }
        return dxv.size() - 1;
    }

    Direction get_opposite(Direction direction) {
        switch (direction) {
            case Direction::TOP:
                return Direction::BOTTOM;
            case Direction::BOTTOM:
                return Direction::TOP;
            case Direction::LEFT:
                return Direction::RIGHT;
            case Direction::RIGHT:
                return Direction::LEFT;
        }

    }

//    std::tuple<std::vector<float>, std::vector<float>> PML(PSTDFileSettings cnf) {
//        std::vector<float> cell_list_p = arange<float>(0.5, cnf.GetPMLCells() + 0.5f, 1);
//        for (int i = 0; i < cell_list_p.size(); i++) {
//            cell_list_p[i] = (float) (cnf.GetAttenuationOfPMLCells() * pow(cell_list_p[i] / cnf.GetPMLCells(), 4));
//        }
//        std::vector<float> cell_list_u = arange<float>(0, cnf.GetPMLCells() + 1, 1);
//        for (int i = 0; i < cell_list_u.size(); i++) {
//            cell_list_u[i] = (float) (cnf.GetAttenuationOfPMLCells() * pow(cell_list_u[i] / cnf.GetPMLCells(), 4));
//            cell_list_u[i] = cnf.GetDensityOfAir() * cell_list_u[i];
//        }
//        return make_tuple(cell_list_p, cell_list_u);
//    }

    Eigen::ArrayXXf spatderp3(std::shared_ptr<Eigen::ArrayXXf> p2, std::shared_ptr<Eigen::ArrayXcf> derfact,
                              int Wlength, int N1, int N2, Eigen::Matrix<float, 1, 4> Rmatrix,
                              std::shared_ptr<Eigen::ArrayXXf> p1,
                              std::shared_ptr<Eigen::ArrayXXf> p3, int var, int direct) {

    }
}