#include <stdlib.h>
#include <math.h>

float sine(float volume, float frequency, double time)
{
    return volume * (sin(frequency * 2.0 * M_PI * time));
}

float tans(float volume, float frequency, double time)
{
    return volume * atan(sin(frequency * 2.0 * M_PI * time));
}

float square(float volume, float frequency, double time)
{
    return volume * ((sin(frequency * 2.0 * M_PI * time)) > 0.0 ? 1.0 : -1.0);
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

float note_to_freq(char c)
{
    float freq = 0;
    switch (c)
    {
        // C
    case 's':
        freq = 261.63;
        break;
        // C#
    case 'e':
        freq = 277.18;
        break;
        // D
    case 'd':
        freq = 293.66;
        break;
        // D#
    case 'r':
        freq = 311.13;
        break;
        // E
    case 'f':
        freq = 329.63;
        break;
        // F
    case 'g':
        freq = 349.23;
        break;
        // F#
    case 'y':
        freq = 369.99;
        break;
        // G
    case 'h':
        freq = 392.00;
        break;
        // G#
    case 'u':
        freq = 415.30;
        break;
        // A
    case 'j':
        freq = 440.00;
        break;
        // A#
    case 'i':
        freq = 466.16;
        break;
        // B
    case 'k':
        freq = 493.88;
        break;
        // C
    case 'l':
        freq = 523.25;
        break;
    default:
        freq = 0;
        break;
    }
    return freq;
}