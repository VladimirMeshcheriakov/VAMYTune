#include <stdlib.h>

#ifndef SIGNALS_H
#define SIGNALS_H

float sine(float volume, float frequency, double time);
float tans(float volume, float frequency, double time);
float ttans(float volume, float frequency, double time);
float square(float volume, float frequency, double time);
float triangle(float volume, float frequency, double time);
float saw(float volume, float frequency, double time);
float saw2(float volume, float frequency, double time, int limit);
float note_to_freq(char c);

#endif
