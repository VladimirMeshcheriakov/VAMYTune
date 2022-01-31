#include <stdlib.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

#ifndef TIMESTAMP_H
#define TIMESTAMP_H

typedef struct
{

    bool released;
    double press_time;
    double stop_time;
    bool press_time_set;

} TimeStamp;

TimeStamp *init_time_stamp();
TimeStamp **init_time_table(int size);
void free_time_table(TimeStamp **table,int size);
#endif