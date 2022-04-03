#include "audio_callback/audio_callback.h"

int main(int argc, char *argv[])
{
    // Init the ADSR envelope
    ADSR *env = init_ADSR_envelope(0.01, 0.01, 0.01, 0.0, 1.0, 0.6, 0.0);
    // Data to pass to the audio_callback function
    uint64_t samples_played = 0.;
    // <Number of keys (usually 127)
    size_t num_keys = 127;
    //Init the data
    ud *data = init_ud(&samples_played, env, num_keys);
    //Run the application
    init_run_app(data,audio_callback,argc,argv);
    return 0;
}



