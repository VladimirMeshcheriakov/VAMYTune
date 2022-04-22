#include <stdlib.h>
#include "SDL2/SDL.h"

#ifndef WAV_MANAGEMENT
#define WAV_MANAGEMENT

//Struct to manage the recoring and playback of files
typedef struct 
{
    // Buffer including a table of 2 floats to be passed to the read/write function
    float *playback_buffer;
    //Samples recorded, passed to the recoding function
    uint64_t recorded_samples;
    //Samples played from the chosen file
    uint64_t played_samples;
    // 1 to set the record 0 to unset
    int record;
    // 1 to set playback, 0 to unset
    int playback;
    // 1 to set loop, 
    int loop;
    // start of the loop
    double loop_start;
    //End of loop
    double loop_end;
}WavManagement;

//Init a wav management struct instance
WavManagement * init_wav_management();

//Free a wav management struct instance
void free_wav_management(WavManagement * wav_management);

#endif