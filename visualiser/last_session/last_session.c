#include "last_session.h"


void on_dismiss_last_session( __attribute_maybe_unused__ GtkWidget * widget, gpointer user_data)
{
    GtkDialog *session_dialog = (GtkDialog *)user_data;
    gtk_widget_destroy(GTK_WIDGET(session_dialog));
}

void on_load_last_session(__attribute_maybe_unused__  GtkWidget * widget, gpointer user_data)
{
    load_from_triton("visualiser/last_session/last_session.triton",1);
    GtkDialog *session_dialog = (GtkDialog *)user_data;
    gtk_widget_destroy(GTK_WIDGET(session_dialog));
}

void on_start_app_last_session_prompt()
{
    GtkBuilder *builder = gtk_builder_new();
    GError *error = NULL;

    if (gtk_builder_add_from_file(builder, "visualiser/last_session/last_session.glade", &error) == 0)
    {
        g_printerr("Error loading file: %s\n", error->message);
        g_clear_error(&error);
    }

    GtkDialog *session_dialog = GTK_DIALOG(gtk_builder_get_object(builder, "session_dialog"));
    GtkButton *load_session = GTK_BUTTON(gtk_builder_get_object(builder, "load_session"));
    GtkButton *continue_session = GTK_BUTTON(gtk_builder_get_object(builder, "continue_session"));

    g_signal_connect(G_OBJECT(continue_session),"clicked",G_CALLBACK(on_dismiss_last_session),session_dialog);
    g_signal_connect(G_OBJECT(load_session),"clicked",G_CALLBACK(on_load_last_session),session_dialog);

    gtk_dialog_run(GTK_DIALOG(session_dialog));
}