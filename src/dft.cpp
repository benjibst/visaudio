#include "dft.hpp"
const float pi2 = 6.28318530718f;


void dft(float* re,float* im,float* data,size_t n)
{
    for(size_t k=0;k<n/2;k++)
    {
        re[k]=0;
        im[k]=0;
        for(size_t j=0;j<n;j++)
        {
            re[k] += data[j]*cosf(pi2*j*k/n);
            im[k] -= data[j]*sinf(pi2*j*k/n);
        }
    }
}

void mag(float* re,float* im,float* mag,size_t n,float* maxmag)
{
    *maxmag = 0;
    for(size_t i=0;i<n;i++)
    {
        mag[i]=sqrtf(re[i]*re[i]+im[i]*im[i]);
        if(mag[i]>*maxmag)
            *maxmag = mag[i];
    }
}