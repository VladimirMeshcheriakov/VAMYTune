#include "time_stamp.h"
#include "adsr.h"

#ifndef USER_DATA_H
#define USER_DATA_H
typedef struct 
{
    Uint8 *keys;
    double *effects;
    size_t size;
}Keys;


typedef struct
{
    uint64_t *samples_played;
    ADSR *adsr;
    TimeStamp **time_table;
    Uint8 *all_freq;
    double actual_time;
    Keys *all_keys;
} ud;


ud * init_ud(uint64_t *samples_played, ADSR *env, TimeStamp **time_table);
#endif