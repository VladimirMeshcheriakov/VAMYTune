#include "audio_callback.h"


//TODO
/*

 ? The buffer size of fstream is 2048, the one passed to sig is 1024 same for harm and filtered, so it would be gread to pass a larger buffer to all those functions so that the visualisation happends slowerz       

*/


void audio_callback(void *userdata, uint8_t *stream, int len)
{
    //Get the user-data and convert it to the correct type
    ud *us_d = (ud *)userdata;
    
    //Get the samples_played
    uint64_t *samples_played = (us_d->samples_played);
    //Init the stream
    float *fstream = (float *)(stream);
    //Set the master volume
    static const float volume = 0.1;
    //Play until the sample len has been achieved 
    for (int sid = 0; sid < (len / 8); ++sid)
    {
        float val = 0;
        //Calculate the actual programm run-time
        double time = (*samples_played + sid) / 44100.0;
        //Set the actual time in the data
        us_d->time_management->actual_time = time;

        //Call the function that calculates the signal outhput at the current time and save the signal to the sig
        val += signal_treat(volume,us_d,sid);

        //If recording
        if(us_d->wav_manager->record)
        {
            insertArray(us_d->fstream,val);
            insertArray(us_d->fstream,val);
            us_d->wav_manager->recorded_samples +=1;
        }

        //Put the signal value into the stream
        fstream[2 * sid + 0] = val; /* L */
        fstream[2 * sid + 1] = val; /* R */
        //printf("time:%f, data %f\n",time,val);
    }
    //Increment the number of samples played
    *samples_played += (len / 8);
}