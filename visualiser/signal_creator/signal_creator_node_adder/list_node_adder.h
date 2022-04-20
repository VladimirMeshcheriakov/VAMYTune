#include "../signal_creator_utils/signal_creator_utils.h"

#ifndef LIST_NODE_ADDER_H
#define LIST_NODE_ADDER_H


gboolean init_and_create_row_composite(GtkWidget *button, gpointer userdata);
gboolean init_and_create_row(GtkWidget *button, gpointer userdata);
void row_create_composite(__attribute_maybe_unused__  GtkWidget *button, gpointer userdata);
void row_create(__attribute_maybe_unused__  GtkWidget *button, gpointer userdata);


#endif