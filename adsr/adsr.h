#include <stdlib.h>
#include <SDL2/SDL.h>
#include "../time_stamp/time_stamp.h"

#ifndef ADSR_H
#define ADSR_H

/*
This class contains the ADSR enveloppe
with all the needed time and amplitude information
*/
typedef struct
{

    double attack_to_decay_time;
    double decay_to_sustain_time;
    double release_time;

    double start_amplitude;
    double attack_top_amplitude;
    double sustain_amplitude;

    double release_amplitude;

    Uint8 premature_release;
} ADSR;


ADSR *init_ADSR_envelope(double attack_to_decay_time,
                         double decay_to_sustain_time,
                         double release_time,
                         double start_amplitude,
                         double attack_top_amplitude,
                         double sustain_amplitude);
float adsr_get_amplitude(double time, ADSR *envelope, TimeStamp *t_inst);


#endif