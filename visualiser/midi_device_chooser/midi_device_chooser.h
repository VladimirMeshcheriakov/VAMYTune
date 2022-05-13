#include <gtk/gtk.h>
#include "../../midi_interface_management/midi_management.h"
#ifndef MIDI_DEVICE_CHOOSER_H
#define MIDI_DEVICE_CHOOSER_H

extern snd_seq_t *midi_seq;

typedef struct midi_device_info
{
  int id;
  const char *name;
} midi_device_info;

void midi_device_chooser_create(__attribute_maybe_unused__ GtkWidget *button, gpointer userdata);

#endif 