#include "dft.hpp"
const float pi2 = 6.28318530718;


void dft(float* re,float* im,uint16_t* data,size_t n)
{
    for(int k=0;k<n/2;k++)
    {
        re[k]=0;
        im[k]=0;
        for(int j=0;j<n;j++)
        {
            re[k] += data[j]*cos(pi2*j*k/n);
            im[k] -= data[j]*sin(pi2*j*k/n);
        }
    }
}

void mag(float* re,float* im,float* mag,size_t n,float* maxmag)
{
    *maxmag = 0;
    for(int i=0;i<n;i++)
    {
        mag[i]=sqrt(re[i]*re[i]+im[i]*im[i]);
        if(mag[i]>*maxmag)
            *maxmag = mag[i];
    }
}
