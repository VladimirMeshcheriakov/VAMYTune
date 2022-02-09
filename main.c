#include "sdl_call_func.h"

void audio_callback(void *userdata, uint8_t *stream, int len)
{
    ud *us_d = (ud *)userdata;
    uint64_t *samples_played = (us_d->samples_played);
    Keys *all_keys = us_d->all_keys;
    float *fstream = (float *)(stream);
    static const float volume = 0.2;
    for (int sid = 0; sid < (len / 8); ++sid)
    {
        all_keys = us_d->all_keys;
        double time = (*samples_played + sid) / 44100.0;
        us_d->actual_time = time;
        float val = signal_treat(volume, time, all_keys, us_d);
        fstream[2 * sid + 0] = val; /* L */
        fstream[2 * sid + 1] = val; /* R */
    }
    *samples_played += (len / 8);
}

int main(int argc, char *argv[])
{
    // Init the ADSR envelope
    ADSR *env = init_ADSR_envelope(0.2, 0.3, 0.1, 0.0, 1.0, 0.6, 0.0);
    // Data to pass to the audio_callback function
    uint64_t samples_played = 0.;
    size_t num_keys = 13;
    ud *data = init_ud(&samples_played, env, num_keys);
    init_run_app(data,num_keys,audio_callback);
    return 0;
}
