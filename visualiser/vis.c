#include "vis.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>
#include <gtk/gtk.h>
#include <pthread.h>

#define SCROLL_WINDOW_HEIGHT 480
#define SCROLL_WINDOW_WIDTH 320

#define NUMBER_OF_KEYS 127
#define MINUTE 60

#define FULL_WIDTH 24000
#define FULL_HEIGHT 1270

#define RECT_HEIGHT FULL_HEIGHT / NUMBER_OF_KEYS
#define RECT_WIDTH FULL_WIDTH / (MINUTE * 20)

typedef struct rectangle
{
  int x;
  int y;
  int width;
  int height;
  int x_ext;
  int y_ext;
  int id;
  int id_ext;
} rectangle;

typedef struct rect_node
{
  struct rectangle *value;
  struct rect_node *next;
} rect_node;

rectangle *init_null_rectangle()
{
  rectangle *new_rectangle = malloc(sizeof(rectangle));
  new_rectangle->height = 0;
  new_rectangle->width = 0;
  new_rectangle->x = 0;
  new_rectangle->y = 0;
  new_rectangle->x_ext = 0;
  new_rectangle->y_ext = 0;
  new_rectangle->id = -1;
  new_rectangle->id_ext = -1;
  return new_rectangle;
}

rectangle *init_rectangle(int x, int y, int height, int width, int x_ext, int y_ext, int id)
{
  rectangle *new_rectangle = malloc(sizeof(rectangle));
  new_rectangle->height = height;
  new_rectangle->width = width;
  new_rectangle->x = x;
  new_rectangle->y = y;
  new_rectangle->x_ext = x_ext;
  new_rectangle->y_ext = y_ext;
  new_rectangle->id = id;
  new_rectangle->id_ext = id_from_coordinate(x_ext, y_ext);
  return new_rectangle;
}

rectangle *init_rectangle_from_press(int key_id, ud *data)
{
  rectangle *new_rectangle = malloc(sizeof(rectangle));

  return new_rectangle;
}

void print_rectangle(rectangle *rect)
{
  printf(" Id: %d --->  x: %d, y: %d, x_ext: %d, y_ext: %d, width: %d, height: %d\n", rect->id, rect->x, rect->y, rect->x_ext, rect->y_ext, rect->width, rect->height);
}

// Verify if rect_node is empty
int rect_node_is_empty(rect_node *head)
{
  return head == NULL;
}

// Verify if rect_node is not empty
int rect_node_is_not_empty(rect_node *head)
{
  return head != NULL;
}

// Builds the sentinell of the rect_node structure
rect_node *rect_node_build_sentinel()
{
  // Creates the sentinel.
  rect_node *head = malloc(sizeof(rect_node));
  head->value = init_null_rectangle();
  head->next = NULL;
  // Returns the head of the rect_node which is the sentinell.
  return head;
}

// Prints the contents of a rect_node rect_node* rect_node_build_sentinel()
void rect_node_print(rect_node *head)
{
  while (head->next)
  {
    head = head->next;
    print_rectangle(head->value);
  }
}

// Frees the allocated rect_node
void rect_node_free(rect_node *head)
{
  rect_node *previous;

  while (head)
  {
    previous = head;
    head = head->next;
    free(previous->value);
    free(previous);
  }
}

// Inserts a value right after the head
/*

    HEAD -> 1 -> 2 -> ..... -> 8
    rect_node_insert_beg(rect_node* HEAD, int 42);
    HEAD -> 42 -> 1 -> 2 -> ..... -> 8

*/
void rect_node_insert_beg(rect_node *head, rectangle *value)
{
  rect_node *tmp = malloc(sizeof(rect_node));
  tmp->value = value;
  tmp->next = head->next;
  head->next = tmp;
}

// Inserts a value right after the head
/*

    HEAD -> 1 -> 2 -> ..... -> 8
    rect_node_insert_end(rect_node* HEAD, int 42);
    HEAD -> 1 -> 2 -> ..... -> 8 -> 42

*/
void rect_node_insert_end(rect_node *head, rectangle *value)
{
  rect_node *tmp = malloc(sizeof(rect_node));
  for (; rect_node_is_not_empty(head->next); head = head->next)
  {
    // This loop runs to the last rect_node and quits with head being that last rect_node
    continue;
  }
  tmp->value = value;
  tmp->next = head->next;
  head->next = tmp;
}

// Deletes the first occurence of value in rect_node
int rect_node_delete_first_occurence(rect_node *head, rect_node *sup, int id)
{
  int seen = 0;
  rect_node *tmp = head;
  while (head->next != NULL)
  {
    if (head->next->value->id == id)
    {
      tmp = head;
      seen += 1;
      break;
    }
    head = head->next;
  }
  if (seen == 0)
  {
    return seen;
  }
  rect_node *output = head->next;
  tmp->next = tmp->next->next;
  output->next = NULL;
  while (sup->next != NULL)
  {
    sup = sup->next;
  }
  sup->next = output;
  return seen;
}

// Deletes all occurences of value in rect_node
int rect_node_delete_all_occurences(rect_node *head, rect_node *sup, int id)
{
  int seen = 0;
  rect_node *tmp = head;
  while (head->next != NULL)
  {
    if (head->next->value->id == id)
    {
      seen += 1;
      tmp = head;
      rect_node *output = head->next;
      tmp->next = tmp->next->next;
      output->next = NULL;
      while (sup->next != NULL)
      {
        sup = sup->next;
      }
      sup->next = output;
      continue;
    }
    head = head->next;
  }
  return seen;
}

// Get a rect_node at index if index invalid return NULL
// DOES NOT DELETE THE rect_node
rect_node *rect_node_get_at(rect_node *rect_node, int index)
{
  while (index > 0)
  {
    rect_node = rect_node->next;

    index--;
  }
  if (rect_node != NULL)
  {
    rect_node = rect_node->next;
  }
  return rect_node;
}

int rect_node_val_count(rect_node *head)
{
  int cpt = 0;
  while (head->next != NULL)
  {
    cpt += 1;
    head = head->next;
  }
  return cpt;
}

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int x_midi;
int y_midi;
int press_set = 0;
int grabbed = 0;
int wait = 0;

rect_node *rect_list;
rect_node *deleted_rects;

// If 1 theres a rect, 0 if not
char *global_rect_table;

typedef struct cairo_surfaces
{
  cairo_surface_t *main_surface, *seen_surface;
} cairo_surfaces;

// An null event has an id of -1 and is_on at -1
typedef struct key_event
{
  int is_on;
  int id;
} key_event;

key_event *create_event(int is_on, int id)
{
  key_event *new_event = malloc(sizeof(key_event));
  new_event->is_on = is_on;
  new_event->id = id;
  return new_event;
}

void set_closest_rectangle()
{
  // First check if the square has no rectangle already
  int new_x = x_midi % (RECT_WIDTH);
  int new_y = y_midi % (RECT_HEIGHT);
  x_midi -= new_x;
  y_midi -= new_y;
}

int id_from_coordinate(int x, int y)
{
  int id = (y / (RECT_HEIGHT)) * MINUTE * 20 + x / (RECT_WIDTH);
  // printf("id: %d\n", id);
  return id;
}

int is_id_in_rectangle(rectangle *rect, int id)
{
  int top_id = id_from_coordinate(rect->x_ext, rect->y_ext);
  // printf("rect->id %d , id %d , top_id %d\n", rect->id, id, top_id);
  return id >= rect->id && id <= top_id;
}

// Gets the current event and sets the x,y possition
gboolean ccurrent_key_click_midi(GtkWidget *da, GdkEvent *event, __attribute_maybe_unused__ gpointer user_data)
{
  GdkDisplay *display = gdk_display_get_default();
  GdkSeat *seat = gdk_display_get_default_seat(display);
  GdkDevice *device = gdk_seat_get_pointer(seat);
  // If the double press was issued
  // start the delete sequence

  gdk_window_get_device_position(gtk_widget_get_window(GTK_WIDGET(da)), device, &x_midi, &y_midi, NULL);
  set_closest_rectangle();
  int id = id_from_coordinate(x_midi, y_midi);
  switch (gdk_event_get_event_type(event))
  {
  case GDK_2BUTTON_PRESS:
    rect_node *list = rect_list;
    while (list->next)
    {
      list = list->next;
      if (is_id_in_rectangle(list->value, id))
      {
        int top_id = id_from_coordinate(list->value->x_ext, list->value->y_ext);
        // Liberate all the rects that it occupied
        for (int i = list->value->id; i < top_id; i++)
        {
          global_rect_table[i] = 0;
        }
        // Delete the actuall rect
        rect_node_delete_first_occurence(rect_list, deleted_rects, list->value->id);
      }
    }
    break;
  case GDK_BUTTON_PRESS:
    press_set = 1;
    // Create the rectangle if there is no rectangle at this coordinate
    if (global_rect_table[id] == 0)
    {
      rectangle *new_rect = init_rectangle(x_midi, y_midi, RECT_HEIGHT, RECT_WIDTH, x_midi + RECT_WIDTH, y_midi, id);
      rect_node_insert_beg(rect_list, new_rect);
      global_rect_table[new_rect->id] = 1;
    }
    break;
  case GDK_MOTION_NOTIFY:
    grabbed = 1;
    // Update the area of the rectangle if the id is unoccupied
    if (global_rect_table[id] == 0)
    {

      rectangle *dragged_rect = rect_list->next->value;
      // Only allow updates on the same line
      if (x_midi != dragged_rect->x && y_midi == dragged_rect->y)
      {
        // No negative distance possible
        int new_width = x_midi - dragged_rect->x;
        if (new_width > 0)
        {
          dragged_rect->width = new_width;
        }
      }
    }
    break;
  case GDK_BUTTON_RELEASE:
    press_set = 0;
    // If it was not dragged it was a simple release and no updates have to be made
    // Or it was a delete event
    if (rect_list->next != NULL)
    {
      rectangle *dragged_rect = rect_list->next->value;
      int width = dragged_rect->width;

      // Updtae the exterior points
      dragged_rect->x_ext = x_midi;
      dragged_rect->y_ext = y_midi;
      int id_ext = id_from_coordinate(x_midi, y_midi);
      if (grabbed)
      {
        grabbed = 0;
        int j = 0;
        for (int i = 0; i < width; i += RECT_WIDTH)
        {
          // printf("width %d blocked id %d\n", width, dragged_rect->id + j);
          global_rect_table[dragged_rect->id + j] = 1;
          j++;
        }
      }
      else
      {
        // printf("width %d blocked id %d\n", width, dragged_rect->id);
        global_rect_table[dragged_rect->id] = 1;
      }
    }
    break;
  default:
    // printf("different\n");
    break;
  }

  return G_SOURCE_REMOVE;
}

void on_midi_quit(__attribute_maybe_unused__ GtkWidget *widget, gpointer user_data)
{
  cairo_surfaces *surfaces;
  surfaces = (cairo_surfaces *)user_data;
  cairo_surface_destroy(surfaces->main_surface);
  cairo_surface_destroy(surfaces->seen_surface);
  gtk_main_quit();
}

gboolean on_configure(GtkWidget *widget, __attribute_maybe_unused__ GdkEventConfigure *event_p, gpointer user_data)
{
  cairo_t *cr_p;
  cairo_surfaces *surfaces;

  surfaces = (cairo_surfaces *)user_data;

  if (surfaces->seen_surface)
  {
    cairo_surface_destroy(surfaces->seen_surface);
  }

  surfaces->seen_surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, SCROLL_WINDOW_HEIGHT, SCROLL_WINDOW_WIDTH);

  gtk_widget_set_size_request(widget, FULL_WIDTH, FULL_HEIGHT);

  cr_p = cairo_create(surfaces->seen_surface);
  cairo_set_source_surface(cr_p, surfaces->main_surface, 0, 0);
  cairo_paint(cr_p);
  cairo_destroy(cr_p);
  return FALSE;
}

// Sets up the drawing space to dynamically manage the resize
void set_up_midi_grid(GtkWidget *widget, cairo_t *cr, double *dx, double *dy, double *clip_y1, double *clip_y2, double *clip_x1, double *clip_x2, GdkRectangle *da)
{
  GdkWindow *window = gtk_widget_get_window(widget);

  /* Determine GtkDrawingArea dimensions */
  gdk_window_get_geometry(window, &da->x, &da->y, &da->width, &da->height);

  /* Draw on a black background */
  cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
  cairo_paint(cr);

  /* Determine the data points to calculate (ie. those in the clipping zone */
  cairo_device_to_user_distance(cr, dx, dy);
  cairo_clip_extents(cr, clip_x1, clip_y1, clip_x2, clip_y2);
}

// Draws the grid on which the rectangles are placed
void on_draw_midi_grid(cairo_t *cr, double dx, double clip_y2, double clip_x2)
{
  // Horizontal lines
  for (size_t i = 0; i < FULL_WIDTH; i += (FULL_WIDTH / MINUTE))
  {
    cairo_set_source_rgb(cr, 0.2, 0.2, 0.2);
    cairo_move_to(cr, i, 0.0);
    cairo_line_to(cr, i, clip_y2);
    cairo_stroke(cr);
    cairo_set_line_width(cr, dx / 4);
    cairo_set_source_rgb(cr, 0.1, 0.1, 0.1);
    for (size_t j = 0; j < (FULL_WIDTH / MINUTE); j += (FULL_WIDTH / (MINUTE * 20)))
    {
      cairo_move_to(cr, j + i, 0.0);
      cairo_line_to(cr, j + i, clip_y2);
    }
    cairo_stroke(cr);
  }

  // vertical lines
  for (size_t i = 0; i < FULL_HEIGHT; i += FULL_HEIGHT / NUMBER_OF_KEYS)
  {
    cairo_set_source_rgb(cr, 0.2, 0.2, 0.2);
    cairo_move_to(cr, 0.0, i);
    cairo_line_to(cr, clip_x2, i);
    cairo_stroke(cr);
  }
}

// Draws one rectangle on the cr, with left top most corner (x,y)
void on_draw_midi_rectangle(cairo_t *cr, int x, int y, int width, int height)
{
  cairo_set_source_rgb(cr, 0.2, 0.6, 0.1);
  cairo_move_to(cr, x, y);
  cairo_line_to(cr, x, y + height);
  cairo_line_to(cr, x + width, y + height);
  cairo_line_to(cr, x + width, y);
  cairo_line_to(cr, x, y);
  cairo_fill(cr);

  cairo_set_source_rgb(cr, 0.2, 0.9, 0.5);

  cairo_set_line_width(cr, 2);
  cairo_move_to(cr, x, y);
  cairo_line_to(cr, x, y + height);
  cairo_line_to(cr, x + width, y + height);
  cairo_line_to(cr, x + width, y);
  cairo_line_to(cr, x, y);
  cairo_stroke(cr);
}

gboolean on_draw_midi(GtkWidget *widget, cairo_t *cr, __attribute_maybe_unused__ gpointer user_data)
{
  GdkRectangle da; /* GtkDrawingArea size */
  rect_node *list = rect_list;
  gdouble dx = 4.0, dy = 4.0; /* Pixels between each point */
  gdouble clip_x1 = 0.0, clip_y1 = 0.0, clip_x2 = 0.0, clip_y2 = 0.0;

  set_up_midi_grid(widget, cr, &dx, &dy, &clip_y1, &clip_y2, &clip_x1, &clip_x2, &da);

  on_draw_midi_grid(cr, dx, clip_y2, clip_x2);

  while (list->next)
  {
    list = list->next;
    on_draw_midi_rectangle(cr, list->value->x, list->value->y, list->value->width, list->value->height);
  }
  gtk_widget_queue_draw_area(widget, 0, 0, da.width, da.height);
  return G_SOURCE_REMOVE;
}

/*


MIDI time start


*/

double start_midi_rec = 0;

// Issued on A PRESS OF THE KEY!!!
rectangle *init_rect_from_key(int id_note, ud *data)
{
  rectangle *new = malloc(sizeof(rectangle));
  // In seconds
  double placement_time = data->time_management->actual_time - start_midi_rec;

  int x = placement_time * 20 * RECT_WIDTH;
  int y = id_note * RECT_HEIGHT;

  int new_x = x % (RECT_WIDTH);
  int new_y = y % (RECT_HEIGHT);
  x -= new_x;
  y -= new_y;

  int id = id_from_coordinate(x, y);
  rectangle *new_rect = init_rectangle(x, y, RECT_HEIGHT, RECT_WIDTH, x + RECT_WIDTH, y, id);
  rect_node_insert_beg(rect_list, new_rect);
  global_rect_table[new_rect->id] = 1;
}

// Issued on A RELEASE OF A KEY
rectangle *release_rect_from_key(int id_note, ud *data)
{
  double placement_time = data->time_management->actual_time - start_midi_rec;
  rect_node *rect_run = rect_list;
  while (rect_run->next != NULL)
  {
    rect_run = rect_run->next;
    int note_id = rect_run->value->id / (MINUTE * 20);
    if (note_id == id_note)
    {
      int x = placement_time * 20 * RECT_WIDTH;

      int new_x = x % (RECT_WIDTH);
      x -= new_x;
      int id = id_from_coordinate(x, rect_run->value->y);
      rect_run->value->x_ext = x;
      rect_run->value->y_ext = rect_run->value->y;
      rect_run->value->width = RECT_WIDTH * (id - rect_run->value->id);
      break;
    }
  }
}

void record_event_array(GtkWidget *widget, gpointer user_data)
{
  ud *data = (ud *)user_data;
  start_midi_rec = data->time_management->actual_time;
}

void construct_event_array(GtkWidget *widget, gpointer user_data)
{
  ud *data = (ud *)user_data;
  double global_snap = data->time_management->actual_time + 30;
  for (size_t i = 0; i < MINUTE * 20; i++)
  {
    int *id_arr = calloc(NUMBER_OF_KEYS, sizeof(int));
    for (size_t j = 0; j < NUMBER_OF_KEYS; j++)
    {
      int id = j * MINUTE * 20 + i;
      int note_id = id / (MINUTE * 20);
      rect_node *list = rect_list;
      while (list->next)
      {
        list = list->next;
        if (id == list->value->id)
        {
          id_arr[note_id] = 2;
        }
        // If it is the exteerior of the sound
        if (id == id_from_coordinate(list->value->x_ext, list->value->y_ext))
        {
          id_arr[note_id] = 1;
        }
      }
    }
    double snap = data->time_management->actual_time;
    // One line of notes is done
    while (snap + 0.05 > data->time_management->actual_time && global_snap > data->time_management->actual_time)
    {
      for (size_t i = 0; i < 127; i++)
      {
        if (id_arr[i] == 2)
        {
          key_on(data, i);
          data->all_keys->keys[i] = 1;
        }
        if (id_arr[i] == 1)
        {
          key_off(data, i);
          data->all_keys->keys[i] = 0;
        }
      }
    }
    free(id_arr);
  }
  
}

void empty_event_array(GtkWidget *widget, gpointer user_data)
{
  for (size_t i = 0; i < 20*MINUTE*NUMBER_OF_KEYS; i++)
  {
    rect_node_delete_first_occurence(rect_list,deleted_rects,i);
  }
  
}

typedef struct pth_and_ud
{
  pthread_t *thr;
  ud *data;
} pth_and_ud;

snd_seq_t *midi_seq;
GMainContext *context;
// File names to add new basic signals
char global_file_name[] = "visualiser/glade_signals/signal.glade";
char global_file_name_components[] = "visualiser/glade_signals/signal_components.glade";

// The nodes of the different signals
node *nodes;
last_events_stack *last_events;

// The current highest id
int global_id = 0;

// The list in which the widgets are stored
GtkListBox *list;

// The global frequency controller
float global_freq = 0.0;

int global_page_id = 0;

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
  else if (event->keyval == GDK_KEY_z)
  {
    // Pop the last venet from the stack and insert it to nodes
    sig_info *poped_sig = stack_pop(last_events);
    if (poped_sig != NULL)
    {
      poped_sig->id = global_id;
      // print_sine_info(poped_sig);
      node_insert_end(nodes, poped_sig);
      if (poped_sig->type < 4)
      {
        row_create(NULL, poped_sig);
      }
      else
      {
        row_create_composite(NULL, poped_sig);
      }
    }
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

  float factor;
  if (sig_sum == 0)
  {
    factor = clip_x2;
  }
  else
  {
    factor = clip_x2 / sig_sum;
  }

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
  float *buffer = malloc(sizeof(float) * 2);
  for (size_t i = 0; i < data->fout_size; i++)
  {
    if (read_from_wav(data->fout, buffer) == 2)
    {
      data->recorded_sig[i] = buffer[0];
    }
    else
    {
      break;
    }
  }
}

void fliter_param_fill(float *filter_param, vis_data *data)
{
  filter_param[0] = data->low_pass_cut;
  filter_param[1] = data->high_pass_cut;
  filter_param[2] = data->band_pass_low;
  filter_param[3] = data->band_pass_high;
  filter_param[4] = data->band_cut_low;
  filter_param[5] = data->band_cut_high;
  filter_param[6] = data->low_active;
  filter_param[7] = data->high_active;
  filter_param[8] = data->band_pass_active;
  filter_param[9] = data->band_cut_active;
}

int filter_param_comp(float *new, float *old)
{
  for (size_t i = 0; i < 10; i++)
  {
    if (new[i] != old[i])
    {
      return 1;
    }
  }
  return 0;
}

// Main loop of the app
void run_app(vis_data *my_data)
{
  ud *data = my_data->data;
  ADSR *new_adsr = init_ADSR_envelope(my_data->attack_phase, my_data->decay_phase, my_data->release_phase, my_data->attack_amp, my_data->decay_amp, my_data->sustain_amp);
  my_data->data->adsr = new_adsr;

  float *filter_params = calloc(10, sizeof(float));
  float *filter_params_past = calloc(10, sizeof(float));

  data->fout = open_WAV("Bach.wav");
  data->fout_size = findSize("Bach.wav");

  struct pollfd *pfds;
  int npfds;

  init_seq();
  connect_to_port("0");
  midi_seq = create_port();
  connect_ports(midi_seq);

  npfds = snd_seq_poll_descriptors_count(midi_seq, POLLIN);
  pfds = malloc(sizeof(*pfds) * npfds);

  while (my_data->stop_thread)
  {
    fliter_param_fill(filter_params_past, my_data);

    snd_seq_poll_descriptors(midi_seq, pfds, npfds, POLLIN);
    int p = poll(pfds, npfds, 20);
    if (p != 0)
    {
      snd_seq_event_t *event_;
      snd_seq_event_input(midi_seq, &event_);
      if (event_)
      {
        switch (event_->type)
        {
        case SND_SEQ_EVENT_NOTEON:
          if (event_->data.note.velocity)
          {
            data->all_keys->keys[(int)event_->data.note.note] = 1;
            key_on(data, (int)event_->data.note.note);
            init_rect_from_key((int)event_->data.note.note, data);
            printf("On %d\n", (int)event_->data.note.note);
          }
          else
          {
            data->all_keys->keys[(int)event_->data.note.note] = 0;
            key_off(data, (int)event_->data.note.note);
            release_rect_from_key((int)event_->data.note.note, data);
            printf("Off %d\n", (int)event_->data.note.note);
          }
          break;
        case SND_SEQ_EVENT_NOTEOFF:
          data->all_keys->keys[(int)event_->data.note.note] = 0;
          key_off(data, (int)event_->data.note.note);
          release_rect_from_key((int)event_->data.note.note, data);
          printf("Off %d\n", (int)event_->data.note.note);
          break;
        }
      }
    }
    update_effects(my_data);
    // The graphic calculus slows the programm!!!
    // Get the current page and if it is filters than do the calc!
    if (global_page_id == 3)
    {
      fliter_param_fill(filter_params, my_data);
      if (filter_param_comp(filter_params, filter_params_past))
      {
        apply_filter_to_sample(my_data, 1024);
      }
    }
  }
  // Write to the last_session file
  write_to_triton(nodes, "visualiser/last_session/last_session.triton");
  rect_node_print(rect_list);
  free(filter_params);
  free(filter_params_past);
  free(pfds);
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

// Tracks the current page of the app to block all the processes not involved on that particular page!
gboolean on_switch_page(GtkWidget *note_book, __attribute_maybe_unused__ gpointer user_data)
{
  gint current_page = gtk_notebook_get_current_page(GTK_NOTEBOOK(note_book));
  // printf("curr %d\n",current_page);
  global_page_id = current_page;
  return G_SOURCE_REMOVE;
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
  cairo_translate(cr, da_parameters.width / 2, da_parameters.height / 2);
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
  size_t cpt = 0;
  for (i = clip_x1; i < clip_x2; i += (clip_x2 * 16 / data->fout_size))
  {
    if (data->wav_manager->playback)
    {
    }

    if (cpt < data->fout_size)
    {
      cairo_line_to(cr, i, data->recorded_sig[cpt] * clip_y2);
    }

    cpt += 1;
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

  pthread_t thr;
  cairo_surfaces my_data;
  rect_list = rect_node_build_sentinel();
  deleted_rects = rect_node_build_sentinel();
  global_rect_table = calloc(NUMBER_OF_KEYS * MINUTE * 20, sizeof(char));

  my_data.seen_surface = NULL;
  my_data.main_surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, FULL_WIDTH, FULL_HEIGHT);

  // The table of threads
  GThread *thread[1];
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
  // g_signal_connect(G_OBJECT(da_record), "draw", G_CALLBACK(on_draw_recorded), vis_d->data);

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
      - GtkNoteBook
  */

  // Main Window
  GtkWindow *window = GTK_WINDOW(gtk_builder_get_object(builder, "main_window"));
  // FileChooser
  GtkFileChooser *my_file_chooser = GTK_FILE_CHOOSER(gtk_builder_get_object(builder, "file_load"));
  // Save Button
  GtkButton *save_file = GTK_BUTTON(gtk_builder_get_object(builder, "save_file"));
  // Choose midi device button
  GtkButton *choose_midi = GTK_BUTTON(gtk_builder_get_object(builder, "choose_midi"));
  // Notebook
  GtkNotebook *note_book = GTK_NOTEBOOK(gtk_builder_get_object(builder, "note_book"));

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
  GtkDrawingArea *da_adsr = GTK_DRAWING_AREA(gtk_builder_get_object(builder, "da1"));

  // ADSR Spin Buttons
  GtkSpinButton *attack_phase = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "attack_phase_control"));
  GtkSpinButton *decay_phase = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "decay_phase_control"));
  GtkSpinButton *release_phase = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "release_phase_control"));

  // ADSR Scales
  GtkScale *attack_bot = GTK_SCALE(gtk_builder_get_object(builder, "attack_bot"));
  GtkScale *attack_top = GTK_SCALE(gtk_builder_get_object(builder, "attack_top"));
  GtkScale *decay_bot = GTK_SCALE(gtk_builder_get_object(builder, "decay_bot"));
  GtkScale *sustain = GTK_SCALE(gtk_builder_get_object(builder, "sustain"));

  /*

  MIDI play

  */

  // GtkScrolledWindow *scrolled_window = GTK_SCROLLED_WINDOW(gtk_builder_get_object(builder, "scrolled_window"));
  GtkDrawingArea *da_midi_player = GTK_DRAWING_AREA(gtk_builder_get_object(builder, "da_midi_play"));
  GtkEventBox *midi_event = GTK_EVENT_BOX(gtk_builder_get_object(builder, "midi_event"));
  GtkButton *play_midi = GTK_BUTTON(gtk_builder_get_object(builder, "play_midi"));
  GtkButton *record_midi = GTK_BUTTON(gtk_builder_get_object(builder, "record_midi"));
  GtkButton *eraise_midi = GTK_BUTTON(gtk_builder_get_object(builder,"eraise_midi"));

  // Unreference the builder, since all the wanted object were built
  g_object_unref(builder);

  /*
    Piano Signals
  */
  g_signal_connect(G_OBJECT(da_piano), "draw", G_CALLBACK(on_draw_set_full_keyboard), vis_d);
  g_signal_connect(G_OBJECT(event_box), "event", G_CALLBACK(current_key_click), NULL);
  g_signal_connect(G_OBJECT(scale_piano), "value_changed", G_CALLBACK(on_scale_change_piano), NULL);

  /*
    Main Utility Signals
  */

  // Set the chosen file
  g_signal_connect(my_file_chooser, "selection-changed", G_CALLBACK(update_preview_cb), NULL);
  // Save a current configuration
  g_signal_connect(G_OBJECT(save_file), "clicked", G_CALLBACK(on_save_state), NULL);
  // Set the cureent chosen midi device
  g_signal_connect(G_OBJECT(choose_midi), "clicked", G_CALLBACK(midi_device_chooser_create), NULL);
  // Track the recording functionalities
  g_signal_connect(G_OBJECT(window), "key_release_event", G_CALLBACK(key_released), (gpointer)vis_d->data);
  // Track the current page for optimisation
  g_signal_connect(G_OBJECT(note_book), "notify", G_CALLBACK(on_switch_page), NULL);
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

  adsr_vs_and_param *attack_phase_param = malloc(sizeof(adsr_vs_and_param));
  attack_phase_param->param_index = 0;
  attack_phase_param->vs = vis_d;
  adsr_vs_and_param *decay_phase_param = malloc(sizeof(adsr_vs_and_param));
  decay_phase_param->param_index = 1;
  decay_phase_param->vs = vis_d;
  adsr_vs_and_param *release_phase_param = malloc(sizeof(adsr_vs_and_param));
  release_phase_param->param_index = 2;
  release_phase_param->vs = vis_d;
  adsr_vs_and_param *attack_amp = malloc(sizeof(adsr_vs_and_param));
  attack_amp->param_index = 3;
  attack_amp->vs = vis_d;
  adsr_vs_and_param *decay_amp = malloc(sizeof(adsr_vs_and_param));
  decay_amp->param_index = 4;
  decay_amp->vs = vis_d;
  adsr_vs_and_param *sustain_amp = malloc(sizeof(adsr_vs_and_param));
  sustain_amp->param_index = 5;
  sustain_amp->vs = vis_d;

  g_signal_connect(G_OBJECT(attack_phase), "value_changed", G_CALLBACK(on_adsr_change_param), attack_phase_param);
  g_signal_connect(G_OBJECT(decay_phase), "value_changed", G_CALLBACK(on_adsr_change_param), decay_phase_param);
  g_signal_connect(G_OBJECT(release_phase), "value_changed", G_CALLBACK(on_adsr_change_param), release_phase_param);

  g_signal_connect(G_OBJECT(attack_bot), "value_changed", G_CALLBACK(on_adsr_change_param), attack_amp);
  g_signal_connect(G_OBJECT(attack_top), "value_changed", G_CALLBACK(on_adsr_change_param), decay_amp);
  g_signal_connect(G_OBJECT(decay_bot), "value_changed", G_CALLBACK(on_adsr_change_param), sustain_amp);
  g_signal_connect(G_OBJECT(sustain), "value_changed", G_CALLBACK(on_adsr_change_param), sustain_amp);

  g_signal_connect(G_OBJECT(da_adsr), "draw", G_CALLBACK(on_adsr_draw), vis_d);

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

  /*

  MIDI Play

  */
  pth_and_ud *data_to_midi = malloc(sizeof(pth_and_ud));
  data_to_midi->data = vis_d->data;
  data_to_midi->thr = &thr;

  g_signal_connect(G_OBJECT(da_midi_player), "configure-event", G_CALLBACK(on_configure), &my_data);
  g_signal_connect(G_OBJECT(da_midi_player), "draw", G_CALLBACK(on_draw_midi), NULL);
  g_signal_connect(G_OBJECT(midi_event), "event", G_CALLBACK(ccurrent_key_click_midi), NULL);
  g_signal_connect(G_OBJECT(play_midi), "clicked", G_CALLBACK(construct_event_array), vis_d->data);
  g_signal_connect(G_OBJECT(record_midi), "clicked", G_CALLBACK(record_event_array), vis_d->data);
  g_signal_connect(G_OBJECT(eraise_midi), "clicked", G_CALLBACK(empty_event_array), vis_d->data);

  context = g_main_context_default();

  thread[0] = g_thread_new(NULL, thread_func, vis_d);
  // pthread_create(&thr, NULL, thread_caller, vis_d);

  gtk_widget_show_all(GTK_WIDGET(window));

  // Prompt The last session file
  on_start_app_last_session_prompt();

  gtk_main();

  g_thread_join(thread[0]);
  // pthread_join(thr, NULL);

  free(band_cut_data_low);
  free(band_cut_data_high);
  free(band_pass_data_low);
  free(band_pass_data_high);

  free(attack_amp);
  free(decay_amp);
  free(sustain_amp);
  free(attack_phase_param);
  free(decay_phase_param);
  free(release_phase_param);

  free(data_to_midi);
  return 0;
}
