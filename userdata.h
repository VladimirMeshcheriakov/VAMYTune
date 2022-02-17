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

/**

* TODO: decompose the struct into many struct:

* ! 1. File pointer + file size; into one struct

* * 2. Time_table and actual time; into one struct 

* ! 3. Pull the octave into the side effects, and the side effects into the keys class

* ! 4. All the WAV associated variables into one class

**/




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
    // The current octave of the piano layout
    float octave;
    // The side effects due to ADSR and other enveloppes
    Uint8 *side_effect;
    //Dynamic array representing the data stream
    Array *fstream;
    

    // Buffer including a table of 2 floats to be passed to the read/write function
    float *playback_buffer;
    //Samples recorded, passed to the recoding function
    uint64_t recorded_samples;
    //Samples played from the chosen file
    uint64_t played_samples;
    // 1 to set the record 0 to unset
    int record;
    // 1 to set playback, 0 to unset
    int playback;

    // Pointer to the file from which the data is read
    FILE * fout;
    // The size of the file
    long int fout_size;

} ud;

ud * init_ud(uint64_t *samples_played, ADSR * env,  size_t size);
void free_user_data(ud *data, size_t size);
#endif