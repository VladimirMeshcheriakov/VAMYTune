#include "audio_callback.h"

// TODO
/*

 ? The buffer size of fstream is 1024, the one passed to sig is 1024 same for harm and filtered, so it would be gread to pass a larger buffer to all those functions so that the visualisation happends slowerz

*/
#define START 0.4
#define STOP 5.7


void audio_callback(void *userdata, uint8_t *stream, int len)
{
    // Get the user-us_d and convert it to the correct type
    vis_data *us_d = (vis_data *)userdata;

    // Get the samples_played
    uint64_t *samples_played = (us_d->data->samples_played);
    // Init the stream
    float *fstream = (float *)(stream);
    // Set the master volume
    static const float volume = 0.1;
    // Play until the sample len has been achieved
    for (int sid = 0; sid < (len / 8); ++sid)
    {
        // Calculate the actual programm run-time
        double time = (*samples_played + sid) / 44100.0;
        // Set the actual time in the us_d
        us_d->data->time_management->actual_time = time;
        // If playback activated

        // Call the function that calculates the signal outhput at the current time and save the signal to the sig
        float val = signal_treat(volume, us_d->data);

        if (us_d->data->wav_manager->playback && read_from_wav(us_d->data)==2)
        {
            val += us_d->data->wav_manager->playback_buffer[0];
        }

        /*
        if (us_d->data->wav_manager->loop)
        {
            if((us_d->data->wav_manager->played_samples)%(int)((STOP-START)*BYTES_PER_SECOND) == 0 )
            {
                read_from_sec(us_d->data->fout,us_d->data->fout_size,START);
            }
            read_from_wav(us_d->data->fout,us_d->data->wav_manager->playback_buffer);
            val += us_d->data->wav_manager->playback_buffer[0];
            us_d->data->wav_manager->played_samples += 8;
        }
        */
        // If recording
        if (us_d->data->wav_manager->record)
        {
            insertArray(us_d->data->fstream, val);
            insertArray(us_d->data->fstream, val);
            us_d->data->wav_manager->recorded_samples += 1;
        }
        us_d->data->sig[sid] = val;
    }
    apply_filter_to_buffer(us_d,1024);
    for (int sid = 0; sid < (len / 8); ++sid)
    {
        fstream[2 * sid + 0] = us_d->data->filtered[sid]; /* L */
        fstream[2 * sid + 1] = us_d->data->filtered[sid]; /* R */
        // printf("time:%f, us_d %f\n",time,val);
    }
    // Increment the number of samples played
    *samples_played += (len / 8);
}