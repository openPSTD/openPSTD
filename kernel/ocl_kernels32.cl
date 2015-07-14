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
// File: ocl_kernels32.cl
// Author: Louis van Harten
// Purpose:
//     Contains 32-bit version of the OpenCL kernels used in the GPU
//     acceleration of the spatial derivatives needed in the PSTD method.
//
//////////////////////////////////////////////////////////////////////////

__kernel void derifact_multiplication(__global float *matr, __global float *mati,
                                      __global float *vecr, __global float *veci, const int fftlen,
                                      const int fftnum) {
    int index_x = get_global_id(0) * get_global_size(0) + get_local_id(0);
    int index_y = get_global_id(1) * get_global_size(1) + get_local_id(1);

    int matindex = index_y * fftlen + index_x; // mat should be a contiguous array
    // if N1%16>0, we're starting too many threads.
    // There is probably a better way to do this, but just eating the surplus
    // should work.
    if (matindex < fftlen * fftnum) {
        float matreal = matr[matindex];
        float matimag = mati[matindex];
        float vecreal = vecr[index_x];
        float vecimag = veci[index_x];

        matr[matindex] = matreal * vecreal - matimag * vecimag;
        mati[matindex] = matreal * vecimag + matimag * vecreal;
    }
}

__kernel void pressure_window_multiplication(__global float *mr, __global float *mi,
                                             __global float *A, __global float *p1,
                                             __global float *p2, __global float *p3,
                                             const int winlen, const int Ns1, const int Ns2,
                                             const int Ns3, const int fftlen, const int fftnum,
                                             const float R21, const float R00, const float R31,
                                             const float R10) {
    int index_x = get_global_id(0) * get_global_size(0) + get_local_id(0);
    int index_y = get_global_id(1) * get_global_size(1) + get_local_id(1);

    if (index_y < fftnum) { // eat the surplus
        int matindex = index_y * fftlen + index_x;

        float G = 1;
        if (index_x < winlen) {
            G = A[index_x];
        } else if (index_x > winlen + Ns2 - 1 && index_x < winlen * 2 + Ns2) {
            G = A[index_x - Ns2];
        }
        mi[matindex] = 0;
        if (index_x < winlen) {
            mr[matindex] = G * (R21 * p1[Ns1 * index_y + index_x - winlen + Ns1] +
                                R00 * p2[Ns2 * index_y + winlen - 1 - index_x]);
        } else if (index_x < winlen + Ns2) {
            mr[matindex] = p2[Ns2 * index_y + index_x - winlen];
        } else if (index_x < winlen * 2 + Ns2) {
            mr[matindex] = G * (R31 * p3[Ns3 * index_y + index_x - winlen - Ns2] +
                                R10 * p2[Ns2 * index_y + 2 * Ns2 + winlen - 1 - index_x]);
        } else {
            mr[matindex] = 0; // zero padding
        }
    }
}

__kernel void velocity_window_multiplication(__global float *mr, __global float *mi,
                                             __global float *A, __global float *p1,
                                             __global float *p2, __global float *p3,
                                             const int winlen, const int Ns1, const int Ns2,
                                             const int Ns3, const int fftlen, const int fftnum,
                                             const float R21, const float R00, const float R31,
                                             const float R10) {
    int index_x = get_global_id(0) * get_global_size(0) + get_local_id(0);
    int index_y = get_global_id(1) * get_global_size(1) + get_local_id(1);

    if (index_y < fftnum) { // eat the surplus
        int matindex = index_y * fftlen + index_x;

        float G = 1;
        if (index_x < winlen) {
            G = A[index_x];
        } else if (index_x > winlen + Ns2 - 1 && index_x < winlen * 2 + Ns2) {
            G = A[index_x - Ns2];
        }
        mi[matindex] = 0;
        if (index_x < winlen) {
            mr[matindex] = G * (R21 * p1[Ns1 * index_y + index_x - winlen + Ns1 - 1] +
                                R00 * p2[Ns2 * index_y + winlen - index_x]);
        } else if (index_x < winlen + Ns2) {
            mr[matindex] = p2[Ns2 * index_y + index_x - winlen];
        } else if (index_x < winlen * 2 + Ns2) {
            mr[matindex] = G * (R31 * p3[Ns3 * index_y + index_x - winlen - Ns2 + 1] +
                                R10 * p2[Ns2 * index_y + 2 * Ns2 + winlen - 2 - index_x]);
        } else {
            mr[matindex] = 0; // zero padding
        }
    }
}
