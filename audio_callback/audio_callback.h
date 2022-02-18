#include "../sdl_call_func/sdl_call_func.h"

#ifndef AUDIO_CALLBACK
#define AUDIO_CALLBACK

void audio_callback(void *userdata, uint8_t *stream, int len);

#endif