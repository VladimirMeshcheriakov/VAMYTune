#include "time_stamp.h"
#include "adsr.h"
#include "keys.h"

#ifndef USER_DATA_H
#define USER_DATA_H



typedef struct
{
    uint64_t *samples_played;
    ADSR *adsr;
    TimeStamp **time_table;
    double actual_time;
    Keys *all_keys;
} ud;

ud * init_ud(uint64_t *samples_played, ADSR * env,  size_t size);
void free_user_data(ud *data, size_t size);
#endif