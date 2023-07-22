#include "ringbuffer.h"

int allocate_ringbuffer(RingBuffer* buf,size_t n_elements, size_t element_size)
{
    buf->n_elements = n_elements;
    buf->element_size = element_size;
    size_t sz = n_elements*element_size;
    buf->sz_total = sz;
    buf->base = calloc(n_elements,element_size);
    assert(buf->base);
    buf->curr_start = buf->base;
    buf->end = buf->base+buf->sz_total;
    return 0;
}

int ringbuffer_push_back(RingBuffer* buf, void* data,size_t n_elements)
{
    size_t sz = n_elements*buf->element_size;
    assert(sz<=buf->sz_total);
    size_t beforeof = buf->end-buf->curr_start;
    if(beforeof>sz)
        memcpy(buf->curr_start,data,sz);
    else
    {
        memcpy(buf->curr_start,data,beforeof);
        memcpy(buf->base,data + beforeof,sz-beforeof);
    }
    buf->curr_start+=sz;
    if(buf->curr_start>buf->end)
        buf->curr_start-=buf->sz_total;
}


