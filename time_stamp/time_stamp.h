#include <stdlib.h>
#include <SDL2/SDL.h>

#ifndef TIMESTAMP_H
#define TIMESTAMP_H

// A time instance for each note
typedef struct
{
    // Is the note released ( 1 = yes, 0 = no)
    int released;
    // The time at which the key was pressed
    double press_time;
    // The time at which the key was released
    double stop_time;
    // Has the key been pressed? ( 1 = yes, 0 = no)
    int press_time_set;
    // Is the sound execution in the release stage ( 1 = yes, 0 = no)
    int release_stage;

} TimeStamp;

// General time management struct 
typedef struct
{
    // The time table for each note
    TimeStamp **time_table;
    // The actual time of the programm
    double actual_time;
    // The size of time_table, (for exemple for 13 notes, we have 13 time instances)
    int size;

} TimeManagement;


void free_time_management(TimeManagement *all_time);
TimeManagement *init_time_management(size_t size);
#endif