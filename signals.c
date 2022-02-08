#include "signals.h"
#include "basic_signals.h"

float signal(float volume, double time, float freq)
{
    // Orgue
    // sine(volume, freq, time) + 0.5 * saw2(volume, freq*2.0, time,4)+ 0.2 *
    // triangle(volume, freq*4.0, time)  +  0.1 * square(volume, freq*5.0,
    // time,0.25);

    return sine(volume, freq, time) + 0.5 * saw2(volume, freq * 2.0, time, 4) + 0.2 * triangle(volume, freq * 4.0, time) + 0.1 * square(volume, freq * 5.0, time, 0.25);
}

float octave(float volume, double time, float freq)
{
    return signal(volume, time, freq * 0.5);
}

float signal_treat(float volume, double time, Keys *keys, ud *data)
{
    float val = 0.0;

    for (int i = 0; i < 13; i++)
    {
        if (keys->keys[i])
        {
            val += keys->effects[i] * signal(volume, time, piano_note_to_freq(i));
        }
        else
        {
            if(data->time_table[i]->release_stage)
            {
                float effect = keys->effects[i];
                if(effect <= 0.0)
                {
                    data->time_table[i]->release_stage = 0;
                }
                else
                {
                    val += effect* signal(volume, time, piano_note_to_freq(i));
                }
            }
        }
        
    }
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


char sdlk_to_freq(int c)
{
    char freq = 0;
    switch (c)
    {
        // C
    case 22:
        freq = 's';
        break;
        // C#
    case 8:
        freq = 'e';
        break;
        // D
    case 7:
        freq = 'd';
        break;
        // D#
    case 21:
        freq = 'r';
        break;
        // E
    case 9:
        freq = 'f';
        break;
        // F
    case 10:
        freq = 'g';
        break;
        // F#
    case 28:
        freq = 'y';
        break;
        // G
    case 11:
        freq = 'h';
        break;
        // G#
    case 24:
        freq = 'u';
        break;
        // A
    case 13:
        freq = 'j';
        break;
        // A#
    case 12:
        freq = 'i';
        break;
        // B
    case 14:
        freq = 'k';
        break;
        // C
    case 15:
        freq = 'l';
        break;
    default:
        freq = 0;
        break;
    }
    return freq;
}




int sdlk_to_piano_keys(int c)
{
    int freq = 0;
    switch (c)
    {
        // C
    case 22:
        freq = 0;
        break;
        // C#
    case 8:
        freq = 1;
        break;
        // D
    case 7:
        freq = 2;
        break;
        // D#
    case 21:
        freq = 3;
        break;
        // E
    case 9:
        freq = 4;
        break;
        // F
    case 10:
        freq = 5;
        break;
        // F#
    case 28:
        freq = 6;
        break;
        // G
    case 11:
        freq = 7;
        break;
        // G#
    case 24:
        freq = 8;
        break;
        // A
    case 13:
        freq = 9;
        break;
        // A#
    case 12:
        freq = 10;
        break;
        // B
    case 14:
        freq = 11;
        break;
        // C
    case 15:
        freq = 12;
        break;
    default:
        freq = 0;
        break;
    }
    return freq;
}


float piano_note_to_freq(int c)
{
    float freq = 0;
    switch (c)
    {
        // C
    case 0:
        freq = 261.63;
        break;
        // C#
    case 1:
        freq = 277.18;
        break;
        // D
    case 2:
        freq = 293.66;
        break;
        // D#
    case 3:
        freq = 311.13;
        break;
        // E
    case 4:
        freq = 329.63;
        break;
        // F
    case 5:
        freq = 349.23;
        break;
        // F#
    case 6:
        freq = 369.99;
        break;
        // G
    case 7:
        freq = 392.00;
        break;
        // G#
    case 8:
        freq = 415.30;
        break;
        // A
    case 9:
        freq = 440.00;
        break;
        // A#
    case 10:
        freq = 466.16;
        break;
        // B
    case 11:
        freq = 493.88;
        break;
        // C
    case 12:
        freq = 523.25;
        break;
    default:
        freq = 0;
        break;
    }
    return freq;
}


