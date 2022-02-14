#include "userdata.h"
#include "keys.h"

//Init the User_Data structure, with a given enveloppe and the number of keys
ud * init_ud(uint64_t *samples_played, ADSR * env,  size_t size)
{
    ud *data = malloc(sizeof(ud));
    TimeStamp **table_time = init_time_table(size);
    data->adsr = env;
    data->time_table = table_time;
    data->samples_played = samples_played;
    Keys *all_keys = init_keys(size);
    data->all_keys = all_keys;
    data->octave = 1.0;
    data->side_effect = malloc(sizeof(Uint8)*4);
    data->fstream = initArray(1);
    data->record = 0;
    data->recorded_samples = 0;
    // play the 0 frequency to have something to start with
    //13 for all the frequencies that can be palyed
    return data;
}

//Free User_data structure
void free_user_data(ud *data, size_t size)
{
    free(data->side_effect);
    free(data->adsr);
    free_keys(data->all_keys);
    free_time_table(data->time_table,size);
    freeArray(data->fstream);
    free(data);
}