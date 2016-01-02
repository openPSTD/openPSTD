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

namespace Kernel {
    RhoArray get_rho_array(const float rho1, const float rho_self, const float rho2) {
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

    int next_2_power(float n) {
        return std::max((int) pow(2, ceil(log2(n))), 1);
    }

    int next_2_power(int n) {
        return std::max((int) pow(2, ceil(log2(n))), 1);
    }

    float get_grid_spacing(PSTDFileSettings cnf) {
        Eigen::Array<float, 9, 1> dxv;
        dxv << 0.002, 0.005, 0.01, 0.02, 0.05, 0.1, 0.2, 0.5, 1; //TODO: is there a good reason to disallow other vals?
        float waveLength = cnf.GetSoundSpeed() / cnf.GetMaxFrequency() / 2;
        if (waveLength < 0.002) {
            throw std::invalid_argument("Wavelength (speed/frequency) is too small");
        }
        for (int i = 0; i < dxv.size(); i++) {
            if (dxv(i) >= waveLength) {
                return dxv(i - 1);
            }
        }
        return dxv(dxv.size() - 1);
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

    CalcDirection get_orthogonal(CalcDirection direction) {
        switch (direction) {
            case CalcDirection::X:
                return CalcDirection::Y;
            case CalcDirection::Y:
                return CalcDirection::X;
        }
    }

    CalcDirection direction_to_calc_direction(Direction direction) {
        switch (direction) {
            case Direction::LEFT:
            case Direction::RIGHT:
                return CalcDirection::X;
            case Direction::TOP:
            case Direction::BOTTOM:
                return CalcDirection::Y;
        }
    }

    Eigen::ArrayXXf spatderp3(std::shared_ptr<Eigen::ArrayXXf> p1, std::shared_ptr<Eigen::ArrayXXf> p2,
                              std::shared_ptr<Eigen::ArrayXXf> p3, std::shared_ptr<Eigen::ArrayXcf> derfact,
                              RhoArray rho_array, Eigen::ArrayXf window, int wlen,
                              CalculationType ct, CalcDirection direct) {

        //in the Python code: N1 = fft_batch and N2 = fft_length
        int fft_batch, fft_length;

        fft_batch = p2->rows();
        fft_length = next_2_power((int) p2->cols() + wlen * 2);

        //if direct == 0, transpose p1, p2 and p3
        if(direct == CalcDirection::Y) {
            p1->transposeInPlace();
            p2->transposeInPlace();
            p3->transposeInPlace();
        }

        //the pressure is calculated for len(p2)+1, velocity for len(p2)-1
        //slicing and the values pulled from the Rmatrix is slightly different for the two branches
        if (ct == CalculationType::PRESSURE) {
            //window the outer domains and concatenate them all
            Eigen::ArrayXf window_left = window.head(wlen);
            Eigen::ArrayXf window_right = window.tail(wlen);

            if (wlen > p1->cols() || wlen > p3->cols()) {
                //TODO error if this happens and give user feedback.
            }
            Eigen::ArrayXXf G1(fft_batch, wlen);
            Eigen::ArrayXXf G2 = *p2;
            Eigen::ArrayXXf G3(fft_batch, wlen);
            Eigen::ArrayXXf G(fft_batch, fft_length);
            G1 = p1->rightCols(wlen).rowwise() * window_left.transpose();
            G3 = p3->leftCols(wlen).rowwise() * window_right.transpose();
            G << G1, G2, G3;


            //set catemp_fft = fft(catemp) with fft length $fft_length. fft one dimensional, applied to every row of catemp.
            //the fft should use the same scaling as numpy.fft (that is, no scaling in the fft and 1/n scaling in the ifft,
            //with the fft defined as $A_k=sum_{m=0}^{n-1} e^{2*\pi i*f*m*\delta*t} * e^{-2*\pi i*m*k/n}, k=0,...,n-1$ and
            //the ifft defined as $a_m=1/n*sum_{k=0}^{n-1} A_k * e^{-2*\pi i*m*k/n}, m=0,...,n-1$


            //set catemp_fft_der to catemp_fft * derfact ((!) complex element-wise multiplication)


            //set Ltemp = real(ifft(catemp_fft_der))

        } else {
            //repeat for velocity calculation

        }
        //slice the result properly, transpose it if direct == 0


    }
}