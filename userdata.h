#include "adsr.h"

#ifndef USER_DATA_H
#define USER_DATA_H

typedef struct
{
    uint64_t *samples_played;
    ADSR *adsr;
    float freq;
    Uint8 *all_freq;
    double actual_time;
} ud;

ud * init_ud(uint64_t samples_played, ADSR *env);
#endif