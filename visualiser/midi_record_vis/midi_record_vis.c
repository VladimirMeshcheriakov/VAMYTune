#include "midi_record_vis.h"

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
void init_rect_from_key(int id_note, ud *data)
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
void release_rect_from_key(int id_note, ud *data)
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

void record_event_array(__attribute_maybe_unused__ GtkWidget *widget, gpointer user_data)
{
  ud *data = (ud *)user_data;
  start_midi_rec = data->time_management->actual_time;
}

void construct_event_array(GtkWidget *widget, gpointer user_data)
{
  ud *data = (ud *)user_data;
  double global_snap = data->time_management->actual_time + 10;
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

void thread_caller(void* data)
{
  construct_event_array(NULL,data);
}

void empty_event_array(GtkWidget *widget, gpointer user_data)
{
  for (size_t i = 0; i < 20*MINUTE*NUMBER_OF_KEYS; i++)
  {
    rect_node_delete_first_occurence(rect_list,deleted_rects,i);
  }
  
}