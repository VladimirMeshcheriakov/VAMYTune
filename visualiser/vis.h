#include <gtk/gtk.h>
#include <math.h>
#include <cairo.h>
#include "../userdata/userdata.h"
#include "../midi_interface_management/midi_management.h"
#include "../signals/signals.h"
#include "../wav_interface/wav_interface.h"
#include <SDL2/SDL.h>

#define N_THREADS 1

#ifndef VIS_H
#define VIS_H




typedef struct
{
    ud* data;
    Uint8 *state;
    float * sig;
    int stop_thread;
    int argc;
    char **argv;
    int x_zoom;
    int y_zoom;
}vis_struct;

int gtk_run_zbi(ud * data, Uint8 * state,int argc, char **argv );

#endif 