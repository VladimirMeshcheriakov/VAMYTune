#include <gtk/gtk.h>
#include <math.h>
#include <cairo.h>
#define WIDTH 1080
#define HEIGHT 1080
#define N_THREADS 1
#define N_ITERATIONS 6500

#define ZOOM_X 100.0
#define ZOOM_Y 100.0

GMainContext *context;

static gboolean
on_draw(GtkWidget *widget, cairo_t *cr, gpointer user_data)
{
    GdkRectangle da;                /* GtkDrawingArea size */
    gdouble dx = 1.055, dy = 1.055; /* Pixels between each point */
    gdouble i, clip_x1 = 0.0, clip_y1 = 0.0, clip_x2 = 0.0, clip_y2 = 0.0;

    GdkWindow *window = gtk_widget_get_window(widget);

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
    cairo_translate(cr, da.width / 2, da.height / 2);
    cairo_scale(cr, ZOOM_X, -ZOOM_Y);

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

    float *us = (float *)user_data;
    // printf("exec x1 %f , x2 %f, dx %f\n", clip_x1, clip_x2, dx);

    /* Link each data point */
    int cpt = 0;
    for (i = clip_x1; i < clip_x2; i += dx)
    {

        float he = us[cpt];
        // printf("double %f\n",i);
        cairo_line_to(cr, i, he);
        cpt += 1;
    }
    // printf("cpt %d\n", cpt);

    /* Draw the curve */
    cairo_set_source_rgba(cr, 1, 0.2, 0.2, 0.6);
    cairo_stroke(cr);

    int drawing_area_width = gtk_widget_get_allocated_width(widget);
    int drawing_area_height = gtk_widget_get_allocated_height(widget);

    gtk_widget_queue_draw_area(widget, 0, 0, drawing_area_width, drawing_area_height);

    return G_SOURCE_REMOVE;
}

static gpointer
thread_func(gpointer user_data)
{
    GSource *source;
    float *table = (float *)user_data;

    g_print("Starting thread %d\n", 1);
    /* If you want to see anything you should add a delay
     * to let the main loop update the UI, e.g.:
     * g_usleep(g_random_int_range(1234, 567890));
     */
    
    g_print("Ending thread %d\n", 1);
    return NULL;
}

int gtk_run_zbi()
{

    GtkWidget *window;
    GThread *thread[N_THREADS];
    GtkWidget *da;

    float *sig = malloc(sizeof(float) * 1024);

    gtk_init(NULL,NULL);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(GTK_WINDOW(window), WIDTH, HEIGHT);
    gtk_window_set_title(GTK_WINDOW(window), "Graph drawing");

    da = gtk_drawing_area_new();
    gtk_container_add(GTK_CONTAINER(window), da);

    g_signal_connect(G_OBJECT(window), "destroy", gtk_main_quit, NULL);
    g_signal_connect(G_OBJECT(da), "draw", G_CALLBACK(on_draw), sig);

    context = g_main_context_default();

    for (int n = 0; n < N_THREADS; ++n)
        thread[n] = g_thread_new(NULL, thread_func, (gpointer)sig);

    gtk_widget_show_all(window);
    gtk_main();

    for (int n = 0; n < N_THREADS; ++n)
        g_thread_join(thread[n]);

    free(sig);
    return 0;
}
