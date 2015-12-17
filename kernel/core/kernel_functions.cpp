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

    int next2Power(float n) {
        return (int) pow(2, ceil(log2(n)));
    }
    int next2Power(int n) {
        return (int) pow(2, ceil(log2(n)));
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
        fft_length = next2Power((int) p2->cols() + wlen*2);

        float *in_buffer;
        in_buffer = (float*) fftwf_malloc(sizeof(float)*fft_length*fft_batch);

        //if direct == 0, transpose p1, p2 and p3
        if(direct == CalcDirection::Y) {
            p1->transposeInPlace();
            p2->transposeInPlace();
            p3->transposeInPlace();
        }

        //the pressure is calculated for len(p2)+1, velocity for len(p2)-1
        //slicing and the values pulled from the Rmatrix is slightly different for the two branches
        if (ct == CalculationType::PRESSURE) {

            //window the outer domains, add a portion of the middle one to the sides and concatenate them all
            Eigen::ArrayXf window_left = window.head(wlen);
            Eigen::ArrayXf window_right = window.tail(wlen);

            if (wlen > p1->cols() || wlen > p3->cols()) {
                //TODO error if this happens and give user feedback.
            }
            Eigen::ArrayXXf dom1(fft_batch, wlen);
            Eigen::ArrayXXf dom3(fft_batch, wlen);
            Eigen::ArrayXXf windowed_data(fft_batch, fft_length);
            dom1 = p1->rightCols(wlen).rowwise()*window_left.transpose()*rho_array.pressure(2,1) +
                    p2->leftCols(wlen).rowwise().reverse()*rho_array.pressure(0,0);
            dom3 = p3->leftCols(wlen).rowwise()*window_right.transpose()*rho_array.pressure(3,1) +
                    p2->rightCols(wlen).rowwise().reverse()*rho_array.pressure(1,0);
            windowed_data << dom1,*p2, dom3;

            //TODO rewrite the C interfacing to acceptable C++
            int shape[] = {fft_length,1};
            fftwf_complex *out_buffer;
            out_buffer = (fftwf_complex *) fftwf_malloc(sizeof(fftwf_complex)*fft_length*fft_batch);
            int istride = fft_batch;
            int ostride = fft_batch;
            int idist = 1;
            int odist = 1;
            //TODO think of how these can be stored in the solver without creating serious spaghetti
            fftwf_plan plan = fftwf_plan_many_dft_r2c(fft_length, shape, fft_batch, in_buffer, NULL, istride, idist,
                                                      out_buffer, NULL, ostride, odist, FFTW_ESTIMATE);
            fftwf_plan plan_inv = fftwf_plan_many_dft_c2r(fft_length, shape, fft_batch, out_buffer, NULL, ostride, odist,
                                                      in_buffer, NULL, istride, idist, FFTW_ESTIMATE);

            memcpy(in_buffer, windowed_data.data(), sizeof(float)*fft_batch*fft_length);
            fftwf_execute_dft_r2c(plan, in_buffer, out_buffer);

            std::complex<float> spectrum_data;

            // this should be byte-compatible according to http://www.fftw.org/doc/Complex-numbers.html#Complex-numbers,
            // but it doesn't compile for some reason. (TODO)
            Eigen::Map<Eigen::ArrayXXcf> spectrum_array((std::complex<float>) out_buffer);

            //should still be on the same memory addresses, no need for any copying
            spectrum_array = spectrum_array.array().rowwise() * derfact->transpose();

            fftwf_execute_dft_c2r(plan, out_buffer, in_buffer);

            //TODO slice the result properly and put it back into an eigen array

        } else {
            //repeat for velocity calculation with different slicing
            //window the outer domains, add a portion of the middle one to the sides and concatenate them all
            Eigen::ArrayXf window_left = window.head(wlen);
            Eigen::ArrayXf window_right = window.tail(wlen);

            if (wlen > p1->cols() || wlen > p3->cols()) {
                //TODO error if this happens and give user feedback.
            }
            Eigen::ArrayXXf dom1(fft_batch, wlen);
            Eigen::ArrayXXf dom3(fft_batch, wlen);
            Eigen::ArrayXXf windowed_data(fft_batch, fft_length);
            dom1 = p1->rightCols(wlen+1).leftCols(wlen).rowwise()*window_left.transpose()*rho_array.pressure(2,1) +
                   p2->leftCols(wlen+1).rightCols(wlen).rowwise().reverse()*rho_array.pressure(0,0);
            dom3 = p3->leftCols(wlen+1).rightCols(wlen).rowwise()*window_right.transpose()*rho_array.pressure(3,1) +
                   p2->rightCols(wlen+1).leftCols(wlen).rowwise().reverse()*rho_array.pressure(1,0);
            windowed_data << dom1,*p2, dom3;

            //TODO rewrite the C interfacing to acceptable C++
            int shape[] = {fft_length,1};
            float *in_buffer;
            in_buffer = (float*) fftwf_malloc(sizeof(float)*fft_length*fft_batch);
            fftwf_complex *out_buffer;
            out_buffer = (fftwf_complex *) fftwf_malloc(sizeof(fftwf_complex)*fft_length*fft_batch);
            int istride = fft_batch;
            int ostride = fft_batch;
            int idist = 1;
            int odist = 1;
            //TODO think of how these can be stored in the solver without creating serious spaghetti
            fftwf_plan plan = fftwf_plan_many_dft_r2c(fft_length, shape, fft_batch, in_buffer, NULL, istride, idist,
                                                      out_buffer, NULL, ostride, odist, FFTW_ESTIMATE);
            fftwf_plan plan_inv = fftwf_plan_many_dft_c2r(fft_length, shape, fft_batch, out_buffer, NULL, ostride, odist,
                                                          in_buffer, NULL, istride, idist, FFTW_ESTIMATE);

            memcpy(in_buffer, windowed_data.data(), sizeof(float)*fft_batch*fft_length);
            fftwf_execute_dft_r2c(plan, in_buffer, out_buffer);

            std::complex<float> spectrum_data;

            // this should be byte-compatible according to http://www.fftw.org/doc/Complex-numbers.html#Complex-numbers,
            // but it doesn't compile for some reason. (TODO)
            Eigen::Map<Eigen::ArrayXXcf> spectrum_array((std::complex<float>) out_buffer);

            //should still be on the same memory addresses, no need for any copying
            spectrum_array = spectrum_array.array().rowwise() * derfact->transpose();

            fftwf_execute_dft_c2r(plan, out_buffer, in_buffer);
        }
        //TODO slice the result properly, transpose it if direct == 0

    }
}