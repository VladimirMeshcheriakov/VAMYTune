#include <gtk/gtk.h>
#include <math.h>
#include <cairo.h>
#include "../userdata/userdata.h"
#include "../midi_interface_management/midi_management.h"
#include "../signals/signals.h"
#include <SDL2/SDL.h>
#include "../FFT/dft.h"
#include "node/node.h"

#define N_THREADS 1

#ifndef VIS_H
#define VIS_H

int gtk_run_zbi(vis_data *vis_d, int argc, char **argv);
float global_signal(float volume,double time, float freq);
float instance_signal(float volume,sig_info *vs, double time, float freq);
float signal_treat(float volume, ud *data);
#endif 