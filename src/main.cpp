#include <stdio.h>
#include <raylib.h>
#include <mutex>
#include <stdint.h>
#include "dft.hpp"
#include "ringbuffer.hpp"

#define HEIGHT 500

RingBuffer buf;
int memchanged = 0;
unsigned int channels;
unsigned int samplesize;
std::mutex lock;
unsigned int frameslast = 0;

void stream_callback(void *bufferData, unsigned int frames)
{
    if(frames!=frameslast)
        printf("%u frames\n",frames);
    frameslast = frames;
    lock.lock();
    memchanged = 1;
    ringbuffer_push_back(&buf,(float*)bufferData,frames,channels);
    lock.unlock();
}


int main(int argc,char** argv)
{
    SetTraceLogLevel(LOG_WARNING);
    InitAudioDevice();
    if(argc<2)
    {
        fprintf(stderr,"Provide input file");
        return 1;
    }
    printf("Loading audio file: %s \n",argv[1]);
    Music music = LoadMusicStream(argv[1]);
    channels = music.stream.channels;
    samplesize = music.stream.sampleSize;
    allocate_ringbuffer(&buf,480);
    float* buflocal = (float*)malloc(buf.n_elements*sizeof(float));
    float* dft_re = (float*)malloc(buf.n_elements*sizeof(float));
    float* dft_im = (float*)malloc(buf.n_elements*sizeof(float));
    float* mags = (float*)malloc(buf.n_elements/2*sizeof(float));
    AttachAudioStreamProcessor(music.stream,stream_callback);
    PlayMusicStream(music);
    InitWindow(buf.n_elements,500,"visaudio");
    int monitor = GetCurrentMonitor();
    SetWindowSize(buf.n_elements,HEIGHT);
    SetTargetFPS(60);
    while(!WindowShouldClose())
    {
        UpdateMusicStream(music);
        ClearBackground(GRAY);    
        lock.lock();
        if(memchanged)
        {
            memcpy(buflocal,buf.base,buf.size_bytes);
            memchanged = 0;
        }
        lock.unlock();
        dft(dft_re,dft_im,buflocal,buf.n_elements);
        float maxmag;
        mag(dft_re,dft_im,mags,buf.n_elements/2,&maxmag);
        BeginDrawing();
        for(size_t i=0;i<buf.n_elements/2;i++)
        {
            int height = (int)(mags[i]/maxmag*HEIGHT);
            DrawRectangle(i*2,HEIGHT-height,2,height,BLACK);
        }
        EndDrawing();
    }
    free(buf.base);
    UnloadMusicStream(music);
    CloseAudioDevice();
    free(buflocal);
    free(dft_re);
    free(mags);
    CloseWindow();
}