#include <cairo.h>
#include "../signals/signals.h"

#include "midi_record_vis/midi_record_vis.h"
#include "midi_device_chooser/midi_device_chooser.h"
#include "filter_visualiser/filter_vis.h"
#include "load_save_triton/load_save_triton.h"
#include "last_session/last_session.h"
#include "piano_widget/piano_widget.h"


#define N_THREADS 1

#ifndef VIS_H
#define VIS_H



int gtk_run_app(vis_data *vis_d, int argc, char **argv);
float global_signal(float volume, float freq);
float instance_signal(float volume,sig_info *vs, double time, float freq);
float signal_treat(float volume, ud *data);
#endif 