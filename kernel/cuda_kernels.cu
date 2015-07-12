#include <stdio.h>
__global__ void derifact_multiplication(double *matr, double *mati, double *vecr, double *veci, int fftlen, int fftnum)
{
    int index_x = blockIdx.x*blockDim.x + threadIdx.x; 
    int index_y = blockIdx.y*blockDim.y + threadIdx.y;

    int matindex = index_y*fftlen+index_x; //mat should be a contiguous array
    //printf("Block(x,y): (%d,%d). Thread(x,y): (%d,%d)\\n",blockIdx.x,blockIdx.y,threadIdx.x,threadIdx.y);
    // if N1%16>0, we're starting too many threads.
    // There is probably a better way to do this, but just eating the surplus should work.
    if (matindex < fftlen*fftnum) {
        double matreal = matr[matindex];
        double matimag = mati[matindex];
        double vecreal = vecr[index_x];
        double vecimag = veci[index_x];

        matr[matindex] = matreal*vecreal - matimag*vecimag;
        mati[matindex] = matreal*vecimag + matimag*vecreal;
    }
}

__global__ void pressure_window_multiplication(double *mr, double *mi, double *A, double *p1, double *p2, double *p3, int winlen, int Ns1, int Ns2, int Ns3, int fftlen, int fftnum, double R21, double R00, double R31, double R10) //passing a few by value seems to be more efficient than building an array first in pycuda
{
    int index_x = blockIdx.x*blockDim.x + threadIdx.x; 
    int index_y = blockIdx.y*blockDim.y + threadIdx.y;

    if (index_y < fftnum) { //eat the surplus
        int matindex = index_y*fftlen+index_x;

        double G = 1;
        if (index_x < winlen) {
            G = A[index_x];
        } else if (index_x > winlen+Ns2-1 && index_x < winlen*2+Ns2) {
            G = A[index_x-Ns2];
        }
        mi[matindex] = 0;
        if (index_x < winlen) {
            mr[matindex] = G*(R21*p1[Ns1*index_y+index_x-winlen+Ns1] + R00*p2[Ns2*index_y+winlen-1-index_x]);
        } else if (index_x < winlen + Ns2) {
            mr[matindex] = p2[Ns2*index_y+index_x-winlen];
        } else if (index_x < winlen*2+Ns2) {
            mr[matindex] = G*(R31*p3[Ns3*index_y+index_x-winlen-Ns2] + R10*p2[Ns2*index_y+2*Ns2+winlen-1-index_x]);
        } else {
            mr[matindex] = 0; //zero padding
        }
        //if(mr[matindex]==0 && matindex < 50) printf("zero at:%d\\n",matindex%fftlen);
    }
}

__global__ void velocity_window_multiplication(double *mr, double *mi, double *A, double *p1, double *p2, double *p3, int winlen, int Ns1, int Ns2, int Ns3, int fftlen, int fftnum, double R21, double R00, double R31, double R10) //passing a few by value seems to be more efficient than building an array first in pycuda
{
    int index_x = blockIdx.x*blockDim.x + threadIdx.x; 
    int index_y = blockIdx.y*blockDim.y + threadIdx.y;

    if (index_y < fftnum) { //eat the surplus
        int matindex = index_y*fftlen+index_x;

        double G = 1;
        if (index_x < winlen) {
            G = A[index_x];
        } else if (index_x > winlen+Ns2-1 && index_x < winlen*2+Ns2) {
            G = A[index_x-Ns2];
        }
        mi[matindex] = 0;
        if (index_x < winlen) {
            mr[matindex] = G*(R21*p1[Ns1*index_y+index_x-winlen+Ns1-1] + R00*p2[Ns2*index_y+winlen-index_x]);
        } else if (index_x < winlen + Ns2) {
            mr[matindex] = p2[Ns2*index_y+index_x-winlen];
        } else if (index_x < winlen*2+Ns2) {
            mr[matindex] = G*(R31*p3[Ns3*index_y+index_x-winlen-Ns2+1] + R10*p2[Ns2*index_y+2*Ns2+winlen-2-index_x]);
        } else {
            mr[matindex] = 0; //zero padding
        }
        //if(mr[matindex]==0 && matindex < 50) printf("zero at:%d\\n",matindex%fftlen);
    }
}
