#include <stdlib.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "adsr.h"



ADSR *init_ADSR_envelope(double attack_to_decay_time,
                         double decay_to_sustain_time,
                         double release_time,
                         double start_amplitude,
                         double attack_top_amplitude,
                         double sustain_amplitude,
                         double release_amplitude)
{
    ADSR *adsr = malloc(sizeof(ADSR));

    adsr->attack_to_decay_time = attack_to_decay_time;
    adsr->decay_to_sustain_time = decay_to_sustain_time;
    adsr->release_time = release_time;

    adsr->attack_top_amplitude = attack_top_amplitude;
    adsr->start_amplitude = start_amplitude;
    adsr->sustain_amplitude = sustain_amplitude;
    adsr->release_amplitude = release_amplitude;

    adsr->released = false;

    adsr->press_time = 0.0;
    adsr->stop_time = 0.0;
    adsr->press_time_set = false;
    return adsr;
}


float adsr_get_amplitude(double time, ADSR *envelope)
{
    float signal;
    // The time from the press of the signal
    double actual_signal_time = time - envelope->press_time;
    // If the time from the press of the signal is still in the AD phase than calculate the amplitude
    if (actual_signal_time <= envelope->attack_to_decay_time + envelope->decay_to_sustain_time)
    {
        // AD

        // if in the Attack phase
        // WORKS
        if (actual_signal_time <= envelope->attack_to_decay_time)
        {
            // Normalise the constant between 1 and 0 and multiply by attack_top_amplitude
            // Makes an affine function from start amplitude to attack_top_amplitude
            signal = ((envelope->attack_top_amplitude - envelope->start_amplitude) * (actual_signal_time / envelope->attack_to_decay_time)) + envelope->start_amplitude;
            //printf("ATTACK %f, timestamp: %f\n", signal, actual_signal_time);
        }
        // if in the Decay phase
        // Seems to WORK
        else
        {
            // normalise the constant between 1 and 0 and multiply by sustain_amplitude
            // since we have reached atta_to_amplitude we will now subtract from it
            // we need to get to the sustain_amplitude at the end so we will be
            // normalising (attack_top_amplitude - sustain_amplitude)
            //
            signal = envelope->attack_top_amplitude -
                     ((envelope->attack_top_amplitude - envelope->sustain_amplitude) *
                      ((actual_signal_time - envelope->attack_to_decay_time) / (envelope->decay_to_sustain_time)));
            //printf("DECAY %f, timestamp: %f\n", signal, actual_signal_time);
        }
        return signal;
    }
    else
    {
        // SR
        // Release
        //printf("%d\n",envelope->released);
        if (envelope->released)
        {
            // If the time the signal is played is still in the ADSR enveloppe,
            // process the amplitude, if not send out zero
            if (actual_signal_time < envelope->stop_time + envelope->release_time)
            {
                signal = envelope->sustain_amplitude -
                         ((envelope->sustain_amplitude - envelope->release_amplitude) *
                          ((actual_signal_time - (envelope->stop_time - envelope->press_time)) / (envelope->release_time)));
                printf("RELEASE %f, timestamp: %f\n", signal, actual_signal_time);
            }
            else
            {
                // End the envelope sequence
                signal = 0;
            }
            return signal;
        }
        // Sustain
        signal = envelope->sustain_amplitude;

        printf("SUSTAIN %f, timestamp: %f\n", signal, actual_signal_time);
        return signal;
    }
}