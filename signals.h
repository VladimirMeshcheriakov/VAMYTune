#include <stdlib.h>
#include "keys.h"
#include "userdata.h"

#ifndef SIGNALS_H
#define SIGNALS_H

float note_to_freq(char c);
char sdlk_to_freq(int c);
int sdlk_to_piano_keys(int c);
float piano_note_to_freq(int c);
float signal(float volume, double time, float freq);
float octave(float volume, double time, float freq);
float signal_treat(float volume, double time, Keys *keys, ud *data);
#endif
