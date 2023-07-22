#include <stdio.h>
#include <raylib.h>
#include <pthread.h>
#include <stdint.h>
#include "dft.h"
#include "ringbuffer.h"

RingBuffer buf;
int memchanged = 0;
unsigned int channels;
unsigned int samplesize;
pthread_mutex_t lock;

void stream_callback(void *bufferData, unsigned int frames)
{
    pthread_mutex_lock(&lock);
    memchanged = 1;
    ringbuffer_push_back(&buf,bufferData,frames,channels);
    pthread_mutex_unlock(&lock);
}


int main(int argc,char** argv)
{
    InitAudioDevice();
    Music alot = LoadMusicStream("a_lot.mp3");
    channels = alot.stream.channels;
    samplesize = alot.stream.sampleSize;
    allocate_ringbuffer(&buf,alot.stream.sampleRate/60,samplesize);
    void* buflocal = malloc(buf.sz_total);
    float* dft_re = malloc(buf.n_elements*sizeof(float));
    float* dft_im = dft_re+buf.n_elements*sizeof(float)/2;
    float* mags = malloc(buf.n_elements/2*sizeof(float));
    assert(buflocal!=0);
    assert(dft_re!=0);
    assert(mags!=0);
    assert(pthread_mutex_init(&lock,NULL)==0);
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
        pthread_mutex_lock(&lock);
        if(memchanged)
        {
            memcpy(buflocal,buf.base,buf.sz_total);
            memchanged = 0;
        }
        pthread_mutex_unlock(&lock);
        dft(dft_re,dft_im,buflocal,buf.n_elements);
        float maxmag;
        mag(dft_re,dft_im,mags,buf.n_elements/2,&maxmag);
        printf("%f \n",maxmag);
        BeginDrawing();
        for(int i=0;i<buf.n_elements;i++)
        {
            int height = mags[i]/maxmag*windowsizey/2;
            if(height>0)
            {
                DrawRectangle(i*2,windowsizey/2,1,height,BLACK);
            }
            if(height>0)
            {
                DrawRectangle(i*2,windowsizey/2-height,1,height,BLACK);
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