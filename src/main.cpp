#include <stdio.h>
#include <raylib.h>
#include <mutex>
#include <stdint.h>
#include "dft.hpp"
#include "ringbuffer.hpp"

RingBuffer buf;
int memchanged = 0;
unsigned int channels;
unsigned int samplesize;
std::mutex lock;

void stream_callback(void *bufferData, unsigned int frames)
{
    lock.lock();
    memchanged = 1;
    ringbuffer_push_back(&buf,bufferData,frames,channels);
    lock.unlock();
}


int main(int argc,char** argv)
{
    InitAudioDevice();
    Music alot = LoadMusicStream(argv[1]);
    channels = alot.stream.channels;
    samplesize = alot.stream.sampleSize;
    allocate_ringbuffer(&buf,alot.stream.sampleRate/60,samplesize);
    uint16_t* buflocal = (uint16_t*)malloc(buf.sz_total);
    float* dft_re = (float*)malloc(buf.n_elements*sizeof(float));
    float* dft_im = dft_re+buf.n_elements*sizeof(float)/2;
    float* mags = (float*)malloc(buf.n_elements/2*sizeof(float));
    assert(buflocal!=0);
    assert(dft_re!=0);
    assert(mags!=0);
    
    AttachAudioStreamProcessor(alot.stream,stream_callback);
    PlayMusicStream(alot);
    InitWindow(500,400,"visaudio");
    int monitor = GetCurrentMonitor();
    int windowsizey = 500;
    SetWindowSize(buf.n_elements,windowsizey);
    SetTargetFPS(60);
    while(!WindowShouldClose())
    {
        UpdateMusicStream(alot);
        ClearBackground(GRAY);    
        lock.lock();
        if(memchanged)
        {
            memcpy(buflocal,buf.base,buf.sz_total);
            memchanged = 0;
        }
        lock.unlock();
        
        dft(dft_re,dft_im,buflocal,buf.n_elements);
        float maxmag;
        mag(dft_re,dft_im,mags,buf.n_elements/2,&maxmag);
        BeginDrawing();
        for(size_t i=0;i<buf.n_elements/2;i++)
        {
            int height = (int)(mags[i]/maxmag*windowsizey/2);
            if(height>0)
            {
                DrawRectangle(i*4,windowsizey/2,2,height,BLACK);
            }
            if(height>0)
            {
                DrawRectangle(i*4,windowsizey/2-height,2,height,BLACK);
            }
        }
        EndDrawing();
    }
    printf("bye\n");
    free(buf.base);
    UnloadMusicStream(alot);
    CloseAudioDevice();
    free(buflocal);
    free(dft_re);
    free(mags);
    CloseWindow();
}