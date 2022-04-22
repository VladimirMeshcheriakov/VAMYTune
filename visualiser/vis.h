#include <gtk/gtk.h>
#include <math.h>
#include <cairo.h>
#include "../midi_interface_management/midi_management.h"
#include "../signals/signals.h"
#include <SDL2/SDL.h>
#include "filter_visualiser/filter_vis.h"
#include "load_save_triton/load_save_triton.h"
#include "piano_widget/piano_widget.h"


#define N_THREADS 1

#ifndef VIS_H
#define VIS_H



int gtk_run_app(vis_data *vis_d, int argc, char **argv);
float global_signal(float volume,double time, float freq);
float instance_signal(float volume,sig_info *vs, double time, float freq);
float signal_treat(float volume, ud *data);
#endif 