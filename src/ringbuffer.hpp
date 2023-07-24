#ifndef __RINGBUFFER_H__
#define __RINGBUFFER_H__

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

typedef struct {
    size_t size_bytes;
    size_t n_elements;
    float *base;
    float *end;
    float *curr;
} RingBuffer;

int allocate_ringbuffer(RingBuffer* buf,size_t n_elements);
void ringbuffer_push_back(RingBuffer* buf, float* data,size_t n_elements,size_t interleaved);

#endif // __RINGBUFFER_H__