#include "signals.h"
#include "userdata.h"
#include "adsr.h"
#include "note_state_utils.h"
#include "wav_interface.h"

#ifndef SDL_CALL_FUNC_H
#define SDL_CALL_FUNC_H


void init_run_app(ud *data,  void *audio_callback);

#endif