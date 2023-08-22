#include <stdio.h>
#include <raylib.h>
#include <mutex>
#include <stdint.h>
#include "dft.hpp"
#include "ringbuffer.hpp"

#define RINGBUF_SIZE 2048
#define WINDOW_HEIGHT 500
#define WINDOW_WIDTH 700

RingBuffer buf;
int memchanged = 0;
unsigned int channels;
unsigned int samplesize;
std::mutex lock;
unsigned int frameslast = 0;
void *buffer_g;
unsigned int frames_g;
float notes[] = {0, 80, 120, 180, 240, 320, 520, 820, 1100, 1350, 1600, 2100, 2600, 3200, 3600, 4000, 4600, 5200, 5750, 6500, 8000, 11000, 20000};
#define NOTES (sizeof(notes) / sizeof(float))
#define RECT_W (WINDOW_WIDTH / (NOTES-1))

void stream_callback(void *bufferData, unsigned int frames)
{
    if (frames != frameslast)
        printf("%u frames\n", frames);
    frameslast = frames;
    lock.lock();
    memchanged = 1;
    buffer_g = bufferData;
    frames_g = frames;
    lock.unlock();
}

int GetNumKeyPressed()
{
    int key = GetKeyPressed();
    if (key < 48 || key > 57)
        return -1;
    return key - KEY_ZERO;
}

int main(int argc, char **argv)
{
    size_t indices[NOTES];
    float notelevelmax[NOTES - 1];
    SetTraceLogLevel(LOG_WARNING);
    InitAudioDevice();
    if (argc < 2)
    {
        fprintf(stderr, "Provide input file");
        return 1;
    }
    printf("Loading audio file: %s \n", argv[1]);
    Music music = LoadMusicStream(argv[1]);
    float musiclength = GetMusicTimeLength(music);
    music.looping = false;
    for (size_t i = 0; i < NOTES; i++)
    {
        indices[i] = (int)notes[i] * RINGBUF_SIZE / music.stream.sampleRate;
    }
    printf("\n");
    channels = music.stream.channels;
    samplesize = music.stream.sampleSize;
    allocate_ringbuffer(&buf, RINGBUF_SIZE);
    cplx *fftres = (cplx *)malloc(sizeof(cplx) * buf.n_elements);
    float *mags = (float *)malloc(buf.n_elements / 2 * sizeof(float));
    AttachAudioStreamProcessor(music.stream, stream_callback);
    PlayMusicStream(music);
    InitWindow((int)buf.n_elements, WINDOW_HEIGHT, "visaudio");
    SetWindowSize((NOTES - 1) * RECT_W, WINDOW_HEIGHT);
    SetTargetFPS(60);
    float maxmagalltime = 0;
    float maxmag = 0;
    size_t maxmagidx = 0;
    float maxmagrelpos = 0;
    int key;
    int indexcnt;
    int height;
    float heightrel;
    Color c = {0,0,0,255};
    while (!WindowShouldClose())
    {
        if ((key = GetNumKeyPressed()) != -1)
        {
            SeekMusicStream(music, key * musiclength / 10.0f);
        }
        UpdateMusicStream(music);
        ClearBackground(GRAY);
        lock.lock();
        if (memchanged)
        {
            ringbuffer_push_back(&buf, (float *)buffer_g, frames_g, channels);
            memchanged = 0;
        }
        lock.unlock();
        memset(notelevelmax, 0, (NOTES - 1) * sizeof(float));
        fft(buf.base, 1, fftres, buf.n_elements);
        mag(fftres, mags, buf.n_elements / 2);
        indexcnt = 0;
        maxmag = 0;
        for (size_t i = indices[0]; i < indices[NOTES - 1]; i++)
        {
            if (i == indices[indexcnt + 1])
                indexcnt++;
            if (mags[i] > notelevelmax[indexcnt])
            {
                notelevelmax[indexcnt] = mags[i];
                if(mags[i]>maxmagalltime)
                    maxmagalltime = mags[i];
                if (mags[i] > maxmag)
                {
                    maxmag = mags[i];
                    maxmagidx = indexcnt;
                }
            }
        }
        //relative position within frequencies scaled from 0 to 1
        maxmagrelpos = (float)maxmagidx/(float)(NOTES-1); 
        BeginDrawing();
        for (size_t i = 0; i < NOTES - 1; i++)
        {
            height = (int)(notelevelmax[i] / maxmagalltime * WINDOW_HEIGHT);
            height = (height<WINDOW_HEIGHT)*height + (height>WINDOW_HEIGHT)*WINDOW_HEIGHT;
            heightrel = (height/(float)WINDOW_HEIGHT)*0.8f;
            c.r = (unsigned char)(255*(heightrel));
            c.g = (unsigned char)(255*(1-heightrel));
            DrawRectangle((int)i * RECT_W, WINDOW_HEIGHT - height, RECT_W, height, c);
        }
        DrawCircleLines(WINDOW_WIDTH-100,100,70,BLUE);
        DrawLine(WINDOW_WIDTH-100,100,(int)(WINDOW_WIDTH-100+cosf(maxmagrelpos*2*PI)*70),(int)(100+sinf(maxmagrelpos*2*PI)*70),RED);
        EndDrawing();
    }
    StopMusicStream(music);
    DetachAudioStreamProcessor(music.stream, stream_callback);
    free(buf.base);
    UnloadMusicStream(music);
    CloseAudioDevice();
    free(fftres);
    free(mags);
    CloseWindow();
}