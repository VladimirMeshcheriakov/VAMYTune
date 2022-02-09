#include <stdlib.h>
#include <SDL2/SDL.h>

#ifndef TIMESTAMP_H
#define TIMESTAMP_H


//A time instance for each note 
typedef struct
{

    int released;
    double press_time;
    double stop_time;
    int press_time_set;
    int release_stage;
    
} TimeStamp;


TimeStamp **init_time_table(int size);
void free_time_table(TimeStamp **table,int size);
void printf_time(TimeStamp **table,int size);
#endif