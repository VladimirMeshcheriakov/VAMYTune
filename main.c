#include "sdl_call_func.h"

void audio_callback(void *userdata, uint8_t *stream, int len)
{
    //Get the user-data and convert it to the correct type
    ud *us_d = (ud *)userdata;
    //Get the samples_played
    uint64_t *samples_played = (us_d->samples_played);
    //Init the stream
    float *fstream = (float *)(stream);
    //Set the master volume
    static const float volume = 0.2;
    //Play until the sample len has been achieved 
    for (int sid = 0; sid < (len / 8); ++sid)
    {
        //Calculate the actual programm run-time
        double time = (*samples_played + sid) / 44100.0;
        //Set the actual time in the data
        us_d->actual_time = time;
        //Call the function that calculates the signal outhput at the current time
        float val = signal_treat(volume,us_d);
        //Put the signal value into the stream
        fstream[2 * sid + 0] = val; /* L */
        fstream[2 * sid + 1] = val; /* R */
    }
    //Increment the number of samples played
    *samples_played += (len / 8);
}

int main(int argc, char *argv[])
{
    // Init the ADSR envelope
    ADSR *env = init_ADSR_envelope(0.1, 0.1, 0.05, 0.0, 1.0, 0.8, 0.0);
    // Data to pass to the audio_callback function
    uint64_t samples_played = 0.;
    // <Number of keys (usually 13)>
    size_t num_keys = 13;
    //Init the data
    ud *data = init_ud(&samples_played, env, num_keys);
    //Run the application
    init_run_app(data,num_keys,audio_callback);
    return 0;
}
