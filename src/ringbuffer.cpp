#include "ringbuffer.hpp"

int allocate_ringbuffer(RingBuffer* buf,size_t n_elements)
{
    buf->size_bytes = n_elements*sizeof(float);
    buf->n_elements = n_elements;
    buf->base = (float*)calloc(n_elements,sizeof(float));
    if(!buf->base)
        return 1;
    buf->curr = buf->base;
    buf->end = buf->base+n_elements;
    return 0;
}

void ringbuffer_push_back(RingBuffer* buf, float* data,size_t n_elements,size_t interleaved)
{
    size_t beforeof = buf->end-buf->curr;
    size_t cnt = 0;
    while (n_elements-->0)
    {
        *(buf->curr) = data[cnt++];
        buf->curr++;
        buf->curr -= buf->n_elements*(buf->curr==buf->end);
    }
    
}


