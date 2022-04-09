#include "complex_numbers/complex_number.h"
#include "../userdata/userdata.h"

#ifndef DFT_H
#define DFT_H

typedef struct
{
    ud* data;
    Uint8 *state;

    int stop_thread;

    int argc;
    char **argv;

    int x_zoom;
    int y_zoom;

    int low_active;
    int high_active;
    int band_pass_active;
    int band_cut_active;

    float low_pass_cut;
    float high_pass_cut;

    float band_pass_low;
    float band_pass_high;

    float band_cut_low;
    float band_cut_high;

    float * sig_sample;
    float * filtered_sample;
    float * harmonics_sample;


    float attack_amp;
    float decay_amp;
    float sustain_amp;

    float attack_phase;
    float decay_phase;
    float release_phase;

}vis_data;

void apply_filter_to_buffer(vis_data *data, size_t size);
void apply_filter_to_sample(vis_data *data, size_t size);

#endif