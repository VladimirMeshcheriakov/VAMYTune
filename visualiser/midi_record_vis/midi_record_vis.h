
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>
#include <gtk/gtk.h>
#include <pthread.h>
#include <cairo.h>
#include "../../note_state_utils/note_state_utils.h"

#ifndef MIDI_RECORD_VIS_H
#define MIDI_RECORD_VIS_H

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

extern int x_midi;
extern int y_midi;
extern int press_set;
extern int grabbed;
extern int wait;

extern rect_node *rect_list;
extern rect_node *deleted_rects;

// If 1 theres a rect, 0 if not
extern char *global_rect_table;

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

void rect_node_print(rect_node *head);
rect_node *rect_node_build_sentinel();

void empty_event_array(GtkWidget *widget, gpointer user_data);
void thread_caller(void* data);
void construct_event_array(GtkWidget *widget, gpointer user_data);
void record_event_array(__attribute_maybe_unused__ GtkWidget *widget, gpointer user_data);
void release_rect_from_key(int id_note, ud *data);
void init_rect_from_key(int id_note, ud *data);

gboolean on_configure(GtkWidget *widget, __attribute_maybe_unused__ GdkEventConfigure *event_p, gpointer user_data);
gboolean on_draw_midi(GtkWidget *widget, cairo_t *cr, __attribute_maybe_unused__ gpointer user_data);
void on_midi_quit(__attribute_maybe_unused__ GtkWidget *widget, gpointer user_data);
gboolean ccurrent_key_click_midi(GtkWidget *da, GdkEvent *event, __attribute_maybe_unused__ gpointer user_data);


#endif 