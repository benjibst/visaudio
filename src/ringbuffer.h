#ifndef __RINGBUFFER_H__
#define __RINGBUFFER_H__

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

typedef struct {
    void *base;
    size_t element_size;
    size_t n_elements;
    size_t sz_total;
    void* end;
    void* curr_start;
} RingBuffer;

int allocate_ringbuffer(RingBuffer* buf,size_t n_elements, size_t element_size);
int ringbuffer_push_back(RingBuffer* buf, void* data,size_t n_elements);

#endif // __RINGBUFFER_H__