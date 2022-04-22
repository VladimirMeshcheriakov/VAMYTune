#include "wav_management.h"

WavManagement * init_wav_management()
{
    WavManagement * wav_management = malloc(sizeof(WavManagement));
    //Create the buffer for storing plackback info
    wav_management->playback_buffer = calloc(2,sizeof(float));
    wav_management->played_samples = 0;
    wav_management->playback = 0;
    wav_management->record = 0;
    wav_management->recorded_samples = 0;
    wav_management->loop = 0;
    wav_management->loop_end = 0;
    wav_management->loop_start = 0;
    return wav_management;
}

void free_wav_management(WavManagement * wav_management)
{
    free(wav_management->playback_buffer);
    free(wav_management);
}