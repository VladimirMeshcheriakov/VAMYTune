#include <stdlib.h>
#include <math.h>

#ifndef BASIC_SIGNALS_H
#define BASIC_SIGNALS_H

float logsin(float volume, float frequency, double time);
float logcos(float volume, float frequency, double time);
float sine(float volume, float frequency, double time);
float tans(float volume, float frequency, double time);
float ttans(float volume, float frequency, double time);
float square(float volume, float frequency, double time, double shape);
float triangle(float volume, float frequency, double time);
float saw(float volume, float frequency, double time);
float saw2(float volume, float frequency, double time, int limit);

#endif