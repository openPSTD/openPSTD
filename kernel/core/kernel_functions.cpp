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
namespace OpenPSTD
{
    namespace Kernel
    {
        RhoArray get_rho_array(const float rho1, const float rho_self, const float rho2)
        {
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
            RhoArray result = {};
            result.pressure = Eigen::ArrayXXf(4, 2);
            result.velocity = Eigen::ArrayXXf(4, 2);;
            result.pressure << rlw1, rlw2, rrw1, rrw2,
                    tlw1, tlw2, trw1, trw2;
            result.velocity << -rlw1, -rlw2, -rrw1, -rrw2,
                    tlw1, tlw2, trw1, trw2;
            return result;
        }

        int next_2_power(float n)
        {
            return std::max((int) pow(2, ceil(log2(n))), 1);
        }

        int next_2_power(int n)
        {
            return std::max((int) pow(2, ceil(log2(n))), 1);
        }

        float get_grid_spacing(PSTDSettings cnf)
        {
            Eigen::Array<float, 9, 1> dxv;
            dxv <<
            0.002, 0.005, 0.01, 0.02, 0.05, 0.1, 0.2, 0.5, 1; //TODO: is there a good reason to disallow other vals?
            float waveLength = cnf.GetSoundSpeed() / cnf.GetMaxFrequency() / 2;
            if (waveLength < 0.002)
            {
                throw std::invalid_argument("Wavelength (speed/frequency) is too small");
            }
            for (int i = 0; i < dxv.size(); i++)
            {
                if (dxv(i) >= waveLength)
                {
                    return dxv(i - 1);
                }
            }
            return dxv(dxv.size() - 1);
        }

        Direction get_opposite(Direction direction)
        {
            switch (direction)
            {
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

        CalcDirection get_orthogonal(CalcDirection direction)
        {
            switch (direction)
            {
                case CalcDirection::X:
                    return CalcDirection::Y;
                case CalcDirection::Y:
                    return CalcDirection::X;
            }
        }

        CalcDirection direction_to_calc_direction(Direction direction)
        {
            switch (direction)
            {
                case Direction::LEFT:
                case Direction::RIGHT:
                    return CalcDirection::X;
                case Direction::TOP:
                case Direction::BOTTOM:
                    return CalcDirection::Y;
            }
        }

        Eigen::ArrayXXf spatderp3(Eigen::ArrayXXf p1, Eigen::ArrayXXf p2,
                                  Eigen::ArrayXXf p3, Eigen::ArrayXcf derfact,
                                  RhoArray rho_array, Eigen::ArrayXf window, int wlen,
                                  CalculationType ct, CalcDirection direct,
                                  fftwf_plan plan, fftwf_plan plan_inv)
        {

            //in the Python code: N1 = fft_batch and N2 = fft_length
            int fft_batch, fft_length;
            Eigen::ArrayXXf result(1, 1); //also called Lp in some places in documentation

            fft_batch = p2.rows();
            fft_length = next_2_power((int) p2.cols() + wlen * 2);

            float *in_buffer;
            in_buffer = (float *) fftwf_malloc(sizeof(float) * fft_length * fft_batch);

            fftwf_complex *out_buffer;
            out_buffer = (fftwf_complex *) fftwf_malloc(sizeof(float) * 2 * (fft_length / 2 + 1) * fft_batch);

            //non-domains don't have a wisdomcache, so this is needed. TODO Perhaps put it in the Scene itself.
            if(plan == NULL) {
                int shape[] = {fft_length};
                int istride = 1; //distance between two elements in one fft-able array
                int ostride = istride;
                int idist = fft_length; //distance between first element of different arrays
                int odist = idist;
                plan = fftwf_plan_many_dft_r2c(1, shape, fft_batch, in_buffer, NULL, istride, idist,
                                                          out_buffer, NULL, ostride, odist, FFTW_ESTIMATE);

                idist = (fft_length / 2) + 1;
                odist = idist;
                int ishape[] = {fft_length / 2 + 1};
                plan_inv = fftwf_plan_many_dft_c2r(1, ishape, fft_batch, out_buffer, NULL, ostride, odist,
                                                              in_buffer, NULL, istride, idist, FFTW_ESTIMATE);
            }

            //if direct == Y, transpose p1, p2 and p3
            if (direct == CalcDirection::Y)
            {
                p1.transposeInPlace();
                p2.transposeInPlace();
                p3.transposeInPlace();
            }

            //the pressure is calculated for len(p2)+1, velocity for len(p2)-1
            //slicing and the values pulled from the Rmatrix is slightly different for the two branches
            if (ct == CalculationType::PRESSURE)
            {
                result.resize(fft_batch, p2.cols() + wlen * 2 + 1);

                //window the outer domains, add a portion of the middle one to the sides and concatenate them all
                Eigen::ArrayXf window_left = window.head(wlen);
                Eigen::ArrayXf window_right = window.tail(wlen);

                if (wlen > p1.cols() || wlen > p3.cols())
                {
                    //TODO error (or just warn) if this happens and give user feedback.
                }

                Eigen::ArrayXXf dom1(fft_batch, wlen);
                Eigen::ArrayXXf dom3(fft_batch, wlen);
                Eigen::ArrayXXf windowed_data(fft_batch, fft_length);
                //this looks inefficient, but Eigen should optimize it into single operations (TODO: check if it does)
                dom1 = p1.rightCols(wlen).rowwise() * window_left.transpose() * rho_array.pressure(2, 1) +
                       p2.leftCols(wlen).rowwise().reverse() * rho_array.pressure(0, 0);
                dom3 = p3.leftCols(wlen).rowwise() * window_right.transpose() * rho_array.pressure(3, 1) +
                       p2.rightCols(wlen).rowwise().reverse() * rho_array.pressure(1, 0);
                windowed_data << dom1, p2, dom3;


                //perform the fft
                memcpy(in_buffer, windowed_data.data(), sizeof(float) * fft_batch * fft_length);
                fftwf_execute_dft_r2c(plan, in_buffer, out_buffer);

                //map the results back into an eigen array
                std::vector<std::complex<float>> spectrum_data;
                for (int i = 0; i < (fft_length / 2 + 1) * fft_batch; i++)
                { //TODO this looks wasteful/slow. Check if it is
                    spectrum_data.push_back(std::complex<float>(out_buffer[i][0], out_buffer[i][1]));
                }
                Eigen::Map<Eigen::ArrayXXcf> spectrum_array(&spectrum_data[0], fft_batch, fft_length);

                //apply the spectral derivative
                spectrum_array = spectrum_array.array().rowwise() * derfact.transpose();
                std::complex<float> *spectrum_prep;
                Eigen::Map<Eigen::ArrayXXcf>(spectrum_prep, fft_batch, fft_length / 2 + 1) = spectrum_array;
                fftwf_execute_dft_c2r(plan, reinterpret_cast<fftwf_complex *>(&spectrum_prep[0]), in_buffer);

                Eigen::ArrayXXf derived_array = Eigen::Map<Eigen::ArrayXXf>(in_buffer, fft_batch, fft_length).array();

                //ifft result contains the outer domains, so slice
                result = derived_array.leftCols(wlen + p2.cols() + 1).rightCols(p2.cols() + 1);

            }
            else
            {
                //repeat for velocity calculation with different slicing

                result.resize(fft_batch, p2.cols() + wlen * 2 - 1);

                //window the outer domains, add a portion of the middle one to the sides and concatenate them all
                Eigen::ArrayXf window_left = window.head(wlen);
                Eigen::ArrayXf window_right = window.tail(wlen);

                if (wlen > p1.cols() || wlen > p3.cols())
                {
                    //TODO error (or just warn) if this happens and give user feedback.
                }
                Eigen::ArrayXXf dom1(fft_batch, wlen);
                Eigen::ArrayXXf dom3(fft_batch, wlen);
                Eigen::ArrayXXf windowed_data(fft_batch, fft_length);
                dom1 = p1.rightCols(wlen + 1).leftCols(wlen).rowwise() * window_left.transpose() *
                       rho_array.pressure(2, 1) +
                       p2.leftCols(wlen + 1).rightCols(wlen).rowwise().reverse() * rho_array.pressure(0, 0);
                dom3 = p3.leftCols(wlen + 1).rightCols(wlen).rowwise() * window_right.transpose() *
                       rho_array.pressure(3, 1) +
                       p2.rightCols(wlen + 1).leftCols(wlen).rowwise().reverse() * rho_array.pressure(1, 0);
                windowed_data << dom1, p2, dom3;

                int shape[] = {fft_length};
                int istride = 1; //distance between two elements in one fft-able array
                int ostride = istride;
                int idist = fft_length; //distance between first element of different arrays
                int odist = idist;
                //TODO think of how these can be stored in the solver without creating serious spaghetti
                fftwf_plan plan = fftwf_plan_many_dft_r2c(1, shape, fft_batch, in_buffer, NULL, istride, idist,
                                                          out_buffer, NULL, ostride, odist, FFTW_ESTIMATE);

                idist = (fft_length / 2) + 1;
                odist = idist;
                int ishape[] = {fft_length / 2 + 1};
                fftwf_plan plan_inv = fftwf_plan_many_dft_c2r(1, ishape, fft_batch, out_buffer, NULL, ostride, odist,
                                                              in_buffer, NULL, istride, idist, FFTW_ESTIMATE);

                //perform the fft
                memcpy(in_buffer, windowed_data.data(), sizeof(float) * fft_batch * fft_length);
                fftwf_execute_dft_r2c(plan, in_buffer, out_buffer);

                //map the results back into an eigen array
                std::vector<std::complex<float>> spectrum_data;
                for (int i = 0; i < (fft_length / 2 + 1) * fft_batch; i++)
                {
                    spectrum_data.push_back(std::complex<float>(out_buffer[i][0], out_buffer[i][1]));
                }
                Eigen::Map<Eigen::ArrayXXcf> spectrum_array(&spectrum_data[0], fft_batch, fft_length);

                //apply the spectral derivative
                spectrum_array = spectrum_array.array().rowwise() * derfact.transpose();
                std::complex<float> *spectrum_prep;
                Eigen::Map<Eigen::ArrayXXcf>(spectrum_prep, fft_batch, fft_length / 2 + 1) = spectrum_array;
                fftwf_execute_dft_c2r(plan, reinterpret_cast<fftwf_complex *>(&spectrum_prep[0]), in_buffer);

                Eigen::ArrayXXf derived_array = Eigen::Map<Eigen::ArrayXXf>(in_buffer, fft_batch, fft_length).array();

                //ifft result contains the outer domains, so slice
                result = derived_array.leftCols(wlen + p2.cols() - 1).rightCols(p2.cols() - 1);

            }
            if (direct == CalcDirection::Y)
            {
                result.transposeInPlace();
            }
            return result;
        }

        Eigen::ArrayXXf spatderp3(Eigen::ArrayXXf p1, Eigen::ArrayXXf p2,
                                  Eigen::ArrayXXf p3, Eigen::ArrayXcf derfact,
                                  RhoArray rho_array, Eigen::ArrayXf window, int wlen,
                                  CalculationType ct, CalcDirection direct)
        {
            return spatderp3(p1, p2, p3, derfact, rho_array, window, wlen, ct, direct, NULL, NULL);
        }


        void debug(std::string msg)
        {
#if 1
            std::cout << msg << std::endl;
#endif
        }

        bool is_approx(float a, float b)
        {
            if (a == 0)
            {
                return fabs(b) < EPSILON;
            }
            else
            {
                return (fabs((a - b) / a) < EPSILON);

            }

        }
    }
}
