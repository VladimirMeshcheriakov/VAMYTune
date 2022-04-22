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

    return tans(volume,freq,time);
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



// Updates the effect table
void update_effects(vis_data *data)
{
    // printf_time(data->time_management->time_table,1);
    for (size_t i = 0; i < 127; i++)
    {
        if (data->data->all_keys->keys[i] || data->data->time_management->time_table[i]->release_stage)
        {
            ADSR * adsr = init_ADSR_envelope(data->attack_phase,data->decay_phase,data->release_phase,data->attack_amp,data->decay_amp,data->sustain_amp,0.0);
            data->data->all_keys->effects[i] = adsr_get_amplitude(data->data->time_management->actual_time, adsr, data->data->time_management->time_table[i]);
            // printf("%f\n", data->all_keys->effects[i]);
            free(adsr);
        }
        else
        {
            data->data->all_keys->effects[i] = 0;
        }
        
    }
}

float piano_note_to_freq(int n)
{
    float semitone = 1.059463094359;
    float C0 = 16.35;
    return C0 * powf(semitone,(float)n);
}
