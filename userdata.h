#include "time_stamp.h"
#include "adsr.h"
#include "keys.h"
#include "dynamic_array.h"
#include "wav_management.h"

#ifndef USER_DATA_H
#define USER_DATA_H
/*
User data structure
contains all the information that is present in the programm
for the sound execution
*/
typedef struct
{
    //Samples played by the programm
    uint64_t *samples_played;
    
    //General TimeManagement class
    TimeManagement * time_management;

    //The ADSR enveloppe
    ADSR *adsr;

    // Keys pressed and effects applied to those keys
    Keys *all_keys;

    // The side effects managing other buttons being pressed
    Uint8 *side_effect;

    //Dynamic array representing the data stream
    Array *fstream;

    //Manageement of record and playback functionality
    WavManagement * wav_manager;

    // Pointer to the file from which the data is read
    FILE * fout;
    // The size of the file
    uint64_t fout_size;

} ud;

ud * init_ud(uint64_t *samples_played, ADSR * env,  size_t size);
void free_user_data(ud *data);
#endif