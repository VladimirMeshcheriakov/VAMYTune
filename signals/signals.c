#include "signals.h"


float FLO(float volume, double time, float freq)
{
    return ttans(volume,freq,time); 
}
// Function that produces a signal
float create_signal(float volume, double time, float freq)
{
    // Orgue
    /*
     sine(volume, freq, time) + 0.5 * saw2(volume, freq*2.0, time,4)+ 0.2 *
     triangle(volume, freq*4.0, time)  +  0.1 * square(volume, freq*5.0,
     time,0.25);
    */

    /*
    triangle(volume, freq, time) + 0.7 * sine(volume, freq*2.0, time)+ 0.5 *
     sine(volume, freq*3.0, time)  +  0.3 * triangle(volume, freq*3.0,
     time);
     */

    return tans(volume,freq,time) +  0.7 * saw2(volume, freq*2.0, time,10)+ 0.5 * triangle(volume,freq*3.0,time) +0.4 *
     triangle(volume, freq*4.0, time)  +  0.3 * square(volume, freq*5.0,
     time,0.34);
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
    

    for (int i = 0; i < 127; i++)
    {   
        if (data->all_keys->keys[i] || (data->time_management->time_table[i]->release_stage && (data->all_keys->effects[i] > 0.0) ))
        {
            val += data->all_keys->effects[i]  * create_signal(volume, data->time_management->actual_time, data->all_keys->octave * piano_note_to_freq(i));
        }
        else
        {
            data->time_management->time_table[i]->release_stage = 0;
        }
        
    }
    return val;
}

// Updates the effect table
void update_effects(ud *data)
{
    // printf_time(data->time_management->time_table,1);
    for (size_t i = 0; i < 127; i++)
    {
        if (data->all_keys->keys[i] || data->time_management->time_table[i]->release_stage)
        {
            data->all_keys->effects[i] = adsr_get_amplitude(data->time_management->actual_time, data->adsr, data->time_management->time_table[i]);
            // printf("%f\n", data->all_keys->effects[i]);
        }
        else
        {
            data->all_keys->effects[i] = 0;
        }
    }
}

float piano_note_to_freq(int n)
{
    float semitone = 1.059463094359;
    float C0 = 16.35;
    return C0 * powf(semitone,(float)n);
}
