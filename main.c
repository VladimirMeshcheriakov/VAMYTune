#include "audio_callback/audio_callback.h"

vis_data *init_vis_data(int argc, char *argv[], uint64_t *samples_played, size_t num_keys)
{
    vis_data *vis_d = malloc(sizeof(vis_data));
    memset(vis_d, 0, sizeof(vis_data));
    vis_d->filtered_sample = malloc(sizeof(float) * 1024);
    vis_d->sig_sample = malloc(sizeof(float) * 1024);
    vis_d->harmonics_sample = malloc(sizeof(float) * 1024);
    ud *data = init_ud(samples_played, num_keys);
    vis_d->data = data;
    vis_d->stop_thread = 1;
    vis_d->argc = argc;
    vis_d->argv = argv;
    vis_d->x_zoom = 100;
    vis_d->y_zoom = 100;

    vis_d->attack_amp = 0.0;
    vis_d->decay_amp = 1.0;
    vis_d->sustain_amp = 0.5;
    vis_d->attack_phase = 0.05;
    vis_d->decay_phase = 0.05;
    vis_d->release_phase = 0.05;

    return vis_d;
}

int main(int argc, char *argv[])
{
    // Init the ADSR envelope
    // ADSR *env = init_ADSR_envelope(0.01, 0.01, 0.01, 0.0, 1.0, 0.6, 0.0);
    // Data to pass to the audio_callback function
    uint64_t samples_played = 0.;
    // <Number of keys (usually 127)
    size_t num_keys = 127;
    // Init the data
    vis_data *data = init_vis_data(argc, argv, &samples_played, num_keys);
    // Run the application
    init_run_app(data, audio_callback, argc, argv);

    //free(data->state);
    free(data->filtered_sample);
    free(data->sig_sample);
    free(data->harmonics_sample);
    free(data);
    return 0;
}
