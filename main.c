#include "audio_callback/audio_callback.h"

Uint8 *init_state_midi_keyboard(size_t size)
{
    Uint8 *state = calloc(size, sizeof(Uint8));
    return state;
}

vis_data * init_vis_data(int argc, char *argv[], uint64_t *samples_played, ADSR *env,size_t num_keys )
{
    vis_data * vis_d = malloc(sizeof(vis_data));
    memset(vis_d,0,sizeof(vis_data));
    ud *data = init_ud(samples_played, env, num_keys);
    vis_d->data = data;
    Uint8 *state = init_state_midi_keyboard(num_keys);
    vis_d->state = state;
    vis_d->stop_thread = 1;
    vis_d->argc = argc;
    vis_d->argv = argv;
    vis_d->x_zoom = 300;
    vis_d->y_zoom = 300;
    return vis_d;
}

int main(int argc, char *argv[])
{
    // Init the ADSR envelope
    ADSR *env = init_ADSR_envelope(0.01, 0.01, 0.01, 0.0, 1.0, 0.6, 0.0);
    // Data to pass to the audio_callback function
    uint64_t samples_played = 0.;
    // <Number of keys (usually 127)
    size_t num_keys = 127;
    //Init the data
    vis_data *data = init_vis_data(argc,argv,&samples_played,env,num_keys);
    //Run the application
    init_run_app(data,audio_callback,argc,argv);
    free(data->state);
    free(data);
    return 0;
}



