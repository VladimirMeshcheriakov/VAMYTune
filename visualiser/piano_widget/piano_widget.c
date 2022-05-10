#include "piano_widget.h"

// Current user cursor possition on the keyboard
//(x,y) cordinates of the users mause on the keyboard
int x = -1;
int y = -1;
//Number of octaves the keyboard shows
int octave_number = 10;

// returns 1 is the (currentx,current_y) is in the rectangle else 0
int is_in_rectangle(int current_x, int current_y, int rect_top_x, int rect_top_y, int rect_width, int rect_height)
{
  return (current_x < rect_top_x + rect_width && current_y < rect_top_y + rect_height && current_x > rect_top_x && current_y > rect_top_y) ? 1 : 0;
}

// Sets the new octave value
gboolean on_scale_change_piano(GtkWidget *a_scale, __attribute_maybe_unused__ gpointer user_data)
{
  int new_size = gtk_range_get_value(GTK_RANGE(a_scale));
  // g_print("id: %d\n", id);
  octave_number = new_size;
  return G_SOURCE_REMOVE;
}

// Gets the current event and sets the x,y possition
gboolean current_key_click(GtkWidget *event_box, __attribute_maybe_unused__ gpointer user_data)
{
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
  cairo_set_source_rgb(cr, 0.5, 0.5, 0.5);
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

int on_draw_black_key(cairo_t *cr, int drawing_area_width, int drawing_area_height, int j,  int on_manual)
{
  int present = 0;
  cairo_set_source_rgb(cr, 0, 0, 0);

  int top_left_x = drawing_area_width * j / (octave_number * 28);
  int top_right_x = drawing_area_width * (2 + j) / (octave_number * 28);
  int bot_right_y = drawing_area_height * 3 / 5;

  cairo_line_to(cr, top_left_x, 0);
  cairo_line_to(cr, top_right_x, 0);
  cairo_line_to(cr, top_right_x, bot_right_y);
  cairo_line_to(cr, top_left_x, bot_right_y);
  cairo_line_to(cr, top_left_x, 0);

  if (is_in_rectangle(x, y, top_left_x, 0, top_right_x - top_left_x, bot_right_y) || on_manual)
  {
    cairo_set_source_rgb(cr, 0.5, 0.5, 0.5);
    //printf("id: %d\n", id);
    present = 1;
  }
  cairo_fill(cr);
  return present;
}
// Draws one left type white key

int on_draw_left_type_white_key(cairo_t *cr, int drawing_area_width, int drawing_area_height, int j, int on_manual)
{
  int present = 0;
  // Default color if not pressed
  cairo_set_source_rgb(cr, 1, 1, 1);

  // The origin from which the tracing starts
  int origin = (drawing_area_width * j) / (octave_number * 7);

  // parameters of the top rectangle
  int top_rect_width = drawing_area_width * 3 / (octave_number * 28);
  int top_rect_height = drawing_area_height * 3 / 5;

  // parametes of the bottom rectangle
  int bot_rect_width = drawing_area_width / (octave_number * 7);
  int bot_rect_height = drawing_area_height * 2 / 5;

  // Draw the top part
  cairo_line_to(cr, origin, 0);
  cairo_line_to(cr, top_rect_width + origin, 0);
  cairo_line_to(cr, top_rect_width + origin, top_rect_height);

  // Check if the key is pressed on the top part
  if (is_in_rectangle(x, y, origin, 0, top_rect_width, top_rect_height) || on_manual)
  {
    cairo_set_source_rgb(cr, 0.5, 0.5, 0.5);
    present = 1;
    //printf("id: %d\n", id);
  }

  // Draw the bottom part
  cairo_line_to(cr, bot_rect_width + origin, top_rect_height);
  cairo_line_to(cr, bot_rect_width + origin, drawing_area_height);
  cairo_line_to(cr, origin, drawing_area_height);
  cairo_line_to(cr, origin, 0);

  // Check if the key is pressed on the bottom part
  if (is_in_rectangle(x, y, origin, top_rect_height, bot_rect_width, bot_rect_height) || on_manual)
  {
    cairo_set_source_rgb(cr, 0.5, 0.5, 0.5);
    present = 1;
    //printf("id: %d\n", id);
  }

  // Draw the rectangle
  cairo_fill(cr);
  return present;
}

// Draws one center type white key
int on_draw_center_type_white_key(cairo_t *cr, int drawing_area_width, int drawing_area_height, int j,  int on_manual)
{
  int present = 0;
  // Default color
  cairo_set_source_rgb(cr, 1, 1, 1);
  // The tracing origin
  int origin = (drawing_area_width * j) / (octave_number * 7) + drawing_area_width / (octave_number * 28);

  // Top rectangle parameters
  int top_rect_width = drawing_area_width / (octave_number * 14);
  int top_rect_height = drawing_area_height * 3 / 5;

  // parametes of the bottom rectangle
  int bot_rect_width = origin - drawing_area_width / (octave_number * 28);
  int bot_rect_height = drawing_area_height * 2 / 5;

  // Trace the top part
  cairo_line_to(cr, origin, 0);
  cairo_line_to(cr, origin + top_rect_width, 0);
  cairo_line_to(cr, origin + top_rect_width, top_rect_height);

  // Check if the key is pressed on the top part
  if (is_in_rectangle(x, y, origin, 0, top_rect_width, top_rect_height) || on_manual)
  {
    cairo_set_source_rgb(cr, 0.5, 0.5, 0.5);
    //printf("id: %d\n", id);
    present = 1;
  }

  // Trace the bottom part
  cairo_line_to(cr, drawing_area_width * 3 / (octave_number * 28) + origin, top_rect_height);
  cairo_line_to(cr, drawing_area_width * 3 / (octave_number * 28) + origin, drawing_area_height);
  cairo_line_to(cr, bot_rect_width, drawing_area_height);
  cairo_line_to(cr, bot_rect_width, top_rect_height);
  cairo_line_to(cr, origin, top_rect_height);
  cairo_line_to(cr, origin, 0);

  // Check if the key is pressed on the bottom part
  if (is_in_rectangle(x, y, bot_rect_width, top_rect_height, drawing_area_width / (octave_number * 7), bot_rect_height))
  {
    cairo_set_source_rgb(cr, 0.5, 0.5, 0.5);
    //printf("id: %d\n", id);
    present = 1;
  }

  cairo_fill(cr);
  return present;
}

// Draws one right type white key
int on_draw_right_type_white_key(cairo_t *cr, int drawing_area_width, int drawing_area_height, int j, int on_manual)
{
  int present = 0;
  // Default color white
  cairo_set_source_rgb(cr, 1, 1, 1);
  // The origin of tracing
  int origin = (drawing_area_width * j) / (octave_number * 7);

  // parameters of the top rectangle
  int top_rect_width = drawing_area_width * 3 / (octave_number * 28);
  int top_rect_height = drawing_area_height * 3 / 5;

  // parametes of the bottom rectangle
  int bot_rect_width = drawing_area_width / (octave_number * 7);
  int bot_rect_height = drawing_area_height * 2 / 5;

  cairo_line_to(cr, origin, 0);
  cairo_line_to(cr, origin, drawing_area_height);
  cairo_line_to(cr, origin - bot_rect_width, drawing_area_height);
  cairo_line_to(cr, origin - bot_rect_width, top_rect_height);
  cairo_line_to(cr, origin - top_rect_width, top_rect_height);
  cairo_line_to(cr, origin - top_rect_width, 0);
  cairo_line_to(cr, origin, 0);
  // Check if the key is pressed on the top part
  if (is_in_rectangle(x, y, origin - top_rect_width, 0, top_rect_width, top_rect_height) || on_manual)
  {
    cairo_set_source_rgb(cr, 0.5, 0.5, 0.5);
    //printf("id: %d\n", id);
    present = 1;
  }
  // Check if the key is pressed on the bottom part
  if (is_in_rectangle(x, y, origin - bot_rect_width, top_rect_height, bot_rect_width, bot_rect_height))
  {
    cairo_set_source_rgb(cr, 0.5, 0.5, 0.5);
    //printf("id: %d\n", id);
    present = 1;
  }
  cairo_fill(cr);
  return present;
}

//
void check_and_draw_Do(cairo_t *cr, int drawing_area_width, int drawing_area_height, Uint8 *keyboard, int o, ud* data)
{
  if (on_draw_left_type_white_key(cr, drawing_area_width, drawing_area_height, 0 + o * 7, 0))
  {
    keyboard[0 + 12 * o] = 2;
    //Key on from screen keyboard
    key_on(data,0 + 12 * o);
  }
  else
  {
    if (keyboard[0 + 12 * o] == 1)
    {
      on_draw_left_type_white_key(cr, drawing_area_width, drawing_area_height, 0 + o * 7,  1);
      //Key on from midi keyboard
    }
    if (keyboard[0 + 12 * o] == 2)
    {
      //The user no longer presses the key on the screen
      keyboard[0 + 12 * o] = 0;
      key_off(data,0 + 12 * o);
    }
  } // Do
}

void check_and_draw_Do_sharp(cairo_t *cr, int drawing_area_width, int drawing_area_height, Uint8 *keyboard, int o, ud* data)
{
  if (on_draw_black_key(cr, drawing_area_width, drawing_area_height, 3 + o * 28,  0))
  {
    keyboard[1 + 12 * o] = 2;
    key_on(data,1 + 12 * o);
  }
  else
  {
    if (keyboard[1 + 12 * o] == 1)
    {
      on_draw_black_key(cr, drawing_area_width, drawing_area_height, 3 + o * 28,  1);
    }
    if (keyboard[1 + 12 * o] == 2)
    {
      keyboard[1 + 12 * o] = 0;
      key_off(data,1 + 12 * o);
    }
  } // Do#
}

void check_and_draw_Re(cairo_t *cr, int drawing_area_width, int drawing_area_height, Uint8 *keyboard, int o, ud* data)
{
  if (on_draw_center_type_white_key(cr, drawing_area_width, drawing_area_height, 1 + o * 7, 0))
  {
    keyboard[2 + 12 * o] = 2;
    key_on(data,2 + 12 * o);
  }
  else
  {
    if (keyboard[2 + 12 * o] == 1)
    {
      on_draw_center_type_white_key(cr, drawing_area_width, drawing_area_height, 1 + o * 7, 1);
    }
    if (keyboard[2 + 12 * o] == 2)
    {
      keyboard[2 + 12 * o] = 0;
      key_off(data,2 + 12 * o);
    }
  } // Re
}

void check_and_draw_Re_sharp(cairo_t *cr, int drawing_area_width, int drawing_area_height, Uint8 *keyboard, int o, ud* data)
{
  if (on_draw_black_key(cr, drawing_area_width, drawing_area_height, 7 + o * 28,  0))
  {
    keyboard[3 + 12 * o] = 2;
    key_on(data,3 + 12 * o);
  }
  else
  {
    if (keyboard[3 + 12 * o] == 1)
    {
      on_draw_black_key(cr, drawing_area_width, drawing_area_height, 7 + o * 28, 1);
    }
    if (keyboard[3 + 12 * o] == 2)
    {
      keyboard[3 + 12 * o] = 0;
      key_off(data,3 + 12 * o);
    }
  } // Re#
}

void check_and_draw_Mi(cairo_t *cr, int drawing_area_width, int drawing_area_height, Uint8 *keyboard, int o, ud* data)
{
  if (on_draw_right_type_white_key(cr, drawing_area_width, drawing_area_height, 3 + o * 7, 0))
  {
    keyboard[4 + 12 * o] = 2;
    key_on(data,4 + 12 * o);
  }
  else
  {
    if (keyboard[4 + 12 * o] == 1)
    {
      on_draw_right_type_white_key(cr, drawing_area_width, drawing_area_height, 3 + o * 7,  1);
    }
    if (keyboard[4 + 12 * o] == 2)
    {
      keyboard[4 + 12 * o] = 0;
      key_off(data,4 + 12 * o);
    }
  } // Mi
}

void check_and_draw_Fa(cairo_t *cr, int drawing_area_width, int drawing_area_height, Uint8 *keyboard, int o, ud* data)
{
  if (on_draw_left_type_white_key(cr, drawing_area_width, drawing_area_height, 3 + o * 7,  0))
  {
    keyboard[5 + 12 * o] = 2;
    key_on(data,5 + 12 * o);
  }
  else
  {
    if (keyboard[5 + 12 * o] == 1)
    {
      on_draw_left_type_white_key(cr, drawing_area_width, drawing_area_height, 3 + o * 7,  1);
    }
    if (keyboard[5 + 12 * o] == 2)
    {
      keyboard[5 + 12 * o] = 0;
      key_off(data,5 + 12 * o);
    }
  } // Fa
}

void check_and_draw_Fa_sharp(cairo_t *cr, int drawing_area_width, int drawing_area_height, Uint8 *keyboard, int o, ud* data)
{
  if (on_draw_black_key(cr, drawing_area_width, drawing_area_height, 15 + o * 28, 0))
  {
    keyboard[6 + 12 * o] = 2;
    key_on(data,6 + 12 * o);
  }
  else
  {
    if (keyboard[6 + 12 * o] == 1)
    {
      on_draw_black_key(cr, drawing_area_width, drawing_area_height, 15 + o * 28,  1);
    }
    if (keyboard[6 + 12 * o] == 2)
    {
      keyboard[6 + 12 * o] = 0;
      key_off(data,6 + 12 * o);
    }
  } // Fa#
}

void check_and_draw_Sol(cairo_t *cr, int drawing_area_width, int drawing_area_height, Uint8 *keyboard, int o, ud* data)
{
  if (on_draw_center_type_white_key(cr, drawing_area_width, drawing_area_height, 4 + o * 7,  0))
  {
    keyboard[7 + 12 * o] = 2;
    key_on(data,7 + 12 * o);
  }
  else
  {
    if (keyboard[7 + 12 * o] == 1)
    {
      on_draw_center_type_white_key(cr, drawing_area_width, drawing_area_height, 4 + o * 7,  1);
    }
    if (keyboard[7 + 12 * o] == 2)
    {
      keyboard[7 + 12 * o] = 0;
      key_off(data,7 + 12 * o);
    }
  } // Sol
}

void check_and_draw_Sol_sharp(cairo_t *cr, int drawing_area_width, int drawing_area_height, Uint8 *keyboard, int o, ud* data)
{

  if (on_draw_black_key(cr, drawing_area_width, drawing_area_height, 19 + o * 28,  0))
  {
    keyboard[8 + 12 * o] = 2;
    key_on(data,8 + 12 * o);
  }
  else
  {
    if (keyboard[8 + 12 * o] == 1)
    {
      on_draw_black_key(cr, drawing_area_width, drawing_area_height, 19 + o * 28,  1);
    }
    if (keyboard[8 + 12 * o] == 2)
    {
      keyboard[8 + 12 * o] = 0;
      key_off(data,8 + 12 * o);
    }
  } // Sol#
}

void check_and_draw_La(cairo_t *cr, int drawing_area_width, int drawing_area_height, Uint8 *keyboard, int o, ud* data)
{

  if (on_draw_center_type_white_key(cr, drawing_area_width, drawing_area_height, 5 + o * 7, 0))
  {
    keyboard[9 + 12 * o] = 2;
    key_on(data,9 + 12 * o);
  }
  else
  {
    if (keyboard[9 + 12 * o] == 1)
    {
      on_draw_center_type_white_key(cr, drawing_area_width, drawing_area_height, 5 + o * 7,  1);
    }
    if (keyboard[9 + 12 * o] == 2)
    {
      keyboard[9 + 12 * o] = 0;
      key_off(data,9 + 12 * o);
    }
  } // La
}

void check_and_draw_La_sharp(cairo_t *cr, int drawing_area_width, int drawing_area_height, Uint8 *keyboard, int o, ud* data)
{

  if (on_draw_black_key(cr, drawing_area_width, drawing_area_height, 23 + o * 28,  0))
  {
    keyboard[10 + 12 * o] = 2;
    key_on(data,10 + 12 * o);
  }
  else
  {
    if (keyboard[10 + 12 * o] == 1)
    {
      on_draw_black_key(cr, drawing_area_width, drawing_area_height, 23 + o * 28,  1);
    }
    if (keyboard[10 + 12 * o] == 2)
    {
      keyboard[10 + 12 * o] = 0;
      key_off(data,10 + 12 * o);
    }
  } // La#
}

void check_and_draw_Si(cairo_t *cr, int drawing_area_width, int drawing_area_height, Uint8 *keyboard, int o, ud* data)
{

  if (on_draw_right_type_white_key(cr, drawing_area_width, drawing_area_height, 7 + o * 7,  0))
  {
    keyboard[11 + 12 * o] = 2;
    key_on(data,11 + 12 * o);
  }
  else
  {
    if (keyboard[11 + 12 * o] == 1)
    {
      on_draw_right_type_white_key(cr, drawing_area_width, drawing_area_height, 7 + o * 7,  1);
    }
    if (keyboard[11 + 12 * o] == 2)
    {
      keyboard[11 + 12 * o] = 0;
      key_off(data,11 + 12 * o);
    }
  } // Si
}

// Draws the full keyboard
gboolean on_draw_full_keyboard(cairo_t *cr, int drawing_area_width, int drawing_area_height, Uint8 *keyboard, ud* data)
{
  for (int o = 0; o < octave_number; o++)
  {
    check_and_draw_Do(cr, drawing_area_width, drawing_area_height, keyboard, o, data);
    check_and_draw_Do_sharp(cr, drawing_area_width, drawing_area_height, keyboard, o,data);
    check_and_draw_Re(cr, drawing_area_width, drawing_area_height, keyboard, o,data);
    check_and_draw_Re_sharp(cr, drawing_area_width, drawing_area_height, keyboard, o,data);
    check_and_draw_Mi(cr, drawing_area_width, drawing_area_height, keyboard, o,data);
    check_and_draw_Fa(cr, drawing_area_width, drawing_area_height, keyboard, o,data);
    check_and_draw_Fa_sharp(cr, drawing_area_width, drawing_area_height, keyboard, o,data);
    check_and_draw_Sol(cr, drawing_area_width, drawing_area_height, keyboard, o,data);
    check_and_draw_Sol_sharp(cr, drawing_area_width, drawing_area_height, keyboard, o,data);
    check_and_draw_La(cr, drawing_area_width, drawing_area_height, keyboard, o,data);
    check_and_draw_La_sharp(cr, drawing_area_width, drawing_area_height, keyboard, o,data);
    check_and_draw_Si(cr, drawing_area_width, drawing_area_height, keyboard, o,data);
  }
  on_draw_key_lines(cr, drawing_area_width, drawing_area_height, octave_number);
  return G_SOURCE_REMOVE;
}

// Dynamically draws the signal
gboolean on_draw_set_full_keyboard(GtkWidget *widget, cairo_t *cr, __attribute_maybe_unused__ gpointer user_data)
{
  vis_data *vs_d = (vis_data *)user_data;
  Uint8 * keyboard = vs_d->state;
  ud* data = vs_d->data;
  GdkRectangle da;            /* GtkDrawingArea size */
  gdouble dx = 2.0, dy = 2.0; /* Pixels between each point */
  gdouble clip_x1 = 0.0, clip_y1 = 0.0, clip_x2 = 0.0, clip_y2 = 0.0;
  GdkWindow *window = gtk_widget_get_window(widget);

  set_up_axes_for_piano(window, &da, cr, &clip_x1, &clip_x2, &clip_y1, &clip_y2, &dx, &dy);

  on_draw_full_keyboard(cr, da.width, da.height, keyboard,data);
  gtk_widget_queue_draw_area(widget, 0, 0, da.width, da.height);
  return G_SOURCE_REMOVE;
}