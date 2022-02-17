#include "signals.h"
#include "basic_signals.h"

float FLO(float volume, double time, float freq)
{
    return ttans(volume,freq,time); 
}
// Function that produces a signal
float create_signal(float volume, double time, float freq)
{
    // Orgue
    // sine(volume, freq, time) + 0.5 * saw2(volume, freq*2.0, time,4)+ 0.2 *
    // triangle(volume, freq*4.0, time)  +  0.1 * square(volume, freq*5.0,
    // time,0.25);

    return sine(volume, freq, time) + 0.5 * saw2(volume, freq*2.0, time,4)+ 0.2 *
    triangle(volume, freq*4.0, time)  +  0.1 * square(volume, freq*5.0,
    time,0.25);
}

// Functions that produces the octave of a signal
float octave_down(float volume, double time, float freq)
{
    return create_signal(volume, time, freq * 0.5);
}

// Functions that produces the octave of a signal
float octave_upp(float volume, double time, float freq)
{
    return create_signal(volume, time, freq * 2.0);
}

// Evaluate the stage execution of a signal and send the final value
float signal_treat(float volume, ud *data)
{
    float val = 0.0;
    for (int i = 0; i < 13; i++)
    {
        if (data->all_keys->keys[i] || (data->time_management->time_table[i]->release_stage && data->all_keys->effects[i] > 0.0))
        {
            val += data->all_keys->effects[i] * create_signal(volume, data->time_management->actual_time, data->all_keys->octave * piano_note_to_freq(i));
        }
        else
        {
            data->time_management->time_table[i]->release_stage = 0;
        }
    }
    return val;
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