#include "adsr.h"

// Init the ADSR enveloppe structure
ADSR *init_ADSR_envelope(double attack_to_decay_time,
                         double decay_to_sustain_time,
                         double release_time,
                         double start_amplitude,
                         double attack_top_amplitude,
                         double sustain_amplitude)
{
    ADSR *adsr = malloc(sizeof(ADSR));

    adsr->attack_to_decay_time = attack_to_decay_time;
    adsr->decay_to_sustain_time = decay_to_sustain_time;
    adsr->release_time = release_time;

    adsr->attack_top_amplitude = attack_top_amplitude;
    adsr->start_amplitude = start_amplitude;
    adsr->sustain_amplitude = sustain_amplitude;

    //THOSE TWO VALUE MUST BE PUT INTO THE TIME STAMP LIFECYCKE OF A NOTE
    adsr->release_amplitude = attack_top_amplitude;
    adsr->premature_release = 0;

    return adsr;
}

// Get the amplitude at an instance T
float adsr_get_amplitude(double time, ADSR *envelope, TimeStamp *t_inst)
{
    float signal;
    // The time from the press of the signal
    double actual_signal_time = time - t_inst->press_time;
    // printf("time: %f, actual: %f\n",time,actual_signal_time);
    //  If the time from the press of the signal is still in the AD phase than calculate the amplitude

    if (actual_signal_time <= envelope->attack_to_decay_time + envelope->decay_to_sustain_time)
    {
        // AD

        // if in the Attack phase
        // WORKS
        if (actual_signal_time <= envelope->attack_to_decay_time || t_inst->premature_release)
        {
            if (t_inst->premature_release)
            {
                printf("release before sustain is reached\n");
                signal = (-(envelope->attack_top_amplitude - envelope->start_amplitude) * (actual_signal_time / envelope->attack_to_decay_time)) + 2 * t_inst->release_amp;
                printf("ATTACK Release %f, timestamp: %f\n", signal, actual_signal_time);
                if (signal < 0)
                {
                    return 0;
                }
                return signal;
            }
            else
            {
                // Normalise the constant between 1 and 0 and multiply by attack_top_amplitude
                // Makes an affine function from start amplitude to attack_top_amplitude
                signal = ((envelope->attack_top_amplitude - envelope->start_amplitude) * (actual_signal_time / envelope->attack_to_decay_time)) + envelope->start_amplitude;
                // printf("ATTACK %f, timestamp: %f\n", signal, actual_signal_time);
            }
        }
        // if in the Decay phase
        // Seems to WORK
        else
        {
            if (t_inst->released)
            {
                printf("release before sustain is reached\n");
                return envelope->attack_top_amplitude -
                       ((envelope->attack_top_amplitude - envelope->sustain_amplitude) * 2 *
                        ((actual_signal_time - envelope->attack_to_decay_time) / (envelope->decay_to_sustain_time)));
            }
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
                // printf("DECAY %f, timestamp: %f\n", signal, actual_signal_time);
            }
        }
        return signal;
    }
    else
    {
        // SR
        // Release
        if (t_inst->released)
        {
            // If the time the signal is played is still in the ADSR enveloppe,
            // process the amplitude, if not send out zero
            // printf("actual: %f , Note stop time: %f, Enveleoppe release: %f, Res: %d ",actual_signal_time, t_inst->stop_time ,envelope->release_time,actual_signal_time < t_inst->stop_time + envelope->release_time);
            if (actual_signal_time < t_inst->stop_time + envelope->release_time)
            {
                signal = envelope->sustain_amplitude -
                         ((envelope->sustain_amplitude) *
                          ((actual_signal_time - (t_inst->stop_time - t_inst->press_time)) / (envelope->release_time)));
                // printf("RELEASE %f, timestamp: %f\n", signal, actual_signal_time);
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
        // printf("SUSTAIN %f, timestamp: %f\n", signal, actual_signal_time);
        return signal;
    }
}