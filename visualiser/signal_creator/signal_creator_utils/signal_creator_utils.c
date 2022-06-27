#include "signal_creator_utils.h"

//Creates the ewt
void ewt_init(sig_info *info)
{
  for (int sid = 0; sid < 44100; ++sid)
  {
    double time = sid / 44100.0;
    info->ewt[sid] = instance_signal(1.0, info, time, 1.0);
  }
}

id_and_param *create_id_with_param(int *id, int param)
{
  id_and_param *out_param = malloc(sizeof(id_and_param));
  out_param->id = id;
  out_param->param = param;
  return out_param;
}

void free_id_and_param(id_and_param * id_param)
{
  free(id_param);
}

all_params_and_id *init_all_params_id(id_and_param *amp, id_and_param *freq, id_and_param *composite, id_and_param *phase, id_and_param *inverse)
{
  all_params_and_id *all_params = malloc(sizeof(all_params_and_id));
  all_params->amp = amp;
  all_params->freq = freq;
  all_params->composite = composite;
  all_params->phase = phase;
  all_params->inverse = inverse;
  return all_params;
}

all_params_and_id *prepare_all_params(int *id)
{
  printf("%d\n",*id);
  id_and_param *id_amp = create_id_with_param(id, 0);
  id_and_param *id_freq = create_id_with_param(id, 1);
  id_and_param *id_composite = create_id_with_param(id, 2);
  id_and_param *id_phase = create_id_with_param(id, 3);
  id_and_param *id_inverse = create_id_with_param(id, 4);
  all_params_and_id *all_params = init_all_params_id(id_amp, id_freq, id_composite, id_phase, id_inverse);
  return all_params;
}

void free_all_params(all_params_and_id *all_params)
{
  free_id_and_param(all_params->amp);
  free_id_and_param(all_params->freq);
  free_id_and_param(all_params->composite);
  free_id_and_param(all_params->phase);
  free_id_and_param(all_params->inverse);
  free(all_params);
}

void affect_new(GtkWidget *a_scale, float *old_val)
{
  float new_val = gtk_range_get_value(GTK_RANGE(a_scale));
  *old_val = new_val;
  g_print("%f\n", new_val);
}

// If the global frequency of the signal changes
// You must recalculate all sub signals
gboolean on_scale_change_global_freq(GtkWidget *a_scale)
{
  float new_freq = gtk_range_get_value(GTK_RANGE(a_scale));
  global_freq = new_freq;
  node * node_copy = nodes;
  // Recalc for all
  while (node_copy->next)
  {
    node_copy = node_copy->next;
    int cpt = 0;
    for (size_t i = 0; i < 1024; i++)
    {
      double time = cpt / 44100.0;
      node_copy->value->signal[i] = instance_signal(0.5, node_copy->value, time, global_freq);
      cpt += 1;
    }
  }
  global_signal_drawing();
  return G_SOURCE_REMOVE;
}

// This function is called when a change to a parameter has occured
// It is hre that we will recalculate a table of a signal
gboolean on_scale_change_param(GtkWidget *a_scale, gpointer user_data)
{
  id_and_param *id_param = (id_and_param *)user_data;
  // printf("id: %d\n", *id_param->id);
  node *id_node = node_get_at(nodes, *id_param->id);
  sig_info *sine_data = id_node->value;
  float *param;
  switch (id_param->param)
  {
  case 0:
    param = &(sine_data->amp);
    break;
  case 1:
    param = &(sine_data->freq);
    break;
  case 2:
    param = &(sine_data->form);
    break;
  case 3:
    param = &(sine_data->phase);
    break;
  default:
    param = &(sine_data->inverse);
    break;
  }
  if (param != NULL)
  {
    affect_new(a_scale, param);
  }

  // Update the new sig table

  int cpt = 0;
  for (size_t i = 0; i < 1024; i++)
  {
    double time = cpt / 44100.0;
    sine_data->signal[i] = instance_signal(0.5, sine_data, time, global_freq);
    cpt += 1;
  }

  //Update the ewt

  ewt_init(sine_data);
  global_signal_drawing();
  return G_SOURCE_REMOVE;
}

gboolean on_delete_node_params(__attribute_maybe_unused__ GtkWidget *a_button, gpointer user_data)
{
  all_params_and_id *all_params = (all_params_and_id *)user_data;
  int id = *all_params->amp->id;

  GtkListBoxRow *to_delete = gtk_list_box_get_row_at_index(list, id);

  gtk_container_remove(GTK_CONTAINER(list), GTK_WIDGET(to_delete));

  node_lower_id(nodes, id, last_events);

  free_all_params(all_params);

  global_signal_drawing();

  global_id -= 1;

  return G_SOURCE_REMOVE;
}

gboolean on_toggle(GtkWidget *a_toggle_button, gpointer user_data)
{
  int *id = (int *)user_data;
  node *id_node = node_get_at(nodes, *id);
  sig_info *sine_data = id_node->value;
  gboolean state = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(a_toggle_button));
  sine_data->mute = state ? 1 : 0;
  global_signal_drawing();
  return G_SOURCE_REMOVE;
}

/*
The functions that draw the signal
*/
void set_up_axes(GdkWindow *window, GdkRectangle *da, cairo_t *cr, gdouble *clip_x1, gdouble *clip_y1, gdouble *clip_x2, gdouble *clip_y2, gdouble *dx, gdouble *dy, int *zoom_x, int *zoom_y)
{
  /* Determine GtkDrawingArea dimensions */
  gdk_window_get_geometry(window, &da->x, &da->y, &da->width, &da->height);
  /* Draw on a black background */
  cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
  cairo_paint(cr);

  /* Change the transformation matrix */
  // Put the origin of the graph into the center of the image
  cairo_translate(cr, da->width / 2, da->height / 2);
  cairo_scale(cr, *zoom_x, -*zoom_y);
  /* Determine the data points to calculate (ie. those in the clipping zone */
  cairo_device_to_user_distance(cr, dx, dy);
  cairo_clip_extents(cr, clip_x1, clip_y1, clip_x2, clip_y2);
  cairo_set_line_width(cr, *dx);
  /* Draws x and y axis */
  cairo_set_source_rgb(cr, 0.0, 1.0, 0.0);
  cairo_move_to(cr, *clip_x1, 0.0);
  cairo_line_to(cr, *clip_x2, 0.0);
  cairo_move_to(cr, 0.0, *clip_y1);
  cairo_line_to(cr, 0.0, *clip_y2);
  cairo_stroke(cr);
}

// Dynamically draws the signal
gboolean on_draw_created_or_full_signal(GtkWidget *widget, cairo_t *cr, gpointer user_data)
{
  int *id = (int *)user_data;
  float mult = 2.0;
  node *id_node;
  sig_info *vs;
  int zoom_x = 30;
  int zoom_y = 100;
  GdkRectangle da;            /* GtkDrawingArea size */
  gdouble dx = 2.0, dy = 2.0; /* Pixels between each point */
  gdouble i, clip_x1 = 0.0, clip_y1 = 0.0, clip_x2 = 0.0, clip_y2 = 0.0;
  GdkWindow *window = gtk_widget_get_window(widget);
  int drawing_area_width = gtk_widget_get_allocated_width(widget);
  int drawing_area_height = gtk_widget_get_allocated_height(widget);
  set_up_axes(window, &da, cr, &clip_x1, &clip_x2, &clip_y1, &clip_y2, &dx, &dy, &zoom_x, &zoom_y);
  float he = 0;
  int cpt = 0;
  for (i = clip_x1; i < -clip_x1; i += fabs(clip_x1) * 2 / 512)
  {
    if (*id != -1)
    {
      mult = 1.0;
      id_node = node_get_at(nodes, *id);
      vs = id_node->value;
      he = vs->signal[cpt];
    }
    else
    {
      he = nodes->value->signal[cpt];
    }
    cairo_line_to(cr, i, mult * he);
    cpt++;
  }
  cairo_set_source_rgba(cr, 1, 0.2, 0.2, 0.6);
  cairo_stroke(cr);
  gtk_widget_queue_draw_area(widget, 0, 0, drawing_area_width, drawing_area_height);
  return G_SOURCE_REMOVE;
}