#include "vis.h"

GMainContext *context;

typedef struct
{
  GtkScale *lead;
  vis_data *data;
} GtkMultipleScales;

char *global_file_data;
size_t data_length;

// The nodes of the different signals
node *nodes;
// The current highest id
int global_id = 0;
// The list in which the widgets are stored
GtkListBox *list;
// The global frequency controller
float global_freq = 0.0;

float instance_signal(float volume, sig_info *vs, double time, float freq)
{
  float he = 0;
  switch (vs->type)
  {
  case 0:
    he += vs->amp * sine(volume, freq * vs->freq, time);
    break;
  case 1:
    he += vs->amp * triangle(volume, freq * vs->freq, time);
    break;
  case 2:
    he += vs->amp * saw(volume, freq * vs->freq, time);
    break;
  case 3:
    he += vs->amp * saw2(volume, freq * vs->freq, time, vs->form + 1);
    break;
  case 4:
    he += vs->amp * square(volume, freq * vs->freq, time, vs->form);
    break;
  }
  return he;
}

float global_signal(float volume, double time, float freq)
{
  float he = 0;
  int j = 0;
  while (j < global_id)
  {
    node *id_node = node_get_at(nodes, j);
    if (id_node != NULL)
    {
      sig_info *vs = id_node->value;
      if (!vs->mute)
      {
        he += instance_signal(volume, vs, time, freq);
      }
    }

    j++;
  }
  return he;
}

void apply_filter_to_sample(vis_data *data, size_t size)
{

  float *buf = data->sig_sample;
  float *mag = data->harmonics_sample;
  float *rep = data->filtered_sample;
  for (size_t i = 0; i < 1024; i++)
  {
    double time = (double)i / 44100.0;
    float val = global_signal(0.6, time, 440);
    buf[i] = val;
    rep[i] = val;
  }

  complex_number *arr = fft(buf, size);

  // put your filter here

  if (data->low_active)
  {
    filter_cut_from(arr, size, (size_t)((data->low_pass_cut * 1024) / 44000));
  }
  if (data->high_active)
  {
    filter_cut_before(arr, (size_t)((data->high_pass_cut * 1024) / 44000), size);
  }
  if (data->band_pass_active)
  {
    filter_cut_around(arr, (size_t)((data->band_pass_low * 1024) / 44000), (size_t)((data->band_pass_high * 1024) / 44000), size);
  }
  if (data->band_cut_active)
  {
    filter_cut_between(arr, (size_t)((data->band_cut_low * 1024) / 44000), (size_t)((data->band_cut_high * 1024) / 44000), size);
  }

  mag_table(arr, mag, size);

  ifft(arr, rep, size);
  free(arr);
}

// Evaluate the stage execution of a signal and send the final value
float signal_treat(float volume, ud *data)
{
  float val = 0.0;

  for (int i = 0; i < 127; i++)
  {
    if (data->all_keys->keys[i] || (data->time_management->time_table[i]->release_stage && (data->all_keys->effects[i] > 0.0)))
    {
      val += data->all_keys->effects[i] * global_signal(volume, data->time_management->actual_time, data->all_keys->octave * piano_note_to_freq(i));
    }
    else
    {
      data->time_management->time_table[i]->release_stage = 0;
    }
  }
  return val;
}

/*

The functions that are called on change of a scale or a button



*/

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

signal_params *init_signal_params()
{
  signal_params *params = calloc(1, sizeof(signal_params));
  params->attack_amp = 0;
  params->attack_phase = 0;
  params->band_cut_active = 0;
  params->band_cut_high = 0;
  params->band_cut_low = 0;
  params->band_pass_active = 0;
  params->band_pass_high = 0;
  params->band_pass_low = 0;
  params->decay_amp = 0;
  params->decay_phase = 0;
  params->sustain_amp = 0;
  params->high_active = 0;
  params->high_pass_cut = 0;
  params->low_active = 0;
  params->low_pass_cut = 0;
  params->release_phase = 0;
  params->signals = node_build_sentinel();
  return params;
}

signal_params *global_params;


char * create_string_from_signal(sig_info * info)
{
    char * buffer = calloc(100,sizeof(char));
    if(info->type< 3)
    {
        sprintf(buffer,"[%d;,\n%.3f;,\n%.3f;,\n],\n",info->type,info->amp*100.0, info->freq*100.0);
    }
    else
    {
        sprintf(buffer,"[%d;,\n%.3f;,\n%.3f;,\n%.3f;,\n],\n",info->type,info->amp*100.0, info->freq*100.0, info->form*100);
    }
    return buffer;
}

void find_scopes(char *arr, size_t len, signal_params *params)
{
  int open_brace_occ = 0;
  int closed_brace_occ = 0;
  for (size_t i = 0; i < len; i++)
  {
    if (arr[i] == '{' && open_brace_occ == 0)
    {

      //printf("Beg of Signals\n");
      open_brace_occ += 1;
      // Signals parameters open
      while (arr[i] != '}')
      {

        // Parse Signals params
        if (arr[i] == '[')
        {
          i += 1;
          //printf("Beg of Sig Parameter\n");
          // A param was spotted
          sig_info *info = init_null_struct();
          int param_num = 0;
          while (arr[i] != ']')
          {

            while (arr[i] != ',')
            {
              int cpt = 0;
              char val[10];
              // One line params
              while (arr[i] != ';')
              {
                val[cpt] = arr[i];
                // Single value
                cpt += 1;
                i += 1;
              }
              val[cpt] = '\n';
              float test = atof(val);
              switch (param_num)
              {
              case 0:
                info->type = test;
                //printf("Info type is set to %d \n", info->type);
                break;
              case 1:
                info->amp = (test / 100.00);
                //printf("Info amp is set to %f \n", test);
                break;
              case 2:
                info->freq = (test / 100.00);
                //printf("Info freq is set to %f \n", test);
                break;
              case 3:
                info->form = test;
                //printf("Info form is set to %f \n", test);
                break;
              }
              param_num += 1;
              //printf("para_num is at %d\n", param_num);
              i += 1;
            }
            i += 2;
            closed_brace_occ += 1;
          }
          node_insert_end(params->signals, info);
          //printf("\n");
          //printf("End of Sig Parameter\n");
          // Add param
        }
        // Onto next parameter
        i += 1;
        closed_brace_occ += 1;
      }
      //printf("End of Signals\n");
      // End of parsing
      break;
    }
  }
}



void write_to_triton(node * nodes)
{
    //Beg of file
    FILE *file;
    file = fopen("out.triton", "w");
    if(file == NULL)
    {
        printf("error\n");
    }
    fputc('{',file);
    fputc('\n',file);
    
   char * buffer_full = calloc(100 * node_val_count(nodes) ,sizeof(char));
    while (nodes->next != NULL)
    {
        nodes = nodes->next;
        strcat(buffer_full,create_string_from_signal(nodes->value));
        
    }
    //printf("%s\n",buffer_full);
    fputs(buffer_full,file);
    free(buffer_full);
    //End of file
    fputc('}',file);
    fputc('\n',file);
    fclose(file);
}
void affect_new(GtkWidget *a_scale, float *old_val)
{
  float new_val = gtk_range_get_value(GTK_RANGE(a_scale));
  *old_val = new_val;
}

gboolean on_scale_change_global_freq(GtkWidget *a_scale)
{
  float new_freq = gtk_range_get_value(GTK_RANGE(a_scale));
  // g_print("id: %d\n", id);
  global_freq = new_freq;
  return G_SOURCE_REMOVE;
}

gboolean on_scale_change_amp(GtkWidget *a_scale, gpointer user_data)
{
  int *id = (int *)user_data;
  node *id_node = node_get_at(nodes, *id);
  sig_info *sine_data = id_node->value;
  affect_new(a_scale, &sine_data->amp);
  return G_SOURCE_REMOVE;
}

gboolean on_scale_change_form(GtkWidget *a_scale, gpointer user_data)
{
  int *id = (int *)user_data;
  node *id_node = node_get_at(nodes, *id);
  // print_sine_info(id_node->value);
  sig_info *sine_data = id_node->value;
  affect_new(a_scale, &sine_data->form);
  return G_SOURCE_REMOVE;
}

gboolean on_scale_change_freq(GtkWidget *a_scale, gpointer user_data)
{
  int *id = (int *)user_data;
  node *id_node = node_get_at(nodes, *id);
  sig_info *sine_data = id_node->value;
  affect_new(a_scale, &sine_data->freq);
  return G_SOURCE_REMOVE;
}

gboolean on_delete_node(GtkWidget *a_button, gpointer user_data)
{
  int *index = (int *)user_data;

  GtkListBoxRow *to_delete = gtk_list_box_get_row_at_index(list, *index);
  gtk_container_remove(GTK_CONTAINER(list), GTK_WIDGET(to_delete));

  node_lower_id(nodes, *index);
  //node_print(nodes);

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
  return G_SOURCE_REMOVE;
}

/*

The functions that draw the signal

*/

void set_up_axes(GdkWindow *window, GdkRectangle *da, cairo_t *cr, gdouble *clip_x1, gdouble *clip_y1, gdouble *clip_x2, gdouble *clip_y2, gdouble *dx, gdouble *dy, int zoom_x, int zoom_y)
{

  /* Determine GtkDrawingArea dimensions */
  gdk_window_get_geometry(window, &da->x, &da->y, &da->width, &da->height);
  /* Draw on a black background */
  cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
  cairo_paint(cr);

  /* Change the transformation matrix */
  // Put the origin of the graph into the center of the image
  cairo_translate(cr, da->width / 2, da->height / 2);
  cairo_scale(cr, zoom_x, -zoom_y);
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
static gboolean
on_draw_created_signal(GtkWidget *widget, cairo_t *cr, gpointer user_data)
{
  int *id = (int *)user_data;
  // g_print("index in draw %d\n",id);
  node *id_node = node_get_at(nodes, *id);
  sig_info *vs = id_node->value;
  int zoom_x = 20;
  int zoom_y = 100;
  GdkRectangle da;            /* GtkDrawingArea size */
  gdouble dx = 2.0, dy = 2.0; /* Pixels between each point */
  gdouble i, clip_x1 = 0.0, clip_y1 = 0.0, clip_x2 = 0.0, clip_y2 = 0.0;
  GdkWindow *window = gtk_widget_get_window(widget);
  int drawing_area_width = gtk_widget_get_allocated_width(widget);
  int drawing_area_height = gtk_widget_get_allocated_height(widget);
  set_up_axes(window, &da, cr, &clip_x1, &clip_x2, &clip_y1, &clip_y2, &dx, &dy, zoom_x, zoom_y);
  int cpt = 0;
  float he = 0;
  for (i = clip_x1; i < -clip_x1; i += fabs(clip_x1) * 2 / 512)
  {
    if (cpt < 1024)
    {
      double time = (i + fabs(clip_x1)) / 44100.0;
      he = instance_signal(0.5, vs, time, global_freq);
      cairo_line_to(cr, i, he);
    }
    cpt += 1;
  }
  cairo_set_source_rgba(cr, 1, 0.2, 0.2, 0.6);
  cairo_stroke(cr);
  gtk_widget_queue_draw_area(widget, 0, 0, drawing_area_width, drawing_area_height);
  return G_SOURCE_REMOVE;
}

static gboolean
on_draw_full_signal(GtkWidget *widget, cairo_t *cr)
{
  int zoom_x = 10;
  int zoom_y = 50;

  GdkRectangle da;            /* GtkDrawingArea size */
  gdouble dx = 2.0, dy = 2.0; /* Pixels between each point */
  gdouble i, clip_x1 = 0.0, clip_y1 = 0.0, clip_x2 = 0.0, clip_y2 = 0.0;
  GdkWindow *window = gtk_widget_get_window(widget);
  int drawing_area_width = gtk_widget_get_allocated_width(widget);
  int drawing_area_height = gtk_widget_get_allocated_height(widget);
  set_up_axes(window, &da, cr, &clip_x1, &clip_x2, &clip_y1, &clip_y2, &dx, &dy, zoom_x, zoom_y);

  float he = 0;
  for (i = clip_x1; i < -clip_x1; i += fabs(clip_x1) * 2 / 512)
  {
    double time = (i + fabs(clip_x1)) / 44100.0;
    he = global_signal(0.5, time, global_freq);
    cairo_line_to(cr, i, 4.0 * he);
  }

  cairo_set_source_rgba(cr, 1, 0.2, 0.2, 0.6);
  cairo_stroke(cr);
  gtk_widget_queue_draw_area(widget, 0, 0, drawing_area_width, drawing_area_height);
  return G_SOURCE_REMOVE;
}

void row_sine(GtkWidget *button)
{
  GtkBuilder *builder = gtk_builder_new();
  GError *error = NULL;
  if (gtk_builder_add_from_file(builder, "visualiser/glade_signals/sine.glade", &error) == 0)
  {
    g_printerr("Error loading file: %s\n", error->message);
    g_clear_error(&error);
  }

  GtkBox *sine_box = GTK_BOX(gtk_builder_get_object(builder, "sine_box"));
  GtkButton *delete_button = GTK_BUTTON(gtk_builder_get_object(builder, "delete"));
  GtkDrawingArea *sin_da = GTK_DRAWING_AREA(gtk_builder_get_object(builder, "sin_da"));
  GtkScale *amp = GTK_SCALE(gtk_builder_get_object(builder, "sin_amp"));
  GtkScale *freq = GTK_SCALE(gtk_builder_get_object(builder, "sin_freq"));
  GtkToggleButton *mute = GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder, "mute"));

  sig_info *sine_data = init_null_struct();
  sine_data->type = 0;
  sine_data->id = global_id;
  node_insert_end(nodes, sine_data);

  // Append the row to the list, the last parameter must be at -1 to allow to append at the end
  gtk_list_box_insert(list, GTK_WIDGET(sine_box), -1);
  //g_print("id : %d\n", sine_data->id);

  g_signal_connect(G_OBJECT(delete_button), "clicked", G_CALLBACK(on_delete_node), (gpointer)&sine_data->id);
  g_signal_connect(G_OBJECT(sin_da), "draw", G_CALLBACK(on_draw_created_signal), (gpointer)&sine_data->id);
  g_signal_connect(G_OBJECT(amp), "value_changed", G_CALLBACK(on_scale_change_amp), (gpointer)&sine_data->id);
  g_signal_connect(G_OBJECT(freq), "value_changed", G_CALLBACK(on_scale_change_freq), (gpointer)&sine_data->id);
  g_signal_connect(G_OBJECT(mute), "toggled", G_CALLBACK(on_toggle), (gpointer)&sine_data->id);

  global_id += 1;

  gtk_widget_show_all(GTK_WIDGET(list));
  gtk_widget_show_all(GTK_WIDGET(sin_da));
}

void row_trig(GtkWidget *button)
{
  GtkBuilder *builder = gtk_builder_new();
  GError *error = NULL;
  if (gtk_builder_add_from_file(builder, "visualiser/glade_signals/trig.glade", &error) == 0)
  {
    g_printerr("Error loading file: %s\n", error->message);
    g_clear_error(&error);
  }

  GtkBox *sine_box = GTK_BOX(gtk_builder_get_object(builder, "trig_box"));
  GtkButton *delete_button = GTK_BUTTON(gtk_builder_get_object(builder, "delete"));
  GtkDrawingArea *trig_da = GTK_DRAWING_AREA(gtk_builder_get_object(builder, "trig_da"));
  GtkScale *amp = GTK_SCALE(gtk_builder_get_object(builder, "trig_amp"));
  GtkScale *freq = GTK_SCALE(gtk_builder_get_object(builder, "trig_freq"));
  GtkToggleButton *mute = GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder, "mute"));

  sig_info *trig_data = init_null_struct();
  trig_data->type = 1;
  trig_data->id = global_id;
  node_insert_end(nodes, trig_data);

  // Append the row to the list, the last parameter must be at -1 to allow to append at the end
  gtk_list_box_insert(list, GTK_WIDGET(sine_box), -1);
  //g_print("id : %d\n", trig_data->id);

  g_signal_connect(G_OBJECT(delete_button), "clicked", G_CALLBACK(on_delete_node), (gpointer)&trig_data->id);
  g_signal_connect(G_OBJECT(trig_da), "draw", G_CALLBACK(on_draw_created_signal), (gpointer)&trig_data->id);
  g_signal_connect(G_OBJECT(amp), "value_changed", G_CALLBACK(on_scale_change_amp), (gpointer)&trig_data->id);
  g_signal_connect(G_OBJECT(freq), "value_changed", G_CALLBACK(on_scale_change_freq), (gpointer)&trig_data->id);
  g_signal_connect(G_OBJECT(mute), "toggled", G_CALLBACK(on_toggle), (gpointer)&trig_data->id);

  global_id += 1;

  gtk_widget_show_all(GTK_WIDGET(list));
  gtk_widget_show_all(GTK_WIDGET(trig_da));
}

void row_saw(GtkWidget *button)
{
  GtkBuilder *builder = gtk_builder_new();
  GError *error = NULL;
  if (gtk_builder_add_from_file(builder, "visualiser/glade_signals/saw.glade", &error) == 0)
  {
    g_printerr("Error loading file: %s\n", error->message);
    g_clear_error(&error);
  }

  GtkBox *sine_box = GTK_BOX(gtk_builder_get_object(builder, "saw_box"));
  GtkButton *delete_button = GTK_BUTTON(gtk_builder_get_object(builder, "delete"));
  GtkDrawingArea *saw_da = GTK_DRAWING_AREA(gtk_builder_get_object(builder, "saw_da"));
  GtkScale *amp = GTK_SCALE(gtk_builder_get_object(builder, "saw_amp"));
  GtkScale *freq = GTK_SCALE(gtk_builder_get_object(builder, "saw_freq"));
  GtkToggleButton *mute = GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder, "mute"));

  sig_info *saw_data = init_null_struct();
  saw_data->type = 2;
  saw_data->id = global_id;
  node_insert_end(nodes, saw_data);

  // Append the row to the list, the last parameter must be at -1 to allow to append at the end
  gtk_list_box_insert(list, GTK_WIDGET(sine_box), -1);
  //g_print("id : %d\n", saw_data->id);

  g_signal_connect(G_OBJECT(delete_button), "clicked", G_CALLBACK(on_delete_node), (gpointer)&saw_data->id);
  g_signal_connect(G_OBJECT(saw_da), "draw", G_CALLBACK(on_draw_created_signal), (gpointer)&saw_data->id);
  g_signal_connect(G_OBJECT(amp), "value_changed", G_CALLBACK(on_scale_change_amp), (gpointer)&saw_data->id);
  g_signal_connect(G_OBJECT(freq), "value_changed", G_CALLBACK(on_scale_change_freq), (gpointer)&saw_data->id);
  g_signal_connect(G_OBJECT(mute), "toggled", G_CALLBACK(on_toggle), (gpointer)&saw_data->id);

  global_id += 1;

  gtk_widget_show_all(GTK_WIDGET(list));
  gtk_widget_show_all(GTK_WIDGET(saw_da));
}

void row_saw_composite(GtkWidget *button)
{
  GtkBuilder *builder = gtk_builder_new();
  GError *error = NULL;
  if (gtk_builder_add_from_file(builder, "visualiser/glade_signals/saw_composite.glade", &error) == 0)
  {
    g_printerr("Error loading file: %s\n", error->message);
    g_clear_error(&error);
  }

  GtkBox *sine_box = GTK_BOX(gtk_builder_get_object(builder, "saw_box"));
  GtkButton *delete_button = GTK_BUTTON(gtk_builder_get_object(builder, "delete"));
  GtkDrawingArea *saw_da = GTK_DRAWING_AREA(gtk_builder_get_object(builder, "saw_da"));
  GtkScale *amp = GTK_SCALE(gtk_builder_get_object(builder, "saw_amp"));
  GtkScale *freq = GTK_SCALE(gtk_builder_get_object(builder, "saw_freq"));
  GtkScale *components = GTK_SCALE(gtk_builder_get_object(builder, "saw_components"));
  GtkToggleButton *mute = GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder, "mute"));

  sig_info *saw_data = init_null_struct();
  saw_data->type = 3;
  saw_data->id = global_id;
  node_insert_end(nodes, saw_data);

  // Append the row to the list, the last parameter must be at -1 to allow to append at the end
  gtk_list_box_insert(list, GTK_WIDGET(sine_box), -1);
  //g_print("id : %d\n", saw_data->id);

  g_signal_connect(G_OBJECT(delete_button), "clicked", G_CALLBACK(on_delete_node), (gpointer)&saw_data->id);
  g_signal_connect(G_OBJECT(saw_da), "draw", G_CALLBACK(on_draw_created_signal), (gpointer)&saw_data->id);
  g_signal_connect(G_OBJECT(amp), "value_changed", G_CALLBACK(on_scale_change_amp), (gpointer)&saw_data->id);
  g_signal_connect(G_OBJECT(freq), "value_changed", G_CALLBACK(on_scale_change_freq), (gpointer)&saw_data->id);
  g_signal_connect(G_OBJECT(components), "value_changed", G_CALLBACK(on_scale_change_form), (gpointer)&saw_data->id);
  g_signal_connect(G_OBJECT(mute), "toggled", G_CALLBACK(on_toggle), (gpointer)&saw_data->id);

  global_id += 1;

  gtk_widget_show_all(GTK_WIDGET(list));
  gtk_widget_show_all(GTK_WIDGET(saw_da));
}

void row_square(GtkWidget *button)
{
  GtkBuilder *builder = gtk_builder_new();
  GError *error = NULL;
  if (gtk_builder_add_from_file(builder, "visualiser/glade_signals/square.glade", &error) == 0)
  {
    g_printerr("Error loading file: %s\n", error->message);
    g_clear_error(&error);
  }

  GtkBox *sine_box = GTK_BOX(gtk_builder_get_object(builder, "square_box"));
  GtkButton *delete_button = GTK_BUTTON(gtk_builder_get_object(builder, "delete"));
  GtkDrawingArea *square_da = GTK_DRAWING_AREA(gtk_builder_get_object(builder, "square_da"));
  GtkScale *amp = GTK_SCALE(gtk_builder_get_object(builder, "square_amp"));
  GtkScale *freq = GTK_SCALE(gtk_builder_get_object(builder, "square_freq"));
  GtkScale *shift = GTK_SCALE(gtk_builder_get_object(builder, "square_shift"));
  GtkToggleButton *mute = GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder, "mute"));

  sig_info *square_data = init_null_struct();
  square_data->type = 4;
  square_data->id = global_id;
  node_insert_end(nodes, square_data);

  // Append the row to the list, the last parameter must be at -1 to allow to append at the end
  gtk_list_box_insert(list, GTK_WIDGET(sine_box), -1);
  //g_print("id : %d\n", square_data->id);

  g_signal_connect(G_OBJECT(delete_button), "clicked", G_CALLBACK(on_delete_node), (gpointer)&square_data->id);
  g_signal_connect(G_OBJECT(square_da), "draw", G_CALLBACK(on_draw_created_signal), (gpointer)&square_data->id);
  g_signal_connect(G_OBJECT(amp), "value_changed", G_CALLBACK(on_scale_change_amp), (gpointer)&square_data->id);
  g_signal_connect(G_OBJECT(freq), "value_changed", G_CALLBACK(on_scale_change_freq), (gpointer)&square_data->id);
  g_signal_connect(G_OBJECT(shift), "value_changed", G_CALLBACK(on_scale_change_form), (gpointer)&square_data->id);
  g_signal_connect(G_OBJECT(mute), "toggled", G_CALLBACK(on_toggle), (gpointer)&square_data->id);

  global_id += 1;

  gtk_widget_show_all(GTK_WIDGET(list));
  gtk_widget_show_all(GTK_WIDGET(square_da));
}

// Stops the main thread, quits the gtk
void on_destroy(GtkWidget *Widget, gpointer user_data)
{
  int *running = (int *)user_data;
  *running = 0;
  gtk_main_quit();
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

// Key_change
static gboolean key_released(GtkWidget *widget, GdkEventKey *event, gpointer user_data)
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

// Toggles the activation
static gboolean
on_activate(GtkWidget *a_check, gpointer user_data)
{
  int *old_state = (int *)user_data;
  // Flip state
  int new_state = *old_state ? 0 : 1;
  *old_state = new_state;
  return G_SOURCE_REMOVE;
}

static gboolean
on_spinner_change(GtkWidget *a_spinner, gpointer user_data)
{
  float *current_time = (float *)user_data;
  float new_time = gtk_spin_button_get_value(GTK_SPIN_BUTTON(a_spinner));
  // g_print("%f\n",new_time);
  *current_time = new_time;
  return G_SOURCE_REMOVE;
}

// Scale move (normal)
static gboolean
on_scale_change(GtkWidget *a_scale, gpointer user_data)
{
  float *old_val = (float *)user_data;
  float actual_val = gtk_range_get_value(GTK_RANGE(a_scale));
  *old_val = actual_val;
  return G_SOURCE_REMOVE;
}

// Scale move on the low side of the band filters
static gboolean
on_scale_band_cut_change_low(GtkWidget *low_scale, gpointer user_data)
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
}

// Scale move on the high side of the band filters
static gboolean
on_scale_band_cut_change_high(GtkWidget *high_scale, gpointer user_data)
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
}

// Scale move on the low side of the band filters
static gboolean
on_scale_band_change_low(GtkWidget *low_scale, gpointer user_data)
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
}

// Scale move on the high side of the band filters
static gboolean
on_scale_band_change_high(GtkWidget *high_scale, gpointer user_data)
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
}

// Dynamically draws the harmonics
static gboolean
on_draw_harmonics(GtkWidget *widget, cairo_t *cr, gpointer user_data)
{
  vis_data *vs = (vis_data *)user_data;
  float *us = vs->harmonics_sample;
  int zoom_x = vs->x_zoom;
  int zoom_y = vs->y_zoom;

  GdkRectangle da;            /* GtkDrawingArea size */
  gdouble dx = 2.0, dy = 2.0; /* Pixels between each point */
  gdouble i, clip_x1 = 0.0, clip_y1 = 0.0, clip_x2 = 0.0, clip_y2 = 0.0;

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
  // Put the origin of the graph into the center of the image
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

  // printf("exec x1 %f , x2 %f, dx %f\n", clip_x1, clip_x2, (clip_x2/(double)1024));
  //  printf("exec y1 %f , y2 %f, dy %f\n", clip_y1, clip_y2, dy);
  /* Link each data point */
  int cpt = 0;
  for (i = clip_x1; i < clip_x2; i += (clip_x2 / (double)1024))
  {
    if (cpt < 1024)
    {
      float he = us[cpt];
      // printf("double %f\n",i);
      cairo_line_to(cr, i, he * clip_y2);
    }
    cpt += 1;
  }

  /* Draw the curve */
  cairo_set_source_rgba(cr, 1, 0.2, 0.2, 0.6);
  cairo_stroke(cr);
  // printf("da_h %d , da_w %d\n",drawing_area_height,drawing_area_width);

  gtk_widget_queue_draw_area(widget, 0, 0, drawing_area_width, drawing_area_height);

  return G_SOURCE_REMOVE;
}

// Dynamically draws the signal
static gboolean
on_draw_signal(GtkWidget *widget, cairo_t *cr, gpointer user_data)
{

  vis_data *vs = (vis_data *)user_data;
  float *us = vs->sig_sample;
  float *resp = vs->filtered_sample;
  int zoom_x = vs->x_zoom;
  int zoom_y = vs->y_zoom;

  GdkRectangle da;            /* GtkDrawingArea size */
  gdouble dx = 2.0, dy = 2.0; /* Pixels between each point */
  gdouble i, clip_x1 = 0.0, clip_y1 = 0.0, clip_x2 = 0.0, clip_y2 = 0.0;

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
  // Put the origin of the graph into the center of the image
  cairo_translate(cr, da.width / 2, da.height / 2);
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

  gtk_widget_queue_draw_area(widget, 0, 0, drawing_area_width, drawing_area_height);

  return G_SOURCE_REMOVE;
}

static gboolean
update_preview_cb(GtkFileChooser *file_chooser, gpointer data)
{
  int *load = (int *)data;
  if (*load == 0)
  {
    guchar *uri = gtk_file_chooser_get_uri(file_chooser);
    if (uri == NULL)
    {
      g_print("uri null\n");
      return G_SOURCE_REMOVE;
    }
    else
    {
      g_print("%s\n", uri);
    }
    GFile *file = g_file_new_for_uri(uri);
    if (file == NULL)
    {
      g_print("file null\n");
      return G_SOURCE_REMOVE;
    }

    GFileInfo *info;
    info = g_file_query_info(file, G_FILE_ATTRIBUTE_STANDARD_CONTENT_TYPE, G_FILE_QUERY_INFO_NONE, NULL, NULL);
    if (info == NULL)
    {
      g_print("info null\n");
      return G_SOURCE_REMOVE;
    }

    GBytes *file_bytes = g_file_load_bytes(file, NULL, NULL, NULL);
    if (file_bytes == NULL)
    {
      g_print("bytes null\n");
      return G_SOURCE_REMOVE;
    }
    size_t data_size = 0;
    char *pointer = g_bytes_get_data(file_bytes, &data_size);
    if (pointer == NULL)
    {
      g_print("pointer null\n");
      return G_SOURCE_REMOVE;
    }
    else
    {
      //g_print("%ld\n", data_size);
      // Call parser here:
      signal_params *params = init_signal_params();
      find_scopes(pointer, data_size, params);
      while (params->signals->next != NULL)
      {
        params->signals->value->id = global_id;

        switch (params->signals->value->type)
        {
        case 0:
        {
          GtkBuilder *builder = gtk_builder_new();
          GError *error = NULL;
          if (gtk_builder_add_from_file(builder, "visualiser/glade_signals/sine.glade", &error) == 0)
          {
            g_printerr("Error loading file: %s\n", error->message);
            g_clear_error(&error);
          }

          GtkBox *sine_box = GTK_BOX(gtk_builder_get_object(builder, "sine_box"));
          GtkButton *delete_button = GTK_BUTTON(gtk_builder_get_object(builder, "delete"));
          GtkDrawingArea *sin_da = GTK_DRAWING_AREA(gtk_builder_get_object(builder, "sin_da"));
          GtkScale *amp = GTK_SCALE(gtk_builder_get_object(builder, "sin_amp"));
          GtkScale *freq = GTK_SCALE(gtk_builder_get_object(builder, "sin_freq"));
          GtkToggleButton *mute = GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder, "mute"));

          sig_info *sine_data = params->signals->value;
          node_insert_end(nodes, sine_data);

          // Append the row to the list, the last parameter must be at -1 to allow to append at the end
          gtk_list_box_insert(list, GTK_WIDGET(sine_box), -1);

          g_signal_connect(G_OBJECT(delete_button), "clicked", G_CALLBACK(on_delete_node), (gpointer)&sine_data->id);
          g_signal_connect(G_OBJECT(sin_da), "draw", G_CALLBACK(on_draw_created_signal), (gpointer)&sine_data->id);
          g_signal_connect(G_OBJECT(amp), "value_changed", G_CALLBACK(on_scale_change_amp), (gpointer)&sine_data->id);
          g_signal_connect(G_OBJECT(freq), "value_changed", G_CALLBACK(on_scale_change_freq), (gpointer)&sine_data->id);
          g_signal_connect(G_OBJECT(mute), "toggled", G_CALLBACK(on_toggle), (gpointer)&sine_data->id);

          global_id += 1;

          gtk_range_set_value(GTK_RANGE(amp), sine_data->amp);
          gtk_range_set_value(GTK_RANGE(freq), sine_data->freq);

          gtk_widget_show_all(GTK_WIDGET(list));
          gtk_widget_show_all(GTK_WIDGET(sin_da));

          break;
        }
        case 1:
        {
          GtkBuilder *builder = gtk_builder_new();
          GError *error = NULL;
          if (gtk_builder_add_from_file(builder, "visualiser/glade_signals/trig.glade", &error) == 0)
          {
            g_printerr("Error loading file: %s\n", error->message);
            g_clear_error(&error);
          }

          GtkBox *sine_box = GTK_BOX(gtk_builder_get_object(builder, "trig_box"));
          GtkButton *delete_button = GTK_BUTTON(gtk_builder_get_object(builder, "delete"));
          GtkDrawingArea *trig_da = GTK_DRAWING_AREA(gtk_builder_get_object(builder, "trig_da"));
          GtkScale *amp = GTK_SCALE(gtk_builder_get_object(builder, "trig_amp"));
          GtkScale *freq = GTK_SCALE(gtk_builder_get_object(builder, "trig_freq"));
          GtkToggleButton *mute = GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder, "mute"));

          sig_info *trig_data = params->signals->value;
          node_insert_end(nodes, trig_data);

          // Append the row to the list, the last parameter must be at -1 to allow to append at the end
          gtk_list_box_insert(list, GTK_WIDGET(sine_box), -1);
          //g_print("id : %d\n", trig_data->id);

          g_signal_connect(G_OBJECT(delete_button), "clicked", G_CALLBACK(on_delete_node), (gpointer)&trig_data->id);
          g_signal_connect(G_OBJECT(trig_da), "draw", G_CALLBACK(on_draw_created_signal), (gpointer)&trig_data->id);
          g_signal_connect(G_OBJECT(amp), "value_changed", G_CALLBACK(on_scale_change_amp), (gpointer)&trig_data->id);
          g_signal_connect(G_OBJECT(freq), "value_changed", G_CALLBACK(on_scale_change_freq), (gpointer)&trig_data->id);
          g_signal_connect(G_OBJECT(mute), "toggled", G_CALLBACK(on_toggle), (gpointer)&trig_data->id);

          global_id += 1;

          gtk_range_set_value(GTK_RANGE(amp), trig_data->amp);
          gtk_range_set_value(GTK_RANGE(freq), trig_data->freq);

          gtk_widget_show_all(GTK_WIDGET(list));
          gtk_widget_show_all(GTK_WIDGET(trig_da));

          break;
        }
        case 2:
        {
          GtkBuilder *builder = gtk_builder_new();
          GError *error = NULL;
          if (gtk_builder_add_from_file(builder, "visualiser/glade_signals/saw.glade", &error) == 0)
          {
            g_printerr("Error loading file: %s\n", error->message);
            g_clear_error(&error);
          }

          GtkBox *sine_box = GTK_BOX(gtk_builder_get_object(builder, "saw_box"));
          GtkButton *delete_button = GTK_BUTTON(gtk_builder_get_object(builder, "delete"));
          GtkDrawingArea *saw_da = GTK_DRAWING_AREA(gtk_builder_get_object(builder, "saw_da"));
          GtkScale *amp = GTK_SCALE(gtk_builder_get_object(builder, "saw_amp"));
          GtkScale *freq = GTK_SCALE(gtk_builder_get_object(builder, "saw_freq"));
          GtkToggleButton *mute = GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder, "mute"));

          sig_info *saw_data = params->signals->value;
          node_insert_end(nodes, saw_data);

          // Append the row to the list, the last parameter must be at -1 to allow to append at the end
          gtk_list_box_insert(list, GTK_WIDGET(sine_box), -1);
          //g_print("id : %d\n", saw_data->id);

          g_signal_connect(G_OBJECT(delete_button), "clicked", G_CALLBACK(on_delete_node), (gpointer)&saw_data->id);
          g_signal_connect(G_OBJECT(saw_da), "draw", G_CALLBACK(on_draw_created_signal), (gpointer)&saw_data->id);
          g_signal_connect(G_OBJECT(amp), "value_changed", G_CALLBACK(on_scale_change_amp), (gpointer)&saw_data->id);
          g_signal_connect(G_OBJECT(freq), "value_changed", G_CALLBACK(on_scale_change_freq), (gpointer)&saw_data->id);
          g_signal_connect(G_OBJECT(mute), "toggled", G_CALLBACK(on_toggle), (gpointer)&saw_data->id);

          global_id += 1;

          gtk_range_set_value(GTK_RANGE(amp), saw_data->amp);
          gtk_range_set_value(GTK_RANGE(freq), saw_data->freq);

          gtk_widget_show_all(GTK_WIDGET(list));
          gtk_widget_show_all(GTK_WIDGET(saw_da));

          break;
        }
        case 3:
        {
          GtkBuilder *builder = gtk_builder_new();
          GError *error = NULL;
          if (gtk_builder_add_from_file(builder, "visualiser/glade_signals/saw_composite.glade", &error) == 0)
          {
            g_printerr("Error loading file: %s\n", error->message);
            g_clear_error(&error);
          }

          GtkBox *sine_box = GTK_BOX(gtk_builder_get_object(builder, "saw_box"));
          GtkButton *delete_button = GTK_BUTTON(gtk_builder_get_object(builder, "delete"));
          GtkDrawingArea *saw_da = GTK_DRAWING_AREA(gtk_builder_get_object(builder, "saw_da"));
          GtkScale *amp = GTK_SCALE(gtk_builder_get_object(builder, "saw_amp"));
          GtkScale *freq = GTK_SCALE(gtk_builder_get_object(builder, "saw_freq"));
          GtkScale *components = GTK_SCALE(gtk_builder_get_object(builder, "saw_components"));
          GtkToggleButton *mute = GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder, "mute"));

          sig_info *saw_data = params->signals->value;
          node_insert_end(nodes, saw_data);

          // Append the row to the list, the last parameter must be at -1 to allow to append at the end
          gtk_list_box_insert(list, GTK_WIDGET(sine_box), -1);
          //g_print("id : %d\n", saw_data->id);

          g_signal_connect(G_OBJECT(delete_button), "clicked", G_CALLBACK(on_delete_node), (gpointer)&saw_data->id);
          g_signal_connect(G_OBJECT(saw_da), "draw", G_CALLBACK(on_draw_created_signal), (gpointer)&saw_data->id);
          g_signal_connect(G_OBJECT(amp), "value_changed", G_CALLBACK(on_scale_change_amp), (gpointer)&saw_data->id);
          g_signal_connect(G_OBJECT(freq), "value_changed", G_CALLBACK(on_scale_change_freq), (gpointer)&saw_data->id);
          g_signal_connect(G_OBJECT(components), "value_changed", G_CALLBACK(on_scale_change_form), (gpointer)&saw_data->id);
          g_signal_connect(G_OBJECT(mute), "toggled", G_CALLBACK(on_toggle), (gpointer)&saw_data->id);

          global_id += 1;

          gtk_range_set_value(GTK_RANGE(amp), saw_data->amp);
          gtk_range_set_value(GTK_RANGE(freq), saw_data->freq);
          gtk_range_set_value(GTK_RANGE(components), saw_data->form);

          gtk_widget_show_all(GTK_WIDGET(list));
          gtk_widget_show_all(GTK_WIDGET(saw_da));

          break;
        }
        case 4:
        {
          GtkBuilder *builder = gtk_builder_new();
          GError *error = NULL;
          if (gtk_builder_add_from_file(builder, "visualiser/glade_signals/square.glade", &error) == 0)
          {
            g_printerr("Error loading file: %s\n", error->message);
            g_clear_error(&error);
          }

          GtkBox *sine_box = GTK_BOX(gtk_builder_get_object(builder, "square_box"));
          GtkButton *delete_button = GTK_BUTTON(gtk_builder_get_object(builder, "delete"));
          GtkDrawingArea *square_da = GTK_DRAWING_AREA(gtk_builder_get_object(builder, "square_da"));
          GtkScale *amp = GTK_SCALE(gtk_builder_get_object(builder, "square_amp"));
          GtkScale *freq = GTK_SCALE(gtk_builder_get_object(builder, "square_freq"));
          GtkScale *shift = GTK_SCALE(gtk_builder_get_object(builder, "square_shift"));
          GtkToggleButton *mute = GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder, "mute"));

          sig_info *square_data = params->signals->value;
          node_insert_end(nodes, square_data);

          // Append the row to the list, the last parameter must be at -1 to allow to append at the end
          gtk_list_box_insert(list, GTK_WIDGET(sine_box), -1);
          //g_print("id : %d\n", square_data->id);

          g_signal_connect(G_OBJECT(delete_button), "clicked", G_CALLBACK(on_delete_node), (gpointer)&square_data->id);
          g_signal_connect(G_OBJECT(square_da), "draw", G_CALLBACK(on_draw_created_signal), (gpointer)&square_data->id);
          g_signal_connect(G_OBJECT(amp), "value_changed", G_CALLBACK(on_scale_change_amp), (gpointer)&square_data->id);
          g_signal_connect(G_OBJECT(freq), "value_changed", G_CALLBACK(on_scale_change_freq), (gpointer)&square_data->id);
          g_signal_connect(G_OBJECT(shift), "value_changed", G_CALLBACK(on_scale_change_form), (gpointer)&square_data->id);
          g_signal_connect(G_OBJECT(mute), "toggled", G_CALLBACK(on_toggle), (gpointer)&square_data->id);

          global_id += 1;

          gtk_range_set_value(GTK_RANGE(amp), square_data->amp);
          gtk_range_set_value(GTK_RANGE(freq), square_data->freq);
          gtk_range_set_value(GTK_RANGE(shift), square_data->form);

          gtk_widget_show_all(GTK_WIDGET(list));
          gtk_widget_show_all(GTK_WIDGET(square_da));
          break;
        }
        default:

          break;
        }
        params->signals = params->signals->next;
      }
      node_free(params->signals);
      free(params);
    }
    
    g_object_unref(file);
    free(pointer);
    *load = 1;
    return G_SOURCE_REMOVE;
  }
  else
  {
    return G_SOURCE_REMOVE;
  }
}

static gboolean on_save_state(GtkButton *a_button, gpointer data)
{
  write_to_triton(nodes);
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
    int p = poll(pfds, npfds, 40);
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
    update_effects(my_data);
    apply_filter_to_sample(my_data, 1024);
  }
}

static gboolean
on_adsr_draw(GtkWidget *widget, cairo_t *cr, gpointer user_data)
{

  vis_data *vs = (vis_data *)user_data;
  int zoom_x = 100;
  int zoom_y = 100;

  GdkRectangle da;            /* GtkDrawingArea size */
  gdouble dx = 2.0, dy = 2.0; /* Pixels between each point */
  gdouble i, clip_x1 = 0.0, clip_y1 = 0.0, clip_x2 = 0.0, clip_y2 = 0.0;

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

  i = clip_x1;

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

int gtk_run_zbi(vis_data *vis_d, int argc, char **argv)
{
  GThread *thread[N_THREADS];
  int loaded = 0;
  gtk_init(&argc, &argv);
  nodes = node_build_sentinel();

  GtkBuilder *builder = gtk_builder_new();
  GError *error = NULL;
  if (gtk_builder_add_from_file(builder, "visualiser/plain.glade", &error) == 0)
  {
    g_printerr("Error loading file: %s\n", error->message);
    g_clear_error(&error);
    return 1;
  }

  // Signal

  list = GTK_LIST_BOX(gtk_builder_get_object(builder, "list"));

  GtkFileChooser *my_file_chooser = GTK_FILE_CHOOSER(gtk_builder_get_object(builder, "file_load"));
  g_signal_connect(my_file_chooser, "update-preview", G_CALLBACK(update_preview_cb), &loaded);

  GtkButton *save_file = GTK_BUTTON(gtk_builder_get_object(builder,"save_file"));
  g_signal_connect(G_OBJECT(save_file),"clicked",G_CALLBACK(on_save_state),NULL);
  GtkButton *sine_submit = GTK_BUTTON(gtk_builder_get_object(builder, "sine_submit"));
  GtkButton *square_submit = GTK_BUTTON(gtk_builder_get_object(builder, "square_submit"));
  GtkButton *trig_submit = GTK_BUTTON(gtk_builder_get_object(builder, "trig_submit"));
  GtkButton *saw_submit = GTK_BUTTON(gtk_builder_get_object(builder, "saw_submit"));
  GtkButton *saw1_submit = GTK_BUTTON(gtk_builder_get_object(builder, "saw1_submit"));

  GtkScale *freq = GTK_SCALE(gtk_builder_get_object(builder, "global_freq"));

  GtkDrawingArea *full_sig = GTK_DRAWING_AREA(gtk_builder_get_object(builder, "full_sig"));

  // Window
  GtkWindow *window = GTK_WINDOW(gtk_builder_get_object(builder, "org.gtk.duel"));

  // Title of the window
  gtk_window_set_title(GTK_WINDOW(window), "filter app");

  // Drawing areas
  GtkDrawingArea *da_signal = GTK_DRAWING_AREA(gtk_builder_get_object(builder, "area"));
  GtkDrawingArea *da_harmonics = GTK_DRAWING_AREA(gtk_builder_get_object(builder, "harmonics_da"));
  // ADSR da
  GtkDrawingArea *da = GTK_DRAWING_AREA(gtk_builder_get_object(builder, "da"));

  // SpinButtons
  GtkSpinButton *spx = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "X_ZOOM"));
  GtkSpinButton *spy = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "Y_ZOOM"));
  // ADSR Spin Buttons
  GtkSpinButton *attack_phase = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "attack_phase_control"));
  GtkSpinButton *decay_phase = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "decay_phase_control"));
  GtkSpinButton *release_phase = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "release_phase_control"));

  // Scale
  GtkScale *low_pass_cutoff = GTK_SCALE(gtk_builder_get_object(builder, "low_pass_cut"));
  GtkScale *high_pass_cutoff = GTK_SCALE(gtk_builder_get_object(builder, "high_pass_cut"));
  GtkScale *band_pass_low = GTK_SCALE(gtk_builder_get_object(builder, "band_pass_low"));
  GtkScale *band_pass_high = GTK_SCALE(gtk_builder_get_object(builder, "band_pass_high"));
  GtkScale *band_cut_low = GTK_SCALE(gtk_builder_get_object(builder, "band_cut_low"));
  GtkScale *band_cut_high = GTK_SCALE(gtk_builder_get_object(builder, "band_cut_high"));

  // ADSR Scales
  GtkScale *attack_bot = GTK_SCALE(gtk_builder_get_object(builder, "attack_bot"));
  GtkScale *attack_top = GTK_SCALE(gtk_builder_get_object(builder, "attack_top"));
  GtkScale *decay_bot = GTK_SCALE(gtk_builder_get_object(builder, "decay_bot"));
  GtkScale *sustain = GTK_SCALE(gtk_builder_get_object(builder, "sustain"));

  // Check Buttons
  GtkCheckButton *low_pass_activate = GTK_CHECK_BUTTON(gtk_builder_get_object(builder, "low_pass_activate"));
  GtkCheckButton *high_pass_activate = GTK_CHECK_BUTTON(gtk_builder_get_object(builder, "high_pass_activate"));
  GtkCheckButton *band_pass_activate = GTK_CHECK_BUTTON(gtk_builder_get_object(builder, "band_pass_activate"));
  GtkCheckButton *band_cut_activate = GTK_CHECK_BUTTON(gtk_builder_get_object(builder, "band_cut_activate"));

  // Unreference the objects
  g_object_unref(builder);

  // Signals

  g_signal_connect(G_OBJECT(window), "key_release_event", G_CALLBACK(key_released), (gpointer)vis_d->data);

  // Destroy signal
  g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(on_destroy), &vis_d->stop_thread);

  // Signal on the x_zoom value
  g_signal_connect(G_OBJECT(spx), "value_changed", G_CALLBACK(on_x), &vis_d->x_zoom);
  // Signal on the y_zoom value
  g_signal_connect(G_OBJECT(spy), "value_changed", G_CALLBACK(on_y), &vis_d->y_zoom);

  // Signal to the low_pass_activate
  g_signal_connect(G_OBJECT(low_pass_activate), "toggled", G_CALLBACK(on_activate), &vis_d->low_active);
  // Signal to the high_pass_activate
  g_signal_connect(G_OBJECT(high_pass_activate), "toggled", G_CALLBACK(on_activate), &vis_d->high_active);
  // Signal to the band_pass_active
  g_signal_connect(G_OBJECT(band_pass_activate), "toggled", G_CALLBACK(on_activate), &vis_d->band_pass_active);
  // Signal to the band_cut_active
  g_signal_connect(G_OBJECT(band_cut_activate), "toggled", G_CALLBACK(on_activate), &vis_d->band_cut_active);

  // ADSR signals

  g_signal_connect(G_OBJECT(attack_phase), "value_changed", G_CALLBACK(on_spinner_change), &vis_d->attack_phase);
  g_signal_connect(G_OBJECT(decay_phase), "value_changed", G_CALLBACK(on_spinner_change), &vis_d->decay_phase);
  g_signal_connect(G_OBJECT(release_phase), "value_changed", G_CALLBACK(on_spinner_change), &vis_d->release_phase);

  g_signal_connect(G_OBJECT(attack_bot), "value_changed", G_CALLBACK(on_scale_change), &vis_d->attack_amp);
  g_signal_connect(G_OBJECT(attack_top), "value_changed", G_CALLBACK(on_scale_change), &vis_d->decay_amp);
  g_signal_connect(G_OBJECT(decay_bot), "value_changed", G_CALLBACK(on_scale_change), &vis_d->sustain_amp);
  g_signal_connect(G_OBJECT(sustain), "value_changed", G_CALLBACK(on_scale_change), &vis_d->sustain_amp);

  g_signal_connect(G_OBJECT(da), "draw", G_CALLBACK(on_adsr_draw), vis_d);

  GtkMultipleScales *band_cut_data_low = malloc(sizeof(GtkMultipleScales));
  band_cut_data_low->data = vis_d;
  band_cut_data_low->lead = band_cut_high;

  GtkMultipleScales *band_cut_data_high = malloc(sizeof(GtkMultipleScales));
  band_cut_data_high->data = vis_d;
  band_cut_data_high->lead = band_cut_low;

  g_signal_connect(G_OBJECT(band_cut_low), "value_changed", G_CALLBACK(on_scale_band_cut_change_low), (gpointer)band_cut_data_low);
  g_signal_connect(G_OBJECT(band_cut_high), "value_changed", G_CALLBACK(on_scale_band_cut_change_high), (gpointer)band_cut_data_high);

  GtkMultipleScales *band_pass_data_low = malloc(sizeof(GtkMultipleScales));
  band_pass_data_low->data = vis_d;
  band_pass_data_low->lead = band_pass_high;

  GtkMultipleScales *band_pass_data_high = malloc(sizeof(GtkMultipleScales));
  band_pass_data_high->data = vis_d;
  band_pass_data_high->lead = band_pass_low;

  g_signal_connect(G_OBJECT(band_pass_low), "value_changed", G_CALLBACK(on_scale_band_change_low), (gpointer)band_pass_data_low);
  g_signal_connect(G_OBJECT(band_pass_high), "value_changed", G_CALLBACK(on_scale_band_change_high), (gpointer)band_pass_data_high);

  g_signal_connect(G_OBJECT(low_pass_cutoff), "value_changed", G_CALLBACK(on_scale_change), &vis_d->low_pass_cut);
  g_signal_connect(G_OBJECT(high_pass_cutoff), "value_changed", G_CALLBACK(on_scale_change), &vis_d->high_pass_cut);

  // Signal on the drawing area of the signal
  g_signal_connect(G_OBJECT(da_signal), "draw", G_CALLBACK(on_draw_signal), vis_d);
  // Signal on the drawing area of the harmonic
  g_signal_connect(G_OBJECT(da_harmonics), "draw", G_CALLBACK(on_draw_harmonics), vis_d);

  g_signal_connect(G_OBJECT(sine_submit), "clicked", G_CALLBACK(row_sine), NULL);
  g_signal_connect(G_OBJECT(trig_submit), "clicked", G_CALLBACK(row_trig), NULL);
  g_signal_connect(G_OBJECT(saw_submit), "clicked", G_CALLBACK(row_saw), NULL);
  g_signal_connect(G_OBJECT(saw1_submit), "clicked", G_CALLBACK(row_saw_composite), NULL);
  g_signal_connect(G_OBJECT(square_submit), "clicked", G_CALLBACK(row_square), NULL);

  g_signal_connect(G_OBJECT(freq), "value_changed", G_CALLBACK(on_scale_change_global_freq), NULL);

  g_signal_connect(G_OBJECT(full_sig), "draw", G_CALLBACK(on_draw_full_signal), NULL);

  context = g_main_context_default();

  for (int n = 0; n < N_THREADS; ++n)
    thread[n] = g_thread_new(NULL, thread_func, vis_d);

  gtk_widget_show_all(GTK_WIDGET(window));

  gtk_main();

  for (int n = 0; n < N_THREADS; ++n)
    g_thread_join(thread[n]);

  free(band_cut_data_low);
  free(band_cut_data_high);
  free(band_pass_data_low);
  free(band_pass_data_high);

  return 0;
}
