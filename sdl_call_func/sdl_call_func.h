#include "../signals/signals.h"
#include "../wav_interface/wav_interface.h"
#include "../midi_interface_management/midi_management.h"

#ifndef SDL_CALL_FUNC_H
#define SDL_CALL_FUNC_H

void init_run_app(ud *data, void *audio_callback, int argc, char *argv[]);

#endif