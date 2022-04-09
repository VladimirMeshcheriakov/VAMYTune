#include "vis.h"

GMainContext *context;

typedef struct
{
    GtkScale *lead;
    vis_data *data;

} GtkMultipleScales;

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

//Key_change
static gboolean key_released(GtkWidget *widget, GdkEventKey *event, gpointer user_data)
{
    ud *data= (ud *)user_data;
    if (event->keyval == GDK_KEY_r)
    {
        //Record
        data->wav_manager->record = 1;
    }
    else if (event->keyval == GDK_KEY_s)
    {
        //Stop record
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
    //g_print("%f\n",new_time);
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

   

    //printf("exec x1 %f , x2 %f, dx %f\n", clip_x1, clip_x2, (clip_x2/(double)1024));
    // printf("exec y1 %f , y2 %f, dy %f\n", clip_y1, clip_y2, dy);
    /* Link each data point */
    int cpt = 0;
    for (i = clip_x1; i < clip_x2; i += (clip_x2/(double)1024))
    {
        if (cpt < 1024)
        {
            float he = us[cpt];
            // printf("double %f\n",i);
            cairo_line_to(cr, i, he* clip_y2);
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
    for (i = clip_x1; i < clip_x2; i += (clip_x2/(double)512) )
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
    for (i = clip_x1; i < clip_x2; i += (clip_x2/(double)512))
    {
        if (cpt < 1024)
        {
            float he = resp[cpt];
            // printf("double %f\n",i);
            cairo_line_to(cr, i, he);
           
        }
         cpt += 1;
    }
    cairo_set_source_rgba(cr, 0.2, 0.6, 0.6, 0.7);
    cairo_stroke(cr);
    

    

    gtk_widget_queue_draw_area(widget, 0, 0, drawing_area_width, drawing_area_height);

    return G_SOURCE_REMOVE;
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
        int p = poll(pfds, npfds, 20);
        if (p == 0)
        {
            update_effects(my_data);
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
                update_effects(my_data);
                note_state(state, data);
                init_piano_keys(state, data);
            }
        }
        apply_filter_to_sample(my_data,1024);
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

    float cord_table[] = {vs->attack_amp,vs->decay_amp,vs->sustain_amp,vs->sustain_amp,0.0};
    

    float sust_phase = (vs->attack_phase+vs->decay_phase+vs->release_phase)/3.0;

    float sig_sum = vs->attack_phase+vs->decay_phase+vs->release_phase + sust_phase;

    float factor = clip_x2/sig_sum;

    int cpt = 0;

    float y_tab[] = {0.0,vs->attack_phase,vs->decay_phase+vs->attack_phase,sust_phase+ vs->decay_phase+vs->attack_phase,vs->release_phase + sust_phase+ vs->decay_phase+vs->attack_phase};

    i = clip_x1;


    while(cpt<5)
    {
        cairo_line_to(cr,y_tab[cpt] * factor,clip_y2 * cord_table[cpt]);
        cpt+=1;
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

    gtk_init(&argc, &argv);

    GtkBuilder *builder = gtk_builder_new();
    GError *error = NULL;
    if (gtk_builder_add_from_file(builder, "visualiser/plain.glade", &error) == 0)
    {
        g_printerr("Error loading file: %s\n", error->message);
        g_clear_error(&error);
        return 1;
    }

    //Drawing Area
    // Window
    GtkWindow *window = GTK_WINDOW(gtk_builder_get_object(builder, "org.gtk.duel"));

    // Title of the window
    gtk_window_set_title(GTK_WINDOW(window), "filter app");

    // Drawing areas
    GtkDrawingArea *da_signal = GTK_DRAWING_AREA(gtk_builder_get_object(builder, "area"));
    GtkDrawingArea *da_harmonics = GTK_DRAWING_AREA(gtk_builder_get_object(builder, "harmonics_da"));
    //ADSR da
    GtkDrawingArea *da = GTK_DRAWING_AREA(gtk_builder_get_object(builder, "da"));

    // SpinButtons
    GtkSpinButton *spx = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "X_ZOOM"));
    GtkSpinButton *spy = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "Y_ZOOM"));
    //ADSR Spin Buttons
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

    //ADSR Scales
    GtkScale *attack_bot = GTK_SCALE(gtk_builder_get_object(builder,"attack_bot"));
    GtkScale *attack_top = GTK_SCALE(gtk_builder_get_object(builder,"attack_top"));
    GtkScale *decay_bot = GTK_SCALE(gtk_builder_get_object(builder,"decay_bot"));
    GtkScale *sustain = GTK_SCALE(gtk_builder_get_object(builder,"sustain"));

    // Check Buttons
    GtkCheckButton *low_pass_activate = GTK_CHECK_BUTTON(gtk_builder_get_object(builder, "low_pass_activate"));
    GtkCheckButton *high_pass_activate = GTK_CHECK_BUTTON(gtk_builder_get_object(builder, "high_pass_activate"));
    GtkCheckButton *band_pass_activate = GTK_CHECK_BUTTON(gtk_builder_get_object(builder, "band_pass_activate"));
    GtkCheckButton *band_cut_activate = GTK_CHECK_BUTTON(gtk_builder_get_object(builder, "band_cut_activate"));

    //Submit button
    GtkButton *submit_adsr = GTK_BUTTON(gtk_builder_get_object(builder, "submit_adsr"));

    // Unreference the objects
    g_object_unref(builder);

    // Signals

    g_signal_connect(G_OBJECT(window), "key_release_event", G_CALLBACK(key_released), (gpointer) vis_d->data);

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

    //ADSR signals

    g_signal_connect(G_OBJECT(attack_phase),"value_changed",G_CALLBACK(on_spinner_change),&vis_d->attack_phase);
    g_signal_connect(G_OBJECT(decay_phase),"value_changed",G_CALLBACK(on_spinner_change),&vis_d->decay_phase);
    g_signal_connect(G_OBJECT(release_phase),"value_changed",G_CALLBACK(on_spinner_change),&vis_d->release_phase);

    g_signal_connect(G_OBJECT(attack_bot),"value_changed", G_CALLBACK(on_scale_change),&vis_d->attack_amp);
    g_signal_connect(G_OBJECT(attack_top),"value_changed", G_CALLBACK(on_scale_change),&vis_d->decay_amp);
    g_signal_connect(G_OBJECT(decay_bot),"value_changed", G_CALLBACK(on_scale_change),&vis_d->sustain_amp);
    g_signal_connect(G_OBJECT(sustain),"value_changed", G_CALLBACK(on_scale_change),&vis_d->sustain_amp);

    g_signal_connect(G_OBJECT(da), "draw", G_CALLBACK(on_adsr_draw), vis_d);




    GtkMultipleScales *band_cut_data_low = malloc(sizeof(GtkMultipleScales));
    band_cut_data_low->data = vis_d;
    band_cut_data_low->lead = band_cut_high;

    GtkMultipleScales *band_cut_data_high = malloc(sizeof(GtkMultipleScales));
    band_cut_data_high->data = vis_d;
    band_cut_data_high->lead = band_cut_low;

    g_signal_connect(G_OBJECT(band_cut_low), "value_changed", G_CALLBACK(on_scale_band_cut_change_low), (gpointer) band_cut_data_low);
    g_signal_connect(G_OBJECT(band_cut_high), "value_changed", G_CALLBACK(on_scale_band_cut_change_high), (gpointer) band_cut_data_high);

    GtkMultipleScales *band_pass_data_low = malloc(sizeof(GtkMultipleScales));
    band_pass_data_low->data = vis_d;
    band_pass_data_low->lead = band_pass_high;

    GtkMultipleScales *band_pass_data_high = malloc(sizeof(GtkMultipleScales));
    band_pass_data_high->data = vis_d;
    band_pass_data_high->lead = band_pass_low;

    g_signal_connect(G_OBJECT(band_pass_low), "value_changed", G_CALLBACK(on_scale_band_change_low), (gpointer) band_pass_data_low);
    g_signal_connect(G_OBJECT(band_pass_high), "value_changed", G_CALLBACK(on_scale_band_change_high), (gpointer) band_pass_data_high);

    g_signal_connect(G_OBJECT(low_pass_cutoff),"value_changed",G_CALLBACK(on_scale_change), &vis_d->low_pass_cut);
    g_signal_connect(G_OBJECT(high_pass_cutoff),"value_changed",G_CALLBACK(on_scale_change),&vis_d->high_pass_cut);

    // Signal on the drawing area of the signal
    g_signal_connect(G_OBJECT(da_signal), "draw", G_CALLBACK(on_draw_signal), vis_d);
    // Signal on the drawing area of the harmonic
    g_signal_connect(G_OBJECT(da_harmonics), "draw", G_CALLBACK(on_draw_harmonics), vis_d);

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


