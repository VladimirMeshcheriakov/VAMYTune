#include "time_stamp.h"

TimeStamp *init_time_stamp()
{
    TimeStamp *time_instance = malloc(sizeof(TimeStamp));
    time_instance->press_time = 0.0;
    time_instance->press_time_set = 0;
    time_instance->released = 0;
    time_instance->stop_time = 0.0;
    return time_instance;
}

TimeStamp **init_time_table(int size)
{
    TimeStamp **table = malloc(sizeof(TimeStamp*) * size);
    for (size_t i = 0; i < size; i++)
    {
        table[i] = init_time_stamp();
    }
    return table;
}

void free_time_table(TimeStamp **table,int size)
{
    for (size_t i = 0; i < size; i++)
    {
        free(table[i]);
    }
    free(table);
}