#include "../../FFT/dft.h"
#include "../node/node.h"
#include "../../userdata/userdata.h"
#include "../../signals/signals.h"

#ifndef FINAL_SIGNAL_PRODUCTION_H
#define FINAL_SIGNAL_PRODUCTION_H

extern node *nodes;
// The current highest id
extern int global_id;
// The list in which the widgets are stored

float instance_signal(float volume, sig_info *vs, double time, float freq);
float global_signal(float volume, double time, float freq);
void apply_filter_to_sample(vis_data *data, size_t size);
float signal_treat(float volume, ud *data);

#endif 