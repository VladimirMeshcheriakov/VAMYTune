#include <gtk/gtk.h>
#include <math.h>
#include "../../node/node.h"
#include "../../final_signal_production/final_signal_production.h"


#ifndef SIGNAL_CREATOR_UTILS_H
#define SIGNAL_CREATOR_UTILS_H

extern node *nodes;
extern last_events_stack * last_events;
// The current highest id
extern int global_id;
extern GMainContext *context;
extern char global_file_name[];
extern char global_file_name_components[];
extern GtkListBox *list;
// The global frequency controller
extern float global_freq;

// Struct to pass node id and the param to modify
typedef struct id_and_param
{
  int *id;
  int param;
} id_and_param;



typedef struct all_params_and_id
{
  struct id_and_param *amp;
  struct id_and_param *freq;
  struct id_and_param *composite;
  struct id_and_param *phase;
  struct id_and_param *inverse;
} all_params_and_id;

all_params_and_id *init_all_params_id(id_and_param *amp, id_and_param *freq, id_and_param *composite, id_and_param *phase, id_and_param *inverse);
all_params_and_id *prepare_all_params(int *id);
void free_all_params(all_params_and_id *all_params);
gboolean on_scale_change_global_freq(GtkWidget *a_scale);
gboolean on_scale_change_param(GtkWidget *a_scale, gpointer user_data);
gboolean on_delete_node_params(__attribute_maybe_unused__ GtkWidget *a_button, gpointer user_data);
gboolean on_toggle(GtkWidget *a_toggle_button, gpointer user_data);
gboolean on_draw_created_or_full_signal(GtkWidget *widget, cairo_t *cr, gpointer user_data);

#endif