#include "filter_vis.h"

// Changes the x zoom
gboolean on_x(GtkWidget *a_spinner, gpointer user_data)
{
  int *x_zoom = (int *)user_data;
  int x = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(a_spinner));
  *x_zoom = x;
  return G_SOURCE_REMOVE;
}

// Changes the y zoom
gboolean on_y(GtkWidget *a_spinner, gpointer user_data)
{
  int *y_zoom = (int *)user_data;
  int y = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(a_spinner));
  *y_zoom = y;
  return G_SOURCE_REMOVE;
}

// Toggles the activation
gboolean on_activate(__attribute_maybe_unused__ GtkWidget *a_check, gpointer user_data)
{
  int *old_state = (int *)user_data;
  // Flip state
  int new_state = *old_state ? 0 : 1;
  *old_state = new_state;
  return G_SOURCE_REMOVE;
}


//Spinner for ADSR
gboolean on_spinner_change(GtkWidget *a_spinner, gpointer user_data)
{
  float *current_time = (float *)user_data;
  float new_time = gtk_spin_button_get_value(GTK_SPIN_BUTTON(a_spinner));
  g_print("%f\n",new_time);
  *current_time = new_time;
  return G_SOURCE_REMOVE;
}

/*

typedef struct
{
    int val;
    int id;
} info;

typedef struct
{
    info *info;
} data;

info *init_info(int val, int id)
{
    info *new = malloc(sizeof(info));
    new->val = val;
    new->id = id;
    return new;
}
void print_info(data * my_data)
{
    printf("val: %d , id: %d\n", my_data->info->val, my_data->info->id);
}

data *my_data = malloc(sizeof(data));
my_data->info = init_info(1, 2);
for (int i = 0; i < 20; i++)
{
    info *tmp = my_data->info;


    info *new = init_info(2, i);
    my_data->info = new;
    free(tmp);
    print_info(my_data);
}
free(my_data->info);
free(my_data);


*/


gboolean on_adsr_change_param(GtkWidget *widget, gpointer user_data)
{
  adsr_vs_and_param * adsr_and_param = (adsr_vs_and_param * ) user_data;
  vis_data * vs = adsr_and_param->vs;
  ADSR ** tmp = &adsr_and_param->vs->data->adsr;
  float new_time = 0;
  float actual_val = 0;
  switch (adsr_and_param->param_index)
  {
  case 0:
    new_time = gtk_spin_button_get_value(GTK_SPIN_BUTTON(widget));
    vs->attack_phase = new_time;
    break;
  case 1:
    new_time = gtk_spin_button_get_value(GTK_SPIN_BUTTON(widget));
    vs->decay_phase = new_time;
    break;
  case 2:
    new_time = gtk_spin_button_get_value(GTK_SPIN_BUTTON(widget));
    vs->release_phase = new_time;
    break;
  case 3:
    actual_val = gtk_range_get_value(GTK_RANGE(widget));
    vs->attack_amp = actual_val;
    break;
  case 4:
    actual_val = gtk_range_get_value(GTK_RANGE(widget));
    vs->decay_amp = actual_val;
    break;
  default:
    actual_val = gtk_range_get_value(GTK_RANGE(widget));
    vs->sustain_amp = actual_val;
    break;
  }
  printf("change param\n");
  free(*tmp);
  ADSR * new_adsr = init_ADSR_envelope(vs->attack_phase,vs->decay_phase,vs->release_phase,vs->attack_amp,vs->decay_amp,vs->sustain_amp);
  adsr_and_param->vs->data->adsr = new_adsr;
  return G_SOURCE_REMOVE;
}



// Scale move (normal)
gboolean on_scale_change(GtkWidget *a_scale, gpointer user_data)
{
  float *old_val = (float *)user_data;
  float actual_val = gtk_range_get_value(GTK_RANGE(a_scale));
  g_print("%f\n",actual_val);
  *old_val = actual_val;
  return G_SOURCE_REMOVE;
}

// Scale move on the low side of the band filters
gboolean on_scale_band_cut_change_low(GtkWidget *low_scale, gpointer user_data)
{
  GtkMultipleScales *data = (GtkMultipleScales *)user_data;

  float *low_cut = &(data->data->band_cut_low);
  float *high_cut = &(data->data->band_cut_high);

  GtkScale *high = GTK_SCALE(data->lead);
  GtkScale *low = GTK_SCALE(low_scale);

  gdouble low_val = gtk_range_get_value(GTK_RANGE(low));
  *low_cut = low_val;

  gdouble high_val = gtk_range_get_value(GTK_RANGE(high));
  if (high_val < low_val)
  {
    *high_cut = low_val;
    gtk_range_set_value(GTK_RANGE(high), low_val);
  }
  return G_SOURCE_REMOVE;
}

// Scale move on the high side of the band filters
gboolean on_scale_band_cut_change_high(GtkWidget *high_scale, gpointer user_data)
{
  GtkMultipleScales *data = (GtkMultipleScales *)user_data;

  float *low_cut = &(data->data->band_cut_low);
  float *high_cut = &(data->data->band_cut_high);

  GtkScale *low = GTK_SCALE(data->lead);
  GtkScale *high = GTK_SCALE(high_scale);

  gdouble low_val = gtk_range_get_value(GTK_RANGE(low));

  gdouble high_val = gtk_range_get_value(GTK_RANGE(high));
  *high_cut = high_val;

  if (high_val < low_val)
  {
    *low_cut = high_val;
    gtk_range_set_value(GTK_RANGE(low), high_val);
  }
  return G_SOURCE_REMOVE;
}

// Scale move on the low side of the band filters
gboolean on_scale_band_change_low(GtkWidget *low_scale, gpointer user_data)
{
  GtkMultipleScales *data = (GtkMultipleScales *)user_data;

  float *low_cut = &(data->data->band_pass_low);
  float *high_cut = &(data->data->band_pass_high);

  GtkScale *high = GTK_SCALE(data->lead);
  GtkScale *low = GTK_SCALE(low_scale);

  gdouble low_val = gtk_range_get_value(GTK_RANGE(low));
  *low_cut = low_val;

  gdouble high_val = gtk_range_get_value(GTK_RANGE(high));
  if (high_val < low_val)
  {
    *high_cut = low_val;
    gtk_range_set_value(GTK_RANGE(high), low_val);
  }
  return G_SOURCE_REMOVE;
}

// Scale move on the high side of the band filters
gboolean on_scale_band_change_high(GtkWidget *high_scale, gpointer user_data)
{
  GtkMultipleScales *data = (GtkMultipleScales *)user_data;

  float *low_cut = &(data->data->band_pass_low);
  float *high_cut = &(data->data->band_pass_high);

  GtkScale *low = GTK_SCALE(data->lead);
  GtkScale *high = GTK_SCALE(high_scale);

  gdouble low_val = gtk_range_get_value(GTK_RANGE(low));

  gdouble high_val = gtk_range_get_value(GTK_RANGE(high));
  *high_cut = high_val;

  if (high_val < low_val)
  {
    *low_cut = high_val;
    gtk_range_set_value(GTK_RANGE(low), high_val);
  }
  return G_SOURCE_REMOVE;
}

/*



Filters Drawing

*/

// Dynamically draws the harmonics
gboolean on_draw_harmonics(GtkWidget *widget, cairo_t *cr, gpointer user_data)
{
  vis_data *vs = (vis_data *)user_data;
  float *us = vs->harmonics_sample;

  GdkRectangle da_parameters; /* GtkDrawingArea size */
  double dx = 2.0, dy = 2.0;  /* Pixels between each point */
  double i, clip_x1 = 0.0, clip_y1 = 0.0, clip_x2 = 0.0, clip_y2 = 0.0;

  GdkWindow *window = gtk_widget_get_window(widget);

  /* Determine GtkDrawingArea dimensions */
  gdk_window_get_geometry(window,
                          &da_parameters.x,
                          &da_parameters.y,
                          &da_parameters.width,
                          &da_parameters.height);

  /* Draw on a black background */
  cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
  cairo_paint(cr);

  /* Change the transformation matrix */
  // Put the origin of the graph into the center of the image
  cairo_translate(cr, da_parameters.width / 2, da_parameters.height);
  cairo_scale(cr, 100, -100);
  /* Determine the data points to calculate (ie. those in the clipping zone */
  cairo_device_to_user_distance(cr, &dx, &dy);
  cairo_clip_extents(cr, &clip_x1, &clip_y1, &clip_x2, &clip_y2);
  cairo_set_line_width(cr, dx);

  cairo_set_source_rgb(cr, 0.0, 1.0, 0.0);
  cairo_move_to(cr, clip_x1, 0.0);
  cairo_line_to(cr, clip_x2, 0.0);
  cairo_stroke(cr);

  // printf("exec x1 %f , x2 %f, dx %f\n", clip_x1, clip_x2, ((fabs(clip_x1) - fabs(clip_x2) ) / (double)1024));
  //  printf("exec y1 %f , y2 %f, dy %f\n", clip_y1, clip_y2, dy);
  /* Link each data point */
  int cpt = 0;

  for (i = clip_x1; i < clip_x2; i += (clip_x2 / (double)512))
  {
    if (cpt < 1024)
    {
      float he = us[cpt];
      cairo_line_to(cr, i, he * clip_y2);
    }
    cpt += 1;
  }
  // printf("cpr:%d\n",cpt);
  /* Draw the curve */
  cairo_set_source_rgba(cr, 1, 0.2, 0.2, 0.6);
  cairo_stroke(cr);
  // printf("da_h %d , da_w %d\n",drawing_area_height,drawing_area_width);

  gtk_widget_queue_draw_area(widget, 0, 0, da_parameters.width, da_parameters.height);

  return G_SOURCE_REMOVE;
}

// Dynamically draws the signal
gboolean on_draw_signal(GtkWidget *widget, cairo_t *cr, gpointer user_data)
{

  vis_data *vs = (vis_data *)user_data;
  float *us = vs->sig_sample;
  float *resp = vs->filtered_sample;
  int zoom_x = vs->x_zoom;
  int zoom_y = vs->y_zoom;

  GdkRectangle da_parameters; /* GtkDrawingArea size */
  gdouble dx = 2.0, dy = 2.0; /* Pixels between each point */
  gdouble i, clip_x1 = 0.0, clip_y1 = 0.0, clip_x2 = 0.0, clip_y2 = 0.0;

  GdkWindow *window = gtk_widget_get_window(widget);

  /* Determine GtkDrawingArea dimensions */
  gdk_window_get_geometry(window,
                          &da_parameters.x,
                          &da_parameters.y,
                          &da_parameters.width,
                          &da_parameters.height);

  /* Draw on a black background */
  cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
  cairo_paint(cr);

  /* Change the transformation matrix */
  // Put the origin of the graph into the center of the image
  cairo_translate(cr, da_parameters.width / 2, da_parameters.height / 2);
  cairo_scale(cr, zoom_x, -zoom_y);

  /* Determine the data points to calculate (ie. those in the clipping zone */
  cairo_device_to_user_distance(cr, &dx, &dy);
  cairo_clip_extents(cr, &clip_x1, &clip_y1, &clip_x2, &clip_y2);
  cairo_set_line_width(cr, dx);

  /* Draws x and y axis */
  cairo_set_source_rgb(cr, 0.0, 1.0, 0.0);
  cairo_move_to(cr, clip_x1, 0.0);
  cairo_line_to(cr, clip_x2, 0.0);
  cairo_move_to(cr, 0.0, clip_y1);
  cairo_line_to(cr, 0.0, clip_y2);
  cairo_stroke(cr);

  // printf("exec x1 %f , x2 %f, dx %f\n", clip_x1, clip_x2, dx);
  // printf("exec y1 %f , y2 %f, dy %f\n", clip_y1, clip_y2, dy);
  /* Link each data point */
  int cpt = 0;
  for (i = clip_x1; i < clip_x2; i += (clip_x2 / (double)512))
  {
    if (cpt < 1024)
    {
      float he = us[cpt];
      // printf("double %f\n",i);
      cairo_line_to(cr, i, he);
    }
    cpt += 1;
  }

  /* Draw the curve */
  cairo_set_source_rgba(cr, 1, 0.2, 0.2, 0.6);
  cairo_stroke(cr);

  cpt = 0;
  for (i = clip_x1; i < clip_x2; i += (clip_x2 / (double)512))
  {
    if (cpt < 1024)
    {
      float he = resp[cpt];
      // printf("double %f\n", he);
      cairo_line_to(cr, i, he);
    }
    cpt += 1;
  }
  cairo_set_source_rgba(cr, 0.2, 0.6, 0.6, 0.7);
  cairo_stroke(cr);

  gtk_widget_queue_draw_area(widget, 0, 0, da_parameters.width, da_parameters.height);

  return G_SOURCE_REMOVE;
}
