#include "vis.h"

GMainContext *context;
char global_file_name[] = "visualiser/glade_signals/signal.glade";
char global_file_name_components[] = "visualiser/glade_signals/signal_components.glade";

// The nodes of the different signals
node *nodes;

// The current highest id
int global_id = 0;

// The list in which the widgets are stored
GtkListBox *list;

// The global frequency controller
float global_freq = 0.0;

// Struct to manage follow up lifting of parameters
typedef struct
{
  GtkScale *lead;
  vis_data *data;
} GtkMultipleScales;

// Stops the main thread, quits the gtk
void on_destroy(__attribute_maybe_unused__ GtkWidget *Widget, gpointer user_data)
{
  int *running = (int *)user_data;
  *running = 0;
  gtk_main_quit();
}

// Key_change
static gboolean key_released(__attribute_maybe_unused__ GtkWidget *widget, GdkEventKey *event, gpointer user_data)
{
  ud *data = (ud *)user_data;
  if (event->keyval == GDK_KEY_r)
  {
    // Record
    data->wav_manager->record = 1;
  }
  else if (event->keyval == GDK_KEY_s)
  {
    // Stop record
    data->wav_manager->record = 0;
  }
  else if (event->keyval == GDK_KEY_p)
  {
    data->wav_manager->playback = 1;
  }
  else if (event->keyval == GDK_KEY_o)
  {
    data->wav_manager->playback = 0;
  }
  return GDK_EVENT_PROPAGATE;
}

// Changes the x zoom
static gboolean
on_x(GtkWidget *a_spinner, gpointer user_data)
{
  int *x_zoom = (int *)user_data;
  int x = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(a_spinner));
  *x_zoom = x;
  return G_SOURCE_REMOVE;
}

// Changes the y zoom
static gboolean
on_y(GtkWidget *a_spinner, gpointer user_data)
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

gboolean on_spinner_change(GtkWidget *a_spinner, gpointer user_data)
{
  float *current_time = (float *)user_data;
  float new_time = gtk_spin_button_get_value(GTK_SPIN_BUTTON(a_spinner));
  // g_print("%f\n",new_time);
  *current_time = new_time;
  return G_SOURCE_REMOVE;
}

// Scale move (normal)
gboolean on_scale_change(GtkWidget *a_scale, gpointer user_data)
{
  float *old_val = (float *)user_data;
  float actual_val = gtk_range_get_value(GTK_RANGE(a_scale));
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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////:

gboolean on_adsr_draw(GtkWidget *widget, cairo_t *cr, gpointer user_data)
{

  vis_data *vs = (vis_data *)user_data;
  int zoom_x = 100;
  int zoom_y = 100;

  GdkRectangle da;            /* GtkDrawingArea size */
  gdouble dx = 2.0, dy = 2.0; /* Pixels between each point */
  gdouble clip_x1 = 0.0, clip_y1 = 0.0, clip_x2 = 0.0, clip_y2 = 0.0;

  GdkWindow *window = gtk_widget_get_window(widget);
  int drawing_area_width = gtk_widget_get_allocated_width(widget);
  int drawing_area_height = gtk_widget_get_allocated_height(widget);

  /* Determine GtkDrawingArea dimensions */
  gdk_window_get_geometry(window,
                          &da.x,
                          &da.y,
                          &da.width,
                          &da.height);

  /* Draw on a black background */
  cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
  cairo_paint(cr);

  /* Change the transformation matrix */
  // Put the origin of the graph into the botom left corner
  cairo_translate(cr, 0, da.height);
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

  float cord_table[] = {vs->attack_amp, vs->decay_amp, vs->sustain_amp, vs->sustain_amp, 0.0};

  float sust_phase = (vs->attack_phase + vs->decay_phase + vs->release_phase) / 3.0;

  float sig_sum = vs->attack_phase + vs->decay_phase + vs->release_phase + sust_phase;

  float factor = clip_x2 / sig_sum;

  int cpt = 0;

  float y_tab[] = {0.0, vs->attack_phase, vs->decay_phase + vs->attack_phase, sust_phase + vs->decay_phase + vs->attack_phase, vs->release_phase + sust_phase + vs->decay_phase + vs->attack_phase};

  while (cpt < 5)
  {
    cairo_line_to(cr, y_tab[cpt] * factor, clip_y2 * cord_table[cpt]);
    cpt += 1;
  }

  /* Draw the curve */
  cairo_set_source_rgba(cr, 1, 0.2, 0.2, 0.6);
  cairo_stroke(cr);
  // printf("da_h %d , da_w %d\n",drawing_area_height,drawing_area_width);

  gtk_widget_queue_draw_area(widget, 0, 0, drawing_area_width, drawing_area_height);

  return G_SOURCE_REMOVE;
}

// Main loop of the app
void run_app(vis_data *my_data)
{
  int argc = my_data->argc;
  char **argv = my_data->argv;
  ud *data = my_data->data;
  Uint8 *state = my_data->state;

  data->fout = open_WAV("Bach.wav");
  data->fout_size = findSize("Bach.wav");

  struct pollfd *pfds;
  int npfds;

  init_seq();

  if (parse_input(argc, argv) != -1)
  {
    printf("Parsing error\n");
  }

  snd_seq_t *seq = create_port();
  int port_count = connect_ports(seq);

  if (port_count > 0)
    printf("Waiting for data.");
  else
    printf("Waiting for data at port %d:0.",
           snd_seq_client_id(seq));
  printf(" Press Ctrl+C to end.\n");
  printf("Source  Event                  Ch  Data\n");

  npfds = snd_seq_poll_descriptors_count(seq, POLLIN);
  pfds = alloca(sizeof(*pfds) * npfds);

  while (my_data->stop_thread)
  {
    snd_seq_poll_descriptors(seq, pfds, npfds, POLLIN);
    int p = poll(pfds, npfds, 20);
    if (p == 0)
    {
      note_state(state, data);
      init_piano_keys(state, data);
    }
    else
    {
      snd_seq_event_t *event_;
      snd_seq_event_input(seq, &event_);
      if (event_)
      {
        dump_event(event_, state);
        note_state(state, data);
        init_piano_keys(state, data);
      }
    }
    // The graphic calculus slows the programm!!!
    apply_filter_to_sample(my_data, 1024);
  }
}

// Thread Function to run the main app
static gpointer
thread_func(gpointer user_data)
{
  vis_data *vs = (vis_data *)user_data;
  g_print("Starting thread %d\n", 1);
  while (vs->stop_thread)
  {
    run_app(vs);
  }
  g_print("Ending thread %d\n", 1);
  return NULL;
}

// Thread function to update the adsr effects
static gpointer
thread_update(gpointer user_data)
{
  vis_data *vs = (vis_data *)user_data;
  g_print("Starting thread %d\n", 1);
  while (vs->stop_thread)
  {
    update_effects(vs);
  }
  g_print("Ending thread %d\n", 1);
  return NULL;
}

int x;
int y;
int octave_number;

// returns 1 is the (currentx,current_y) is in the rectangle else 0
int is_in_rectangle(int current_x, int current_y, int rect_top_x, int rect_top_y, int rect_width, int rect_height)
{
  return (current_x < rect_top_x + rect_width && current_y < rect_top_y + rect_height && current_x > rect_top_x && current_y > rect_top_y) ? 1 : 0;
}

// Sets the new octave value
static gboolean
on_scale_change_piano(GtkWidget *a_scale, __attribute_maybe_unused__ gpointer user_data)
{
  int new_size = gtk_range_get_value(GTK_RANGE(a_scale));
  // g_print("id: %d\n", id);
  octave_number = new_size;
  return G_SOURCE_REMOVE;
}

// Gets the current event and sets the x,y possition
static gboolean
current_key_click(GtkWidget *event_box, __attribute_maybe_unused__ gpointer user_data)
{
  Uint8 * keyboard = (Uint8 *)user_data;
  GdkEvent *event = gtk_get_current_event();
  GdkDisplay *display = gdk_display_get_default();
  GdkSeat *seat = gdk_display_get_default_seat(display);
  GdkDevice *device = gdk_seat_get_pointer(seat);

  if (gdk_event_get_event_type(event) == GDK_BUTTON_PRESS)
  {
    gdk_window_get_device_position(gtk_widget_get_window(GTK_WIDGET(event_box)), device, &x, &y, NULL);
  }
  if (gdk_event_get_event_type(event) == GDK_BUTTON_RELEASE)
  {
    x = -1;
    y = -1;
  }
  gdk_event_free(event);
  return G_SOURCE_REMOVE;
}

// General axes set_up
void set_up_axes_for_piano(GdkWindow *window, GdkRectangle *da, cairo_t *cr, gdouble *clip_x1, gdouble *clip_y1, gdouble *clip_x2, gdouble *clip_y2, gdouble *dx, gdouble *dy)
{
  gdk_window_get_geometry(window, &da->x, &da->y, &da->width, &da->height);
  // Draw white background
  cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
  cairo_paint(cr);
  cairo_device_to_user_distance(cr, dx, dy);
  cairo_clip_extents(cr, clip_x1, clip_y1, clip_x2, clip_y2);
  cairo_set_line_width(cr, *dx);
}

// Draws all the lines in one octave
static gboolean
on_draw_key_lines(cairo_t *cr, int drawing_area_width, int drawing_area_height, int num_octaves)
{
  int num_keys = 7 * num_octaves;
  for (int o = 0; o < num_octaves; o++)
  {
    for (size_t j = 0; j <= 7; j++)
    {
      int i = j + o * 7;
      if (j == 7 || j == 3)
      {
        cairo_line_to(cr, drawing_area_width * i / num_keys, 0);
      }
      else
      {
        cairo_line_to(cr, drawing_area_width * i / num_keys, drawing_area_height * 3 / 5);
      }
      cairo_line_to(cr, drawing_area_width * i / num_keys, drawing_area_height);
      cairo_set_source_rgb(cr, 0, 0, 0);
      cairo_stroke(cr);
    }
  }
  return G_SOURCE_REMOVE;
}

// Draws one black key

int on_draw_black_key(cairo_t *cr, int drawing_area_width, int drawing_area_height, int num_keys, int j, int id)
{
  int present = 0;
  cairo_set_source_rgb(cr, 0, 0, 0);

  int top_left_x = drawing_area_width * j / (num_keys * 4);
  int top_right_x = drawing_area_width * (2 + j) / (num_keys * 4);
  int bot_right_y = drawing_area_height * 3 / 5;

  cairo_line_to(cr, top_left_x, 0);
  cairo_line_to(cr, top_right_x, 0);
  cairo_line_to(cr, top_right_x, bot_right_y);
  cairo_line_to(cr, top_left_x, bot_right_y);
  cairo_line_to(cr, top_left_x, 0);

  if (is_in_rectangle(x, y, top_left_x, 0, top_right_x - top_left_x, bot_right_y))
  {
    cairo_set_source_rgb(cr, 0.5, 0.5, 0.5);
    printf("id: %d\n", id);
    present = 1;
  }
  cairo_fill(cr);
  return present;
}
// Draws one left type white key

int on_draw_left_type_white_key(cairo_t *cr, int drawing_area_width, int drawing_area_height, int num_keys, int j, int id)
{
  int present = 0;
  // Default color if not pressed
  cairo_set_source_rgb(cr, 1, 1, 1);

  // The origin from which the tracing starts
  int origin = drawing_area_width / (num_keys / 7) * j / 7;

  // parameters of the top rectangle
  int top_rect_width = drawing_area_width * 3 / (num_keys * 4);
  int top_rect_height = drawing_area_height * 3 / 5;

  // parametes of the bottom rectangle
  int bot_rect_width = drawing_area_width / num_keys;
  int bot_rect_height = drawing_area_height * 2 / 5;

  // Draw the top part
  cairo_line_to(cr, origin, 0);
  cairo_line_to(cr, top_rect_width + origin, 0);
  cairo_line_to(cr, top_rect_width + origin, top_rect_height);

  // Check if the key is pressed on the top part
  if (is_in_rectangle(x, y, origin, 0, top_rect_width, top_rect_height))
  {
    cairo_set_source_rgb(cr, 0.5, 0.5, 0.5);
    printf("id: %d\n", id);
    present = 1;
  }

  // Draw the bottom part
  cairo_line_to(cr, bot_rect_width + origin, top_rect_height);
  cairo_line_to(cr, bot_rect_width + origin, drawing_area_height);
  cairo_line_to(cr, origin, drawing_area_height);
  cairo_line_to(cr, origin, 0);

  // Check if the key is pressed on the bottom part
  if (is_in_rectangle(x, y, origin, top_rect_height, bot_rect_width, bot_rect_height))
  {
    cairo_set_source_rgb(cr, 0.5, 0.5, 0.5);
    present = 1;
    printf("id: %d\n", id);
  }

  // Draw the rectangle
  cairo_fill(cr);
  return present;
}

// Draws one center type white key
int on_draw_center_type_white_key(cairo_t *cr, int drawing_area_width, int drawing_area_height, int num_keys, int j, int id)
{
  int present = 0;
  // Default color if not pressed
  cairo_set_source_rgb(cr, 1, 1, 1);
  // The tracing origin
  int origin = drawing_area_width / (num_keys / 7) * j / 7 + drawing_area_width / (num_keys * 4);

  // Top rectangle parameters
  int top_rect_width = drawing_area_width / (num_keys * 2);
  int top_rect_height = drawing_area_height * 3 / 5;

  // parametes of the bottom rectangle
  int bot_rect_width = origin - drawing_area_width / (num_keys * 4);
  int bot_rect_height = drawing_area_height * 2 / 5;

  // Trace the top part
  cairo_line_to(cr, origin, 0);
  cairo_line_to(cr, origin + top_rect_width, 0);
  cairo_line_to(cr, origin + top_rect_width, top_rect_height);

  // Check if the key is pressed on the top part
  if (is_in_rectangle(x, y, origin, 0, top_rect_width, top_rect_height))
  {
    cairo_set_source_rgb(cr, 0.5, 0.5, 0.5);
    printf("id: %d\n", id);
    present = 1;
  }

  // Trace the bottom part
  cairo_line_to(cr, drawing_area_width * 3 / (num_keys * 4) + origin, top_rect_height);
  cairo_line_to(cr, drawing_area_width * 3 / (num_keys * 4) + origin, drawing_area_height);
  cairo_line_to(cr, bot_rect_width, drawing_area_height);
  cairo_line_to(cr, bot_rect_width, top_rect_height);
  cairo_line_to(cr, origin, top_rect_height);
  cairo_line_to(cr, origin, 0);

  // Check if the key is pressed on the bottom part
  if (is_in_rectangle(x, y, bot_rect_width, top_rect_height, drawing_area_width / num_keys, bot_rect_height))
  {
    cairo_set_source_rgb(cr, 0.5, 0.5, 0.5);
    printf("id: %d\n", id);
    present = 1;
  }

  cairo_fill(cr);
  return present;
}

// Draws one right type white key
int on_draw_right_type_white_key(cairo_t *cr, int drawing_area_width, int drawing_area_height, int num_keys, int j, int id)
{
  int present = 0;
  // Default color white
  cairo_set_source_rgb(cr, 1, 1, 1);
  // The origin of tracing
  int origin = drawing_area_width / (num_keys / 7) * j / 7;

  // parameters of the top rectangle
  int top_rect_width = drawing_area_width * 3 / (num_keys * 4);
  int top_rect_height = drawing_area_height * 3 / 5;

  // parametes of the bottom rectangle
  int bot_rect_width = drawing_area_width / num_keys;
  int bot_rect_height = drawing_area_height * 2 / 5;

  cairo_line_to(cr, origin, 0);
  cairo_line_to(cr, origin, drawing_area_height);
  cairo_line_to(cr, origin - bot_rect_width, drawing_area_height);
  cairo_line_to(cr, origin - bot_rect_width, top_rect_height);
  cairo_line_to(cr, origin - top_rect_width, top_rect_height);
  cairo_line_to(cr, origin - top_rect_width, 0);
  cairo_line_to(cr, origin, 0);

  // Check if the key is pressed on the top part
  if (is_in_rectangle(x, y, origin - top_rect_width, 0, top_rect_width, top_rect_height))
  {
    cairo_set_source_rgb(cr, 0.5, 0.5, 0.5);
    printf("id: %d\n", id);
    present = 1;
  }
  // Check if the key is pressed on the bottom part
  if (is_in_rectangle(x, y, origin - bot_rect_width, top_rect_height, bot_rect_width, bot_rect_height))
  {
    cairo_set_source_rgb(cr, 0.5, 0.5, 0.5);
    printf("id: %d\n", id);
    present = 1;
  }
  cairo_fill(cr);
  return present;
}

// Draws the full keyboard
static gboolean
on_draw_full_keyboard(cairo_t *cr, int drawing_area_width, int drawing_area_height, int num_octaves, Uint8 * keyboard)
{
  int num_keys = 7 * num_octaves;
  for (int o = 0; o < num_octaves; o++)
  {
    if(on_draw_left_type_white_key(cr, drawing_area_width, drawing_area_height, num_keys, 0 + o * 7, 0 + 12 * o))
    {
      keyboard[0 + 12 * o] = 1;
    }
    else
    {
      keyboard[0 + 12 * o] = 0;
    } // Do
    if(on_draw_black_key(cr, drawing_area_width, drawing_area_height, num_keys, 3 + o * 28, 1 + 12 * o))
    {
      keyboard[1 + 12 * o] = 1;
    }
    else
    {
      keyboard[1 + 12 * o] = 0;
    } // Do#
    if(on_draw_center_type_white_key(cr, drawing_area_width, drawing_area_height, num_keys, 1 + o * 7, 2 + 12 * o))
    {
      keyboard[2 + 12 * o] = 1;
    }
    else
    {
      keyboard[2 + 12 * o] = 0;
    } //Re
    if(on_draw_black_key(cr, drawing_area_width, drawing_area_height, num_keys, 7 + o * 28, 3 + 12 * o)  )
    {
      keyboard[3 + 12 * o] = 1;
    }
    else
    {
      keyboard[3 + 12 * o] = 0;
    } //Re#
    if(on_draw_right_type_white_key(cr, drawing_area_width, drawing_area_height, num_keys, 3 + o * 7, 4 + 12 * o) )
    {
      keyboard[4 + 12 * o] = 1;
    }
    else
    {
      keyboard[4 + 12 * o] = 0;
    } //Mi
    if(on_draw_left_type_white_key(cr, drawing_area_width, drawing_area_height, num_keys, 3 + o * 7, 5 + 12 * o))
    {
      keyboard[5 + 12 * o] = 1;
    }
    else
    {
      keyboard[5 + 12 * o] = 0;
    } //Fa
    if(on_draw_black_key(cr, drawing_area_width, drawing_area_height, num_keys, 15 + o * 28, 6 + 12 * o))
    {
      keyboard[6 + 12 * o] = 1;
    }
    else
    {
      keyboard[6 + 12 * o] = 0;
    } //Fa#
    if(on_draw_center_type_white_key(cr, drawing_area_width, drawing_area_height, num_keys, 4 + o * 7, 7 + 12 * o))
    {
      keyboard[7 + 12 * o] = 1;
    }
    else
    {
      keyboard[7 + 12 * o] = 0;
    } //Sol
    if(on_draw_black_key(cr, drawing_area_width, drawing_area_height, num_keys, 19 + o * 28, 8 + 12 * o))
    {
      keyboard[8 + 12 * o] = 1;
    }
    else
    {
      keyboard[8 + 12 * o] = 0;
    } //Sol#
    if(on_draw_center_type_white_key(cr, drawing_area_width, drawing_area_height, num_keys, 5 + o * 7, 9 + 12 * o))
    {
      keyboard[9 + 12 * o] = 1;
    }
    else
    {
      keyboard[9 + 12 * o] = 0;
    } //La
    if(on_draw_black_key(cr, drawing_area_width, drawing_area_height, num_keys, 23 + o * 28, 10 + 12 * o))
    {
      keyboard[10 + 12 * o] = 1;
    }
    else
    {
      keyboard[10 + 12 * o] = 0;
    } //La#
    if(on_draw_right_type_white_key(cr, drawing_area_width, drawing_area_height, num_keys, 7 + o * 7, 11 + 12 * o))
    {
      keyboard[11 + 12 * o] = 1;
    }
    else
    {
      keyboard[11 + 12 * o] = 0;
    } //Si
  }
  on_draw_key_lines(cr, drawing_area_width, drawing_area_height, num_octaves);
  return G_SOURCE_REMOVE;
}

// Dynamically draws the signal
static gboolean
on_draw_set_full_keyboard(GtkWidget *widget, cairo_t *cr, __attribute_maybe_unused__ gpointer user_data)
{
  Uint8 * keyboard = (Uint8 *)user_data;
  GdkRectangle da;            /* GtkDrawingArea size */
  gdouble dx = 2.0, dy = 2.0; /* Pixels between each point */
  gdouble clip_x1 = 0.0, clip_y1 = 0.0, clip_x2 = 0.0, clip_y2 = 0.0;
  GdkWindow *window = gtk_widget_get_window(widget);
  int drawing_area_width = gtk_widget_get_allocated_width(widget);
  int drawing_area_height = gtk_widget_get_allocated_height(widget);

  set_up_axes_for_piano(window, &da, cr, &clip_x1, &clip_x2, &clip_y1, &clip_y2, &dx, &dy);

  on_draw_full_keyboard(cr, drawing_area_width, drawing_area_height, octave_number,keyboard);
  gtk_widget_queue_draw_area(widget, 0, 0, drawing_area_width, drawing_area_height);
  return G_SOURCE_REMOVE;
}

int gtk_run_app(vis_data *vis_d, int argc, char **argv)
{
  // The table of threads
  GThread *thread[2];
  //! WARNING this parameter has to be removed to allow multiple saves and loads
  int loaded = 0;
  // Init the gtk
  gtk_init(&argc, &argv);
  // Init the node structure
  nodes = node_build_sentinel();
  // Init the builder
  GtkBuilder *builder = gtk_builder_new();
  GError *error = NULL;
  if (gtk_builder_add_from_file(builder, "visualiser/plain.glade", &error) == 0)
  {
    g_printerr("Error loading file: %s\n", error->message);
    g_clear_error(&error);
    return 1;
  }

  x = y = -1;
  octave_number = 1;

  /*
    PIANO
  */

  GtkDrawingArea *da_piano = GTK_DRAWING_AREA(gtk_builder_get_object(builder, "da_piano"));
  GtkEventBox *event_box = GTK_EVENT_BOX(gtk_builder_get_object(builder, "event_box"));
  GtkScale *scale_piano = GTK_SCALE(gtk_builder_get_object(builder, "octaves"));

  g_signal_connect(G_OBJECT(da_piano), "draw", G_CALLBACK(on_draw_set_full_keyboard), vis_d->state);
  g_signal_connect(G_OBJECT(event_box), "event", G_CALLBACK(current_key_click), vis_d->state);
  g_signal_connect(G_OBJECT(scale_piano), "value_changed", G_CALLBACK(on_scale_change_piano), NULL);

  /*
    Top level Widgets
      - Main Window of the application
      - File Chooser (allows to load files)
      - Save button (allows to save a current signal to a file)
  */

  // Main Window
  GtkWindow *window = GTK_WINDOW(gtk_builder_get_object(builder, "main_window"));
  // FileChooser
  GtkFileChooser *my_file_chooser = GTK_FILE_CHOOSER(gtk_builder_get_object(builder, "file_load"));
  // Save Button
  GtkButton *save_file = GTK_BUTTON(gtk_builder_get_object(builder, "save_file"));

  /*
    Signal Creator Widgets
      - Drawing Area on which the full signal is drawn
      - List Box that contains all the basic signals
      - Scale that sets the main frequency of the signal for visual purposes
      - Buttons allowing to add a new basic signals to the List Box
  */

  // Drawing Area for the full signal
  GtkDrawingArea *full_sig = GTK_DRAWING_AREA(gtk_builder_get_object(builder, "full_sig"));
  // List Box to put the basic signals in
  list = GTK_LIST_BOX(gtk_builder_get_object(builder, "list"));
  // Global visual frequency of the signal
  GtkScale *freq = GTK_SCALE(gtk_builder_get_object(builder, "global_freq"));

  // Submition BUttons
  GtkButton *sine_submit = GTK_BUTTON(gtk_builder_get_object(builder, "sine_submit"));
  GtkButton *square_submit = GTK_BUTTON(gtk_builder_get_object(builder, "square_submit"));
  GtkButton *trig_submit = GTK_BUTTON(gtk_builder_get_object(builder, "trig_submit"));
  GtkButton *saw_submit = GTK_BUTTON(gtk_builder_get_object(builder, "saw_submit"));
  GtkButton *saw1_submit = GTK_BUTTON(gtk_builder_get_object(builder, "saw1_submit"));

  /*
    Filter Manager Widgets
    - Drawing Areas for the harmonics and the proper signal
    - Spin Buttons that allow to zoom onto the signal
    - Check Buttons that allow to choose a particular filter
    - Scales to set the different frequency filter values
  */

  // Drawing areas
  GtkDrawingArea *da_signal = GTK_DRAWING_AREA(gtk_builder_get_object(builder, "area"));
  GtkDrawingArea *da_harmonics = GTK_DRAWING_AREA(gtk_builder_get_object(builder, "harmonics_da"));

  // SpinButtons
  GtkSpinButton *spx = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "X_ZOOM"));
  GtkSpinButton *spy = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "Y_ZOOM"));

  // Check Buttons
  GtkCheckButton *low_pass_activate = GTK_CHECK_BUTTON(gtk_builder_get_object(builder, "low_pass_activate"));
  GtkCheckButton *high_pass_activate = GTK_CHECK_BUTTON(gtk_builder_get_object(builder, "high_pass_activate"));
  GtkCheckButton *band_pass_activate = GTK_CHECK_BUTTON(gtk_builder_get_object(builder, "band_pass_activate"));
  GtkCheckButton *band_cut_activate = GTK_CHECK_BUTTON(gtk_builder_get_object(builder, "band_cut_activate"));

  // Scale
  GtkScale *low_pass_cutoff = GTK_SCALE(gtk_builder_get_object(builder, "low_pass_cut"));
  GtkScale *high_pass_cutoff = GTK_SCALE(gtk_builder_get_object(builder, "high_pass_cut"));
  GtkScale *band_pass_low = GTK_SCALE(gtk_builder_get_object(builder, "band_pass_low"));
  GtkScale *band_pass_high = GTK_SCALE(gtk_builder_get_object(builder, "band_pass_high"));
  GtkScale *band_cut_low = GTK_SCALE(gtk_builder_get_object(builder, "band_cut_low"));
  GtkScale *band_cut_high = GTK_SCALE(gtk_builder_get_object(builder, "band_cut_high"));

  /*
    ADSR Creator Widgets
      - Drawing Area for the ADSR signal
      - The Spin Buttons for setting the times of each phase
      - The Sacels to set the relative amplitude
  */

  // ADSR da
  GtkDrawingArea *da = GTK_DRAWING_AREA(gtk_builder_get_object(builder, "da"));

  // ADSR Spin Buttons
  GtkSpinButton *attack_phase = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "attack_phase_control"));
  GtkSpinButton *decay_phase = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "decay_phase_control"));
  GtkSpinButton *release_phase = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "release_phase_control"));

  // ADSR Scales
  GtkScale *attack_bot = GTK_SCALE(gtk_builder_get_object(builder, "attack_bot"));
  GtkScale *attack_top = GTK_SCALE(gtk_builder_get_object(builder, "attack_top"));
  GtkScale *decay_bot = GTK_SCALE(gtk_builder_get_object(builder, "decay_bot"));
  GtkScale *sustain = GTK_SCALE(gtk_builder_get_object(builder, "sustain"));

  // Unreference the builder, since all the wanted object were built
  g_object_unref(builder);

  /*
    Main Utility Signals
  */

  // Set the chosen file
  g_signal_connect(my_file_chooser, "update-preview", G_CALLBACK(update_preview_cb), &loaded);
  // Save a current configuration
  g_signal_connect(G_OBJECT(save_file), "clicked", G_CALLBACK(on_save_state), NULL);
  // Track the recording functionalities
  g_signal_connect(G_OBJECT(window), "key_release_event", G_CALLBACK(key_released), (gpointer)vis_d->data);
  // Destroy signal
  g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(on_destroy), &vis_d->stop_thread);

  /*
    Filter Window Signals
  */

  // Signal on the x_zoom value of the filter Drawing area
  g_signal_connect(G_OBJECT(spx), "value_changed", G_CALLBACK(on_x), &vis_d->x_zoom);
  // Signal on the y_zoom value of the filter Drawing area
  g_signal_connect(G_OBJECT(spy), "value_changed", G_CALLBACK(on_y), &vis_d->y_zoom);

  // Signal to the low_pass_activate
  g_signal_connect(G_OBJECT(low_pass_activate), "toggled", G_CALLBACK(on_activate), &vis_d->low_active);
  // Signal to the high_pass_activate
  g_signal_connect(G_OBJECT(high_pass_activate), "toggled", G_CALLBACK(on_activate), &vis_d->high_active);
  // Signal to the band_pass_active
  g_signal_connect(G_OBJECT(band_pass_activate), "toggled", G_CALLBACK(on_activate), &vis_d->band_pass_active);
  // Signal to the band_cut_active
  g_signal_connect(G_OBJECT(band_cut_activate), "toggled", G_CALLBACK(on_activate), &vis_d->band_cut_active);

  GtkMultipleScales *band_cut_data_low = malloc(sizeof(GtkMultipleScales));
  band_cut_data_low->data = vis_d;
  band_cut_data_low->lead = band_cut_high;

  GtkMultipleScales *band_cut_data_high = malloc(sizeof(GtkMultipleScales));
  band_cut_data_high->data = vis_d;
  band_cut_data_high->lead = band_cut_low;

  GtkMultipleScales *band_pass_data_low = malloc(sizeof(GtkMultipleScales));
  band_pass_data_low->data = vis_d;
  band_pass_data_low->lead = band_pass_high;

  GtkMultipleScales *band_pass_data_high = malloc(sizeof(GtkMultipleScales));
  band_pass_data_high->data = vis_d;
  band_pass_data_high->lead = band_pass_low;

  g_signal_connect(G_OBJECT(band_cut_low), "value_changed", G_CALLBACK(on_scale_band_cut_change_low), (gpointer)band_cut_data_low);
  g_signal_connect(G_OBJECT(band_cut_high), "value_changed", G_CALLBACK(on_scale_band_cut_change_high), (gpointer)band_cut_data_high);
  g_signal_connect(G_OBJECT(band_pass_low), "value_changed", G_CALLBACK(on_scale_band_change_low), (gpointer)band_pass_data_low);
  g_signal_connect(G_OBJECT(band_pass_high), "value_changed", G_CALLBACK(on_scale_band_change_high), (gpointer)band_pass_data_high);
  g_signal_connect(G_OBJECT(low_pass_cutoff), "value_changed", G_CALLBACK(on_scale_change), &vis_d->low_pass_cut);
  g_signal_connect(G_OBJECT(high_pass_cutoff), "value_changed", G_CALLBACK(on_scale_change), &vis_d->high_pass_cut);

  // Signal on the drawing area of the signal
  g_signal_connect(G_OBJECT(da_signal), "draw", G_CALLBACK(on_draw_signal), vis_d);
  // Signal on the drawing area of the harmonic
  g_signal_connect(G_OBJECT(da_harmonics), "draw", G_CALLBACK(on_draw_harmonics), vis_d);

  // ADSR signals

  g_signal_connect(G_OBJECT(attack_phase), "value_changed", G_CALLBACK(on_spinner_change), &vis_d->attack_phase);
  g_signal_connect(G_OBJECT(decay_phase), "value_changed", G_CALLBACK(on_spinner_change), &vis_d->decay_phase);
  g_signal_connect(G_OBJECT(release_phase), "value_changed", G_CALLBACK(on_spinner_change), &vis_d->release_phase);

  g_signal_connect(G_OBJECT(attack_bot), "value_changed", G_CALLBACK(on_scale_change), &vis_d->attack_amp);
  g_signal_connect(G_OBJECT(attack_top), "value_changed", G_CALLBACK(on_scale_change), &vis_d->decay_amp);
  g_signal_connect(G_OBJECT(decay_bot), "value_changed", G_CALLBACK(on_scale_change), &vis_d->sustain_amp);
  g_signal_connect(G_OBJECT(sustain), "value_changed", G_CALLBACK(on_scale_change), &vis_d->sustain_amp);

  g_signal_connect(G_OBJECT(da), "draw", G_CALLBACK(on_adsr_draw), vis_d);

  int sine_file = 0;
  int trig_file = 1;
  int saw_file = 2;
  int saw_composite_file = 3;
  int square_file = 4;

  g_signal_connect(G_OBJECT(sine_submit), "clicked", G_CALLBACK(init_and_create_row), &sine_file);
  g_signal_connect(G_OBJECT(trig_submit), "clicked", G_CALLBACK(init_and_create_row), &trig_file);
  g_signal_connect(G_OBJECT(saw_submit), "clicked", G_CALLBACK(init_and_create_row), &saw_file);
  g_signal_connect(G_OBJECT(saw1_submit), "clicked", G_CALLBACK(init_and_create_row_composite), &saw_composite_file);
  g_signal_connect(G_OBJECT(square_submit), "clicked", G_CALLBACK(init_and_create_row_composite), &square_file);

  g_signal_connect(G_OBJECT(freq), "value_changed", G_CALLBACK(on_scale_change_global_freq), NULL);

  int full_sig_id = -1;
  g_signal_connect(G_OBJECT(full_sig), "draw", G_CALLBACK(on_draw_created_or_full_signal), &full_sig_id);

  context = g_main_context_default();

  thread[0] = g_thread_new(NULL, thread_func, vis_d);
  thread[1] = g_thread_new(NULL, thread_update, vis_d);

  gtk_widget_show_all(GTK_WIDGET(window));

  gtk_main();

  g_thread_join(thread[0]);
  g_thread_join(thread[1]);

  free(band_cut_data_low);
  free(band_cut_data_high);
  free(band_pass_data_low);
  free(band_pass_data_high);

  return 0;
}
