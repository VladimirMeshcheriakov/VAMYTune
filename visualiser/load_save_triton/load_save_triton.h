#include "../signal_creator/signal_creator_node_adder/list_node_adder.h"

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

gboolean update_preview_cb(GtkFileChooser *file_chooser, gpointer data);
int load_from_triton(const char *uri, int uri_or_path);
void write_to_triton(node *nodes, const char *name);
gboolean on_save_state(__attribute_maybe_unused__ GtkButton *a_button);
gboolean update_new_wav_file(GtkFileChooser *file_chooser, __attribute_maybe_unused__ gpointer data);

#endif 