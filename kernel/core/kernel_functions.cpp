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
#include "PSTDFile.h"

using namespace std;
using namespace Eigen;

namespace Kernel {
    //Todo: Refactor: remove 'using namespace' and adapt filename to coding style
    rMatrices1D getRMatrices1D(const double rho1, const double rho2, const double rho) {
        // 0mar: I switched the order of parameters in this function. Remember when porting classes.py.
        double zn1 = rho1 / rho;
        double inv_zn1 = rho / rho1;
        double rlw1 = (zn1 - 1.) / (zn1 + 1);
        double rlw2 = (inv_zn1 - 1) / (inv_zn1 + 1);
        double tlw1 = (2 * zn1) / (zn1 + 1);
        double tlw2 = (2 * inv_zn1) / (inv_zn1 + 1);

        double zn2 = rho2 / rho;
        double inv_zn2 = rho / rho2;
        double rrw1 = (zn2 - 1) / (zn2 + 1);
        double rrw2 = (inv_zn2 - 1) / (inv_zn2 + 1);
        double trw1 = (2 * zn2) / (zn2 + 1);
        double trw2 = (2 * inv_zn2) / (inv_zn2 + 1);
        rMatrices1D result;
        result.pressure << rlw1, rlw2, rrw1, rrw2,
                tlw1, tlw2, trw1, trw2;
        result.velocity << -rlw1, -rlw2, -rrw1, -rrw2,
                tlw1, tlw2, trw1, trw2;
        return result;
    }


    rMatrices2D getRMatrices2D(const double rhoLeft, const double rhoRight, const double rhoLower,
                               const double rhoUpper,
                               const double rho) {

        // Todo (0mar): Assignments have structure; put them into a for-loop
        // Todo (0mar): Merge getRMatrices method to 1 generic method for all 3 dimensional options?
        // Todo (0mar): This method is not used, remove.
        double znLeft = rhoLeft / rho;
        double inv_znLeft = rho / rhoLeft;
        double rlw1 = (znLeft - 1) / (znLeft + 1);
        double rlw2 = (inv_znLeft - 1) / (inv_znLeft - 1);
        double tlw1 = (2 * znLeft) / (znLeft + 1);
        double tlw2 = (2 * inv_znLeft) / (inv_znLeft + 1);

        double znRight = rhoRight / rho;
        double inv_znRight = rho / rhoRight;
        double rrw1 = (znRight - 1) / (znRight + 1);
        double rrw2 = (inv_znRight - 1) / (inv_znRight + 1);
        double trw1 = (2 * znRight) / (znRight + 1);
        double trw2 = (2 * inv_znRight) / (inv_znRight + 1);

        double znLower = rhoLower / rho;
        double inv_znLower = rho / rhoLower;
        double rlw3 = (znLower - 1) / (znLower + 1);
        double rlw4 = (inv_znLower - 1) / (inv_znLower + 1);
        double tlw3 = (2 * znLower) / (znLower + 1);
        double tlw4 = (2 * inv_znLower) / (inv_znLower + 1);

        double znUpper = rhoUpper / rho;
        double inv_znUpper = rho / rhoUpper;
        double rrw3 = (znUpper - 1) / (znUpper + 1);
        double rrw4 = (inv_znUpper - 1) / (inv_znUpper + 1);
        double trw3 = (2 * znUpper) / (znUpper + 1);
        double trw4 = (2 * inv_znUpper) / (inv_znUpper + 1);

        rMatrices2D result;
        result.pressure << rlw1, rlw2, rlw3, rlw4,
                rrw1, rrw2, rrw3, rrw4,
                tlw1, tlw2, tlw3, tlw4,
                trw1, trw2, trw3, trw4;

        result.velocity << -rlw1, -rlw2, -rlw3, -rlw4,
                -rrw1, -rrw2, -rrw3, -rrw4,
                tlw2, tlw1, tlw4, tlw3,
                trw2, trw1, trw4, trw3;
        return result;
    }

    int next2Power(double n) {

        return (int) pow(2, ceil(log2(n)));

    }

    double getGridSpacing(PSTDFileSettings cnf) {
        Array<double, 9, 1> dxv;
        dxv << 0.002, 0.005, 0.01, 0.02, 0.05, 0.1, 0.2, 0.5, 1; //Louis: is there a good reason to disallow other vals?
        double waveLength = 0.5 * cnf.GetSoundSpeed() / cnf.GetMaxFrequency(); // Omar: This is the wavelength, right?
        // Todo (0mar): Throw a ValueError when waveLength does not exceed 2e-3
        for (int i = 0; i < dxv.size(); i++) {
            if (dxv[i] >= waveLength) {
                return i - 1;
            }
        }
        return dxv.size() - 1;
    }

    tuple<vector<double>, vector<double>> PML(PSTDFileSettings cnf) {
        vector<double> cell_list_p = arange<double>(0.5, cnf.GetPMLCells() + 0.5, 1);
        for (int i = 0; i < cell_list_p.size(); i++) {
            cell_list_p[i] = cnf.GetAttenuationOfPMLCells() * pow(cell_list_p[i] / cnf.GetPMLCells(), 4);
        }
        vector<double> cell_list_u = arange<double>(0, cnf.GetPMLCells() + 1, 1);
        for (int i = 0; i < cell_list_u.size(); i++) {
            cell_list_u[i] = cnf.GetAttenuationOfPMLCells() * pow(cell_list_u[i] / cnf.GetPMLCells(), 4);
            cell_list_u[i] = cnf.GetDensityOfAir() * cell_list_u[i];
        }
        return make_tuple(cell_list_p, cell_list_u);
    }

}