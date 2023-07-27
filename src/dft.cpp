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

void mag(cplx* dft,float* mag,size_t n)
{
    for(size_t i=0;i<n;i++)
    {
        mag[i]=std::abs(dft[i]);
    }
}
 
void fft(float *data,size_t stride,cplx* out, size_t n)
{
    if(n<=1)
    {
        out[0]=cplx{data[0],0};
        return;
    }
    fft(data,stride*2,out,n/2);
    fft(data+stride,stride*2,out+n/2,n/2);
    for (size_t k = 0; k < n/2; k++)
    {
        float t = (float)k/n;
        cplx v = std::exp(cplx{0,-pi2*t})*out[k+n/2];
        cplx e = out[k];
        out[k] = e+v;
        out[k+n/2] = e-v;
    }
}