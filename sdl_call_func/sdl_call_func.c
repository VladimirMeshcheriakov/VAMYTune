#include "sdl_call_func.h"
#include <gtk/gtk.h>
#include <math.h>
#include <cairo.h>

#define N_THREADS 1
GMainContext *context;

// Inits the sdl audio
int init_sdl_audio()
{
    if (SDL_Init(SDL_INIT_AUDIO) < 0)
    {
        fprintf(stderr, "Error initializing SDL. SDL_Error: %s\n",
                SDL_GetError());
        return -1;
    }
    return 0;
}

// Sets and an audio device, or sends  -1 if error
SDL_AudioDeviceID audio_spec_set_data(ud *data, void *audio_callback)
{
    SDL_AudioSpec audio_spec_want, audio_spec;

    SDL_memset(&audio_spec_want, 0, sizeof(audio_spec_want));

    audio_spec_want.freq = 44100;
    audio_spec_want.format = AUDIO_F32;
    audio_spec_want.channels = 2;
    audio_spec_want.samples = 1024;
    audio_spec_want.callback = audio_callback;
    audio_spec_want.userdata = data;

    SDL_AudioDeviceID audio_device_id = SDL_OpenAudioDevice(NULL, 0, &audio_spec_want, &audio_spec, SDL_AUDIO_ALLOW_FORMAT_CHANGE);
    if (!audio_device_id)
    {
        fprintf(stderr, "Error creating SDL audio device. SDL_Error: %s\n",
                SDL_GetError());
        SDL_Quit();
        return -1;
    }
    else
    {
        return audio_device_id;
    }
}


// Updates the effect table
void update_effects(ud *data)
{
    // printf_time(data->time_management->time_table,1);
    for (size_t i = 0; i < 127; i++)
    {
        if (data->all_keys->keys[i] || data->time_management->time_table[i]->release_stage)
        {
            data->all_keys->effects[i] = adsr_get_amplitude(data->time_management->actual_time, data->adsr, data->time_management->time_table[i]);
            // printf("%f\n", data->all_keys->effects[i]);
        }
        else
        {
            data->all_keys->effects[i] = 0;
        }
    }
}


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

// Main loop of the app
void run_app(vis_struct * my_data)
{
    int argc = my_data->argc;
    char **argv = my_data->argv;
    ud* data = my_data->data;
    Uint8 * state = my_data->state;
    int running = my_data->stop_thread;
    data->fout = open_WAV("Bach.wav");
    data->fout_size = findSize("Bach.wav");
    struct pollfd *pfds;
    int npfds;

    init_seq();

    if (parse_input(argc, argv) != -1)
    {
        printf("Parsing error\n");
    }

    snd_seq_t * seq = create_port();
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

    while (running)
    {
        snd_seq_poll_descriptors(seq, pfds, npfds, POLLIN);
        int p = poll(pfds, npfds, 20);
        if (p == 0)
        {
            update_effects(data);
            note_state(state, data);
            init_piano_keys(state, data);
        }
        else
        {
            if (p < 0)
            {
                break;
            }
            snd_seq_event_t *event_;
            snd_seq_event_input(seq, &event_);
            if (event_)
            {
                dump_event(event_, state);
                update_effects(data);
                note_state(state, data);
                init_piano_keys(state, data);
            }
        }
    }
    printf("The while is ended");
}



/*
static gboolean on_quit(gpointer user_data)
{
    vis_struct* vis = (vis_struct *)user_data;
    vis->stop_thread = 0;
    printf("The stop is triggered\n");
    return G_SOURCE_REMOVE;
}
*/
static gboolean
on_x(GtkWidget *a_spinner,gpointer user_data)
{
    int * x_zoom = (int *) user_data;
    int x = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON(a_spinner));
    *x_zoom = x;
    return G_SOURCE_REMOVE;
}

static gboolean
on_y(GtkWidget *a_spinner,gpointer user_data)
{
    int * y_zoom = (int *) user_data;
    int y = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON(a_spinner));
    *y_zoom = y;
    return G_SOURCE_REMOVE;
}

static gboolean
on_draw(GtkWidget *widget, cairo_t *cr, gpointer user_data)
{

    vis_struct *vs = (vis_struct *)user_data;
    float * us = vs->sig;
    int zoom_x = vs->x_zoom;
    int zoom_y = vs->y_zoom;


    GdkRectangle da;                /* GtkDrawingArea size */
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
    //Put the origin of the graph into the center of the image
    cairo_translate(cr, da.width/2 , da.height/2 );
    cairo_scale(cr, zoom_x,-zoom_y);

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

    
    dx = (((double)drawing_area_width/200)/5.12) * 0.01;

    //printf("exec x1 %f , x2 %f, dx %f\n", clip_x1, clip_x2, dx);
    //printf("exec y1 %f , y2 %f, dy %f\n", clip_y1, clip_y2, dy);
     /* Link each data point */
    int cpt = 0;
    for (i = clip_x1; i < clip_x2; i += dx)
    {
        if(cpt<1024)
        {
            float he = us[cpt];
            //printf("double %f\n",i);
            cairo_line_to(cr, i, he);
            cpt += 1;
        }
    }
    //printf("cpt %d\n", cpt);

    /* Draw the curve */
    cairo_set_source_rgba(cr, 1, 0.2, 0.2, 0.6);
    cairo_stroke(cr);

    //printf("da_h %d , da_w %d\n",drawing_area_height,drawing_area_width);

    gtk_widget_queue_draw_area(widget, 0, 0, drawing_area_width, drawing_area_height);

    return G_SOURCE_REMOVE;
}

static gpointer
thread_func(gpointer user_data)
{
    vis_struct *vis_data = (vis_struct *)user_data;

    g_print("Starting thread %d\n", 1);
    run_app(vis_data);
    g_print("Ending thread %d\n", 1);
    return NULL;
}

int gtk_run_zbi(float * sig, ud * data, Uint8 * state,int argc, char **argv )
{
    GThread *thread[N_THREADS];

    vis_struct vis_data;

    vis_data.data = data;
    vis_data.sig = sig;
    vis_data.state = state;
    vis_data.stop_thread = 1;
    vis_data.argc = argc;
    vis_data.argv = argv;
    vis_data.x_zoom = 300;
    vis_data.y_zoom = 300;

    gtk_init(NULL,NULL);

    GtkBuilder* builder = gtk_builder_new();
    GError* error = NULL;
    if (gtk_builder_add_from_file(builder, "sdl_call_func/plain.glade", &error) == 0)
    {
        g_printerr("Error loading file: %s\n", error->message);
        g_clear_error(&error);
        return 1;
    }

    GtkWindow* window = GTK_WINDOW(gtk_builder_get_object(builder, "org.gtk.duel"));
    //gtk_window_set_default_size(GTK_WINDOW(window), WIDTH, HEIGHT);
    gtk_window_set_title(GTK_WINDOW(window), "Graph drawing");


    GtkDrawingArea* da = GTK_DRAWING_AREA(gtk_builder_get_object(builder, "area"));
    GtkSpinButton* spx = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "X_ZOOM"));
    GtkSpinButton* spy = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "Y_ZOOM"));
    
    g_object_unref(builder);

    g_signal_connect(G_OBJECT(window), "destroy", gtk_main_quit, NULL);
    g_signal_connect(G_OBJECT(spx), "value_changed", G_CALLBACK(on_x), &vis_data.x_zoom);
    g_signal_connect(G_OBJECT(spy), "value_changed", G_CALLBACK(on_y), &vis_data.y_zoom);
    g_signal_connect(G_OBJECT(da), "draw", G_CALLBACK(on_draw), (gpointer)&vis_data);

    context = g_main_context_default();

    for (int n = 0; n < N_THREADS; ++n)
        thread[n] = g_thread_new(NULL, thread_func, (gpointer)&vis_data);

    gtk_widget_show_all(GTK_WIDGET(window));
    gtk_main();

    for (int n = 0; n < N_THREADS; ++n)
        g_thread_join(thread[n]);

    
    return 0;
}


// Frees all the memory after the run_app
void stop_app(SDL_AudioDeviceID audio_device_id, ud *data)
{
    SDL_CloseAudioDevice(audio_device_id);
    free_user_data(data);
    SDL_Quit();
}

Uint8 *init_state_midi_keyboard(size_t size)
{
    Uint8 *state = calloc(size, sizeof(Uint8));
    return state;
}

// Main function that is called by the main
void init_run_app(ud *data, void *audio_callback, int argc, char *argv[])
{
    init_sdl_audio();
    SDL_AudioDeviceID audio_device_id = audio_spec_set_data(data, audio_callback);

    SDL_PauseAudioDevice(audio_device_id, 0);
    Uint8 *state = init_state_midi_keyboard(127);

    init_piano_keys(state, data);

    gtk_run_zbi(data->sig,data,state, argc, argv);

    fclose(data->fout);
    if (data->wav_manager->recorded_samples)
    {
        record(data->wav_manager->recorded_samples, data->fstream->array, "Bach.wav", "wb");
    }
    free(state);
    stop_app(audio_device_id, data);
}