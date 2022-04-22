#include <gtk/gtk.h>
#include <math.h>
#include <cairo.h>
#include "../../FFT/dft.h"

#ifndef FILTER_VIS_H
#define FILTER_VIS_H

// Struct to manage follow up lifting of parameters
typedef struct
{
  GtkScale *lead;
  vis_data *data;
} GtkMultipleScales;

gboolean on_x(GtkWidget *a_spinner, gpointer user_data);
gboolean on_y(GtkWidget *a_spinner, gpointer user_data);
gboolean on_activate(__attribute_maybe_unused__ GtkWidget *a_check, gpointer user_data);
gboolean on_spinner_change(GtkWidget *a_spinner, gpointer user_data);
gboolean on_scale_change(GtkWidget *a_scale, gpointer user_data);
gboolean on_scale_band_cut_change_low(GtkWidget *low_scale, gpointer user_data);
gboolean on_scale_band_cut_change_high(GtkWidget *high_scale, gpointer user_data);
gboolean on_scale_band_change_low(GtkWidget *low_scale, gpointer user_data);
gboolean on_scale_band_change_high(GtkWidget *high_scale, gpointer user_data);
gboolean on_draw_harmonics(GtkWidget *widget, cairo_t *cr, gpointer user_data);
gboolean on_draw_signal(GtkWidget *widget, cairo_t *cr, gpointer user_data);

#endif