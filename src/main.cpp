#include <stdio.h>
#include <raylib.h>
#include <mutex>
#include <stdint.h>
#include "dft.hpp"
#include "ringbuffer.hpp"

#define HEIGHT 500
#define RINGBUF_SIZE 2048
#define NOTES 12
#define RECT_W 50
#define WINDOW_HEIGHT 500

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
    float notes[NOTES] = {0,140,320,520,820,1350,2100,3200,5200,8000,11000,20000};
    size_t indices[NOTES];
    float notelevelavg[NOTES-1];
    SetTraceLogLevel(LOG_WARNING);
    InitAudioDevice();
    if(argc<2)
    {
        fprintf(stderr,"Provide input file");
        return 1;
    }
    printf("Loading audio file: %s \n",argv[1]);
    Music music = LoadMusicStream(argv[1]);
    music.looping=false;
    for (size_t i = 0; i < NOTES; i++)
    {
        indices[i]=(int)notes[i]*RINGBUF_SIZE/music.stream.sampleRate;
    }    
    printf("\n");
    channels = music.stream.channels;
    samplesize = music.stream.sampleSize;
    allocate_ringbuffer(&buf,RINGBUF_SIZE);
    float* buflocal = (float*)malloc(buf.n_elements*sizeof(float));
    cplx* fftres = (cplx*)malloc(sizeof(cplx)*buf.n_elements);
    float* mags = (float*)malloc(buf.n_elements/2*sizeof(float));
    AttachAudioStreamProcessor(music.stream,stream_callback);
    PlayMusicStream(music);
    InitWindow(buf.n_elements,WINDOW_HEIGHT,"visaudio");
    int monitor = GetCurrentMonitor();
    SetWindowSize((NOTES-1)*RECT_W,HEIGHT);
    SetTargetFPS(60);
    float maxmaglast = 1;
    float maxmag=0;
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
        memset(notelevelavg,0,(NOTES-1)*sizeof(float));
        fft(buflocal,1,fftres,buf.n_elements);
        mag(fftres,mags,buf.n_elements/2);
        int indexcnt = 0;
        for (size_t i = indices[0]; i < indices[NOTES-1]; i++)
        {
            if(i==indices[indexcnt+1])
                indexcnt++;
            notelevelavg[indexcnt]+=mags[i];
        }
        for (size_t i = 0; i < NOTES-1; i++)
        {
            notelevelavg[i]/=(indices[i+1]-indices[i]);
            if(notelevelavg[i]>maxmag)
                maxmag = notelevelavg[i];
        }
        BeginDrawing();
        for(size_t i=0;i<NOTES-1;i++)
        {
            int height = (int)(notelevelavg[i]/maxmaglast*HEIGHT);
            DrawRectangle(i*RECT_W,HEIGHT-height,RECT_W,height,BLACK);
        }
        EndDrawing();
        maxmaglast = maxmag;
    }
    StopMusicStream(music);
    DetachAudioStreamProcessor(music.stream,stream_callback);
    free(buf.base);
    UnloadMusicStream(music);
    CloseAudioDevice();
    free(buflocal);
    free(fftres);
    free(mags);
    CloseWindow();
}