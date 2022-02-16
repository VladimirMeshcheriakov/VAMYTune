#include <stdlib.h>
#include "userdata.h"

#ifndef SIGNALS_H
#define SIGNALS_H

float piano_note_to_freq(int c);
float create_signal(float volume, double time, float freq);
float octave_down(float volume, double time, float freq);
float octave_upp(float volume, double time, float freq);
float signal_treat(float volume, ud *data);
#endif
