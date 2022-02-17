#include "userdata.h"
#include "keys.h"

// Init the User_Data structure, with a given enveloppe and the number of keys
ud *init_ud(uint64_t *samples_played, ADSR *env, size_t size)
{
    ud *data = malloc(sizeof(ud));
    TimeManagement * time_management = init_time_management(size);
    data->adsr = env;
    data->time_management = time_management;
    data->samples_played = samples_played;
    Keys *all_keys = init_keys(size);
    data->all_keys = all_keys;
    data->side_effect = malloc(sizeof(Uint8) * 6);
    data->fstream = initArray(1);
    data->wav_manager = init_wav_management();
    data->fout_size = 0;
    // play the 0 frequency to have something to start with
    // 13 for all the frequencies that can be palyed

    return data;
}

// Free User_data structure
void free_user_data(ud *data)
{
    free(data->side_effect);
    free(data->adsr);
    free_keys(data->all_keys);
    free_time_management(data->time_management);
    freeArray(data->fstream);
    free_wav_management(data->wav_manager);
    free(data);
}