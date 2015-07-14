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
// File: cuda_kernels.cu
// Author: Louis van Harten
// Purpose:
//     Contains 64-bit version of the Cuda kernels used in the GPU
//     acceleration of the spatial derivatives needed in the PSTD method.
//
//////////////////////////////////////////////////////////////////////////

__global__ void derifact_multiplication(double *matr, double *mati, double *vecr, double *veci,
                                        int fftlen, int fftnum) {
    int index_x = blockIdx.x * blockDim.x + threadIdx.x;
    int index_y = blockIdx.y * blockDim.y + threadIdx.y;

    int matindex = index_y * fftlen + index_x; // mat should be a contiguous array
    // if N1%16>0, we're starting too many threads.
    // There is probably a better way to do this, but just eating the surplus
    // should work.
    if (matindex < fftlen * fftnum) {
        double matreal = matr[matindex];
        double matimag = mati[matindex];
        double vecreal = vecr[index_x];
        double vecimag = veci[index_x];

        matr[matindex] = matreal * vecreal - matimag * vecimag;
        mati[matindex] = matreal * vecimag + matimag * vecreal;
    }
}

__global__ void pressure_window_multiplication(double *mr, double *mi, double *A, double *p1,
                                               double *p2, double *p3, int winlen, int Ns1, int Ns2,
                                               int Ns3, int fftlen, int fftnum, double R21,
                                               double R00, double R31, double R10) {
    int index_x = blockIdx.x * blockDim.x + threadIdx.x;
    int index_y = blockIdx.y * blockDim.y + threadIdx.y;

    if (index_y < fftnum) { // eat the surplus
        int matindex = index_y * fftlen + index_x;

        double G = 1;
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

__global__ void velocity_window_multiplication(double *mr, double *mi, double *A, double *p1,
                                               double *p2, double *p3, int winlen, int Ns1, int Ns2,
                                               int Ns3, int fftlen, int fftnum, double R21,
                                               double R00, double R31, double R10) {
    int index_x = blockIdx.x * blockDim.x + threadIdx.x;
    int index_y = blockIdx.y * blockDim.y + threadIdx.y;

    if (index_y < fftnum) { // eat the surplus
        int matindex = index_y * fftlen + index_x;

        double G = 1;
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
