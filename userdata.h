#include "time_stamp.h"
#include "adsr.h"
#include "keys.h"
#include "dynamic_array.h"

#ifndef USER_DATA_H
#define USER_DATA_H
/*
User data structure
contains all the information that is present in the programm
for the sound execution
*/
typedef struct
{
    uint64_t *samples_played;
    uint64_t recorded_samples;
    ADSR *adsr;
    TimeStamp **time_table;
    double actual_time;
    Keys *all_keys;
    float octave;
    Uint8 *side_effect;
    Array *fstream;
    int record;
} ud;

ud * init_ud(uint64_t *samples_played, ADSR * env,  size_t size);
void free_user_data(ud *data, size_t size);
#endif