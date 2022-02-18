#include "basic_signals.h"


float sine(float volume, float frequency, double time)
{
    return volume * (sin(frequency * M_PI2 * time));
}

float logsin(float volume, float frequency, double time)
{
    return volume * (sin(log(frequency * M_PI2 * time)*sin(frequency * M_PI2 * time)));
}

float logcos(float volume, float frequency, double time)
{
    return volume * (cos(log(frequency * M_PI2 * time)*cos(frequency * M_PI2 * time)));
}

float tans(float volume, float frequency, double time)
{
    return volume * atan(sin(frequency * M_PI2 * time));
}

float ttans(float volume, float frequency, double time)
{
    return volume * acos(asin(sin(frequency * M_PI2 * time)) * TWO_OVER_PI);
}

float square(float volume, float frequency, double time, double shape)
/*
** shape : when shape == 0.0, then the signal is a 50% square wave, else it's a rectangle
*/
{
    return volume * ((sin(frequency * M_PI2 * time)) > shape ? 1.0 : -1.0);
}

float triangle(float volume, float frequency, double time)
{
    return volume * (asin(sin(frequency * M_PI2 * time)) * TWO_OVER_PI);
}

float saw(float volume, float frequency, double time)
{
    return volume * (TWO_OVER_PI) * (frequency * M_PI * fmod(time, 1.0 / frequency) - M_PI_OVER_TWO);
}

float saw2(float volume, float frequency, double time, int limit)
{
    float val = 0.0;
    for (int n = 1; n < limit; n++)
    {
        val += (sin(frequency * M_PI2 * time * (float)n)) / (float)n;
    }

    val = volume * (TWO_OVER_PI) * val;
    return val;
}
