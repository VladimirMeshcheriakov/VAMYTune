#include <stdlib.h>
#include <math.h>

#ifndef BASIC_SIGNALS_H
#define BASIC_SIGNALS_H


#ifndef M_PI
#define M_PI    3.14159265358979323846264338327950288 
#endif

#define M_PI2   6.28318530717958647692528676655900576 

#define TWO_OVER_PI (2.0 / 3.14159265358979323846264338327950288) 

#define M_PI_OVER_TWO (3.14159265358979323846264338327950288 /2.0)

//Contains all the basic signal forms
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