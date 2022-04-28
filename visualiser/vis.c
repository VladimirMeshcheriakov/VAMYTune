#include "vis.h"

GMainContext *context;
//File names to add new basic signals
char global_file_name[] = "visualiser/glade_signals/signal.glade";
char global_file_name_components[] = "visualiser/glade_signals/signal_components.glade";

// The nodes of the different signals
node *nodes;
last_events_stack* last_events;

// The current highest id
int global_id = 0;

// The list in which the widgets are stored
GtkListBox *list;

// The global frequency controller
float global_freq = 0.0;



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
  else if (event->keyval == GDK_KEY_l)
  {
    data->wav_manager->loop = 1;
  }
  else if (event->keyval == GDK_KEY_m)
  {
    data->wav_manager->loop = 0;
  }
  return GDK_EVENT_PROPAGATE;
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


void recorded_samples_fill(ud *data)
{
  float * buffer = malloc(sizeof(float)*2);
  for (size_t i = 0; i < data->fout_size; i++)
  { 
    if(read_from_wav(data->fout,buffer)==2)
    {
      data->recorded_sig[i] = buffer[0];
    }
    else
    {
      break;
    }
      
  }
  
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
    //apply_filter_to_sample(my_data, 1024);
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


// Dynamically draws the harmonics
gboolean on_draw_recorded(GtkWidget *widget, cairo_t *cr, gpointer user_data)
{
  ud *data = (ud *)user_data;

  GdkRectangle da_parameters; /* GtkDrawingArea size */
  double dx = 0.5, dy = 0.5;  /* Pixels between each point */
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
  cairo_translate(cr, da_parameters.width / 2, da_parameters.height/2);
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
  cairo_set_source_rgba(cr, 1, 0.2, 0.2, 0.6);
  size_t cpt =0;
  for (i = clip_x1; i < clip_x2; i += (clip_x2 * 16  / data->fout_size ))
  {
    if(data->wav_manager->playback)
    { 
      
    }

    if(cpt<data->fout_size)
    {
      cairo_line_to(cr, i, data->recorded_sig[cpt] * clip_y2);
    }
    

    
    cpt+=1;
  }

  // printf("cpr:%d\n",cpt);
  /* Draw the curve */

  // printf("da_h %d , da_w %d\n",drawing_area_height,drawing_area_width);
  cairo_stroke(cr);
  gtk_widget_queue_draw_area(widget, 0, 0, da_parameters.width, da_parameters.height);

  return G_SOURCE_REMOVE;
}


int gtk_run_app(vis_data *vis_d, int argc, char **argv)
{
  // The table of threads
  GThread *thread[2];
  // Init the gtk
  gtk_init(&argc, &argv);
  // Init the node structure
  nodes = node_build_sentinel();
  last_events = last_events_stack_build_sentinel();
  // Init the builder
  GtkBuilder *builder = gtk_builder_new();
  GError *error = NULL;
  if (gtk_builder_add_from_file(builder, "visualiser/plain.glade", &error) == 0)
  {
    g_printerr("Error loading file: %s\n", error->message);
    g_clear_error(&error);
    return 1;
  }
  /*
    Record
    
  */
  GtkDrawingArea *da_record = GTK_DRAWING_AREA(gtk_builder_get_object(builder, "record_da"));
  //g_signal_connect(G_OBJECT(da_record), "draw", G_CALLBACK(on_draw_recorded), vis_d->data);

  /*
    PIANO
  */

  GtkDrawingArea *da_piano = GTK_DRAWING_AREA(gtk_builder_get_object(builder, "da_piano"));
  GtkEventBox *event_box = GTK_EVENT_BOX(gtk_builder_get_object(builder, "event_box"));
  GtkScale *scale_piano = GTK_SCALE(gtk_builder_get_object(builder, "octaves"));

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
    Piano Signals
  */
  g_signal_connect(G_OBJECT(da_piano), "draw", G_CALLBACK(on_draw_set_full_keyboard), vis_d->state);
  g_signal_connect(G_OBJECT(event_box), "event", G_CALLBACK(current_key_click), vis_d->state);
  g_signal_connect(G_OBJECT(scale_piano), "value_changed", G_CALLBACK(on_scale_change_piano), NULL);

  /*
    Main Utility Signals
  */

  // Set the chosen file
  g_signal_connect(my_file_chooser, "selection-changed", G_CALLBACK(update_preview_cb), NULL);
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
