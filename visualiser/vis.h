#include <gtk/gtk.h>
#include <math.h>
#include <cairo.h>
#include "../userdata/userdata.h"
#include "../midi_interface_management/midi_management.h"
#include "../signals/signals.h"
#include <SDL2/SDL.h>
#include "../FFT/dft.h"

#define N_THREADS 1

#ifndef VIS_H
#define VIS_H

int gtk_run_zbi(ud * data, Uint8 * state,int argc, char **argv );

#endif 