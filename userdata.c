#include "userdata.h"


ud * init_ud(uint64_t samples_played, ADSR *env)
{
    ud *data = malloc(sizeof(ud));
    data->adsr = env;
    data->samples_played = &samples_played;
    // play the 0 frequency to have something to start with
    data->freq = 0.0;
    //13 for all the frequencies that can be palyed
    return data;
}