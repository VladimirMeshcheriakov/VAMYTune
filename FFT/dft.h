#include "complex_numbers/complex_number.h"
#include "../userdata/userdata.h"

#ifndef DFT_H
#define DFT_H

typedef struct
{
    ud* data;

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
void filter_cut_around(complex_number *arr, size_t bot, size_t top, size_t size);
void filter_cut_between(complex_number *arr, size_t bot, size_t top,size_t size);
void filter_cut_before(complex_number *arr,  size_t cut, size_t size);
void filter_cut_from(complex_number *arr, size_t size, size_t cut);
void mag_table(complex_number *table, float*mag,  size_t size);
float *ifft(complex_number *buf, float * table, size_t size);
complex_number *fft(float *data, size_t size);

#endif