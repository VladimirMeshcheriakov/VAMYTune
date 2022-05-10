#include "signals.h"


// Updates the effect table
void update_effects(vis_data *data)
{
    // printf_time(data->time_management->time_table,1);
    for (size_t i = 0; i < 127; i++)
    {
        
        if (data->data->all_keys->keys[i] || data->data->time_management->time_table[i]->release_stage)
        {
            //printf("phases %f, %f, %f\n",data->attack_phase,data->decay_phase,data->release_phase);
            data->data->all_keys->effects[i] = adsr_get_amplitude(data->data->time_management->actual_time, data->data->adsr, data->data->time_management->time_table[i]);
            //printf(" %f \n",data->data->time_management->actual_time);
            //printf("%f\n", data->data->all_keys->effects[i]);
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
