#include "time_stamp.h"

//Init one time stamp
TimeStamp *init_time_stamp()
{
    TimeStamp *time_instance = malloc(sizeof(TimeStamp));
    time_instance->press_time = 0.0;
    time_instance->press_time_set = 0;
    time_instance->released = 0;
    time_instance->stop_time = 0.0;
    time_instance->release_stage = 0;
    return time_instance;
}
//Init a table of size size, of timestamp
TimeStamp **init_time_table(int size)
{
    TimeStamp **table = malloc(sizeof(TimeStamp*) * size);
    for (size_t i = 0; i < size; i++)
    {
        table[i] = init_time_stamp();
    }
    return table;
}
// Create an instance of time management struct
TimeManagement *  init_time_management(int size)
{
    TimeManagement * all_time = malloc(sizeof(TimeManagement));
    TimeStamp **table = init_time_table(size);
    all_time->time_table = table;
    all_time->actual_time = 0;
    all_time->size = size;
    return all_time;
}

//Free the time table
void free_time_table(TimeStamp **table,int size)
{
    for (size_t i = 0; i < size; i++)
    {
        free(table[i]);
    }
    free(table);
}

//Free the time management struct
void free_time_management(TimeManagement * all_time)
{
    free_time_table(all_time->time_table, all_time->size);
    free(all_time);
}

//Print the time table of size size
void printf_time(TimeStamp **table,int size)
{
    for (size_t i = 0; i < size; i++)
    {
        printf("note: %ld, press: %f, stop: %f |",i,table[i]->press_time,table[i]->stop_time);
        printf("\n");
    }
}