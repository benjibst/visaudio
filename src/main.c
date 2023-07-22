#include <stdio.h>
#include <raylib.h>
#include "ringbuffer.h"

#define W 500
#define H 400

int sample_rate;
int channels;
int sample_size;

void stream_callback(void *bufferData, unsigned int frames)
{

}


int main(int argc,char** argv)
{
    RingBuffer buf;
    allocate_ringbuffer(&buf,10,4);
    int data[]={1,2,3};
    int data2[]={1,2,3,5};
    int data3[]={1,2,3,4,5,6,7,8,9,10};
    ringbuffer_push_back(&buf,data3,10);
    ringbuffer_push_back(&buf,data,3);
    ringbuffer_push_back(&buf,data2,4);
    for (size_t i = 0; i < 10; i++)
    {
        void* target = buf.curr_start+i*buf.element_size;
        if(target>=buf.end)
            target-=buf.sz_total;
        int* targetint = (int*)target;
        printf("%d ",*targetint);
    }
    printf("\n");
    
    /* InitAudioDevice();
    Music alot = LoadMusicStream("a_lot.mp3");
    AttachAudioStreamProcessor(alot.stream,stream_callback);
    PlayMusicStream(alot);
    InitWindow(W,H,"visaudio");
    SetTargetFPS(60);
    while(!WindowShouldClose())
    {
        UpdateMusicStream(alot);
        BeginDrawing();
        ClearBackground(GRAY);
        EndDrawing();
    } */
}