#ifndef __DFT_H__
#define __DFT_H__

#include <stdint.h>
#include <stddef.h>
#include <math.h>
#include <string.h>
#include <complex>

typedef std::complex<float> cplx;

void dft(float* re,float* im,float* data,size_t n);
void mag(cplx* data,float* mag,size_t n);
void fft(float *data,size_t stride,cplx* out, size_t n);

#endif // __DFT_H__