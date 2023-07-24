#include "ringbuffer.hpp"

int allocate_ringbuffer(RingBuffer* buf,size_t n_elements, size_t element_size)
{
    buf->n_elements = n_elements;
    buf->element_size = element_size;
    size_t sz = n_elements*element_size;
    buf->sz_total = sz;
    buf->base = calloc(n_elements,element_size);
    assert(buf->base);
    buf->curr_start = buf->base;
    buf->end = (char*)(buf->base)+buf->sz_total;
    return 0;
}

void ringbuffer_push_back(RingBuffer* buf, void* data,size_t n_elements,size_t jmp) //jmp is the number of channels for audio
{
    size_t sz = n_elements*buf->element_size;
    assert(sz<=buf->sz_total);
    size_t beforeof = (char*)buf->end-(char*)buf->curr_start;
    if(beforeof>sz)
    {
        for (size_t i = 0; i < n_elements; i++)
        {
            for (size_t j = 0; j < buf->element_size; j++)
            {
                *(char*)(buf->curr_start)=*(char*)((char*)data+(i*jmp*buf->element_size)+j);
                (*((char**)&(buf->curr_start)))++;
            }
            
        }
    }
    else
    {
        for (size_t i = 0; i < beforeof/buf->element_size; i++)
        {
            for (size_t j = 0; j < buf->element_size; j++)
            {
                *(unsigned char*)(buf->curr_start)=*(char*)((char*)data+(i*jmp*buf->element_size)+j);
                (*((char**)&(buf->curr_start)))++;
            }
            
        }
        buf->curr_start = buf->base;
        for (size_t i = beforeof/buf->element_size; i < n_elements-beforeof/buf->element_size; i++)
        {
            for (size_t j = 0; j < buf->element_size; j++)
            {
                *(unsigned char*)(buf->curr_start)=*(unsigned char*)((char*)data+(i*jmp*buf->element_size)+j);
                (*((char**)&(buf->curr_start)))++;
            }
            
        }
    }
}


