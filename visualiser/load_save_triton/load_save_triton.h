#include "../node/node.h"

#ifndef LOAD_SAVE_TRITON_H
#define LOAD_SAVE_TRITON_H

typedef struct signal_params
{
  node *signals;

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
  float attack_amp;
  float decay_amp;
  float sustain_amp;
  float attack_phase;
  float decay_phase;
  float release_phase;

} signal_params;

signal_params *init_signal_params();
char * create_string_from_signal(sig_info * info);
void find_scopes(char *arr, size_t len, signal_params *params);

void write_to_triton(node * nodes);

#endif 