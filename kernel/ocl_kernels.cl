#pragma OPENCL EXTENSION cl_khr_fp64 : enable

__kernel void derifact_multiplication(__global double *matr, __global double *mati, __global double *vecr, __global double *veci, const int fftlen, const int fftnum)
{
    int index_x = get_global_id(0) * get_global_size(0) + get_local_id(0); 
    int index_y = get_global_id(1) * get_global_size(1) + get_local_id(1);

    int matindex = index_y*fftlen+index_x; //mat should be a contiguous array
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

__kernel void pressure_window_multiplication(__global double *mr, __global double *mi, __global double *A, __global double *p1, __global double *p2, __global double *p3, __global int *winlen, __global int *Ns1, __global int *Ns2, __global int *Ns3, __global int *fftlen, __global int *fftnum, __global double *R21, __global double *R00, __global double *R31, __global double *R10)
{
    int index_x = get_global_id(0) * get_global_size(0) + get_local_id(0); 
    int index_y = get_global_id(1) * get_global_size(1) + get_local_id(1);

    if (index_y < *fftnum) { //eat the surplus
        int matindex = index_y**fftlen+index_x;

        double G = 1;
        if (index_x < *winlen) {
            G = A[index_x];
        } else if (index_x > *winlen+*Ns2-1 && index_x < *winlen*2+*Ns2) {
            G = A[index_x-*Ns2];
        }
        mi[matindex] = 0;
        if (index_x < *winlen) {
            mr[matindex] = G*(*R21*p1[*Ns1*index_y+index_x-*winlen+*Ns1] + *R00*p2[*Ns2*index_y+*winlen-1-index_x]);
        } else if (index_x < *winlen + *Ns2) {
            mr[matindex] = p2[*Ns2*index_y+index_x-*winlen];
        } else if (index_x < *winlen*2+*Ns2) {
            mr[matindex] = G*(*R31*p3[*Ns3*index_y+index_x-*winlen-*Ns2] + *R10*p2[*Ns2*index_y+2**Ns2+*winlen-1-index_x]);
        } else {
            mr[matindex] = 0; //zero padding
        }
    }
}

__kernel void velocity_window_multiplication(__global double *mr, __global double *mi, __global double *A, __global double *p1, __global double *p2, __global double *p3, __global int *winlen, __global int *Ns1, __global int *Ns2, __global int *Ns3, __global int *fftlen, __global int *fftnum, __global double *R21, __global double *R00, __global double *R31, __global double *R10)
{
    int index_x = get_global_id(0) * get_global_size(0) + get_local_id(0); 
    int index_y = get_global_id(1) * get_global_size(1) + get_local_id(1);

    if (index_y < *fftnum) { //eat the surplus
        int matindex = index_y**fftlen+index_x;

        double G = 1;
        if (index_x < *winlen) {
            G = A[index_x];
        } else if (index_x > *winlen+*Ns2-1 && index_x < *winlen*2+*Ns2) {
            G = A[index_x-*Ns2];
        }
        mi[matindex] = 0;
        if (index_x < *winlen) {
            mr[matindex] = G*(*R21*p1[*Ns1*index_y+index_x-*winlen+*Ns1-1] + *R00*p2[*Ns2*index_y+*winlen-index_x]);
        } else if (index_x < *winlen + *Ns2) {
            mr[matindex] = p2[*Ns2*index_y+index_x-*winlen];
        } else if (index_x < *winlen*2+*Ns2) {
            mr[matindex] = G*(*R31*p3[*Ns3*index_y+index_x-*winlen-*Ns2+1] + *R10*p2[*Ns2*index_y+2**Ns2+*winlen-2-index_x]);
        } else {
            mr[matindex] = 0; //zero padding
        }
    }
}
