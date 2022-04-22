#include "../signal_creator/signal_creator_node_adder/list_node_adder.h"

#ifndef PIANO_WIDGET_H
#define PIANO_WIDGET_H



gboolean on_scale_change_piano(GtkWidget *a_scale, __attribute_maybe_unused__ gpointer user_data);
gboolean on_draw_set_full_keyboard(GtkWidget *widget, cairo_t *cr, __attribute_maybe_unused__ gpointer user_data);
gboolean current_key_click(GtkWidget *event_box, __attribute_maybe_unused__ gpointer user_data);

#endif