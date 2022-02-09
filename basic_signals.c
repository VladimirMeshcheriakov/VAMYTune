#include "basic_signals.h"


//Contains all the basic signal forms
float sine(float volume, float frequency, double time)
{
    return volume * (sin(frequency * 2.0 * M_PI * time));
}

float logsin(float volume, float frequency, double time)
{
    return volume * (sin(log(frequency * 2.0 * M_PI * time)*sin(frequency * 2.0 * M_PI * time)));
}

float logcos(float volume, float frequency, double time)
{
    return volume * (cos(log(frequency * 2.0 * M_PI * time)*cos(frequency * 2.0 * M_PI * time)));
}

float tans(float volume, float frequency, double time)
{
    return volume * atan(sin(frequency * 2.0 * M_PI * time));
}

float ttans(float volume, float frequency, double time)
{
    return volume * acos(asin(sin(frequency * 2.0 * M_PI * time)) * 2.0 / M_PI);
}

float square(float volume, float frequency, double time, double shape)
/*
** shape : when shape == 0.0, then the signal is a 50% square wave, else it's a rectangle
*/
{
    return volume * ((sin(frequency * 2.0 * M_PI * time)) > shape ? 1.0 : -1.0);
}

float triangle(float volume, float frequency, double time)
{
    return volume * (asin(sin(frequency * 2.0 * M_PI * time)) * 2.0 / M_PI);
}

float saw(float volume, float frequency, double time)
{
    return volume * (2.0 / M_PI) * (frequency * M_PI * fmod(time, 1.0 / frequency) - (M_PI / 2.0));
}

float saw2(float volume, float frequency, double time, int limit)
{
    float val = 0.0;
    for (int n = 1; n < limit; n++)
    {
        val += (sin(frequency * 2.0 * M_PI * time * (float)n)) / (float)n;
    }

    val = volume * (2.0 / M_PI) * val;
    return val;
}
