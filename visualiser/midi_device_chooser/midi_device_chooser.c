#include "midi_device_chooser.h"

void on_choose_midi_device(__attribute_maybe_unused__ GtkWidget *widget, gpointer user_data)
{
    midi_device_info *new_info = (midi_device_info *)user_data;
    GtkWidget * parent = gtk_widget_get_parent(widget);
    parent = gtk_widget_get_parent(parent);
    parent = gtk_widget_get_parent(parent);
    parent = gtk_widget_get_parent(parent);
    parent = gtk_widget_get_parent(parent);
    int port_id = new_info->id;
    char str[10];
    sprintf(str, "%d", port_id);
    connect_to_port(str);
    midi_seq = create_port();
    connect_ports(midi_seq);
    gtk_widget_destroy(parent);
}

void free_midi_device_info(__attribute_maybe_unused__ GtkWidget *widget, gpointer user_data)
{
    midi_device_info *new_info = (midi_device_info *)user_data;
    free(new_info);
}

void on_cancel_dialog(__attribute_maybe_unused__ GtkWidget *widget, __attribute_maybe_unused__ gpointer user_data)
{
    GtkDialog *dialog = (GtkDialog *)user_data;
    gtk_widget_destroy(GTK_WIDGET(dialog));
}

void midi_device_add_row(GtkListBox *midi_device_list, int id, const char *name)
{
    GtkBuilder *builder = gtk_builder_new();
    GError *error = NULL;

    if (gtk_builder_add_from_file(builder, "visualiser/midi_device_chooser/midi_device_row.glade", &error) == 0)
    {
        g_printerr("Error loading file: %s\n", error->message);
        g_clear_error(&error);
    }

    midi_device_info *new_info = malloc(sizeof(midi_device_info));
    new_info->id = id;
    new_info->name = name;

    GtkBox *box_midi = GTK_BOX(gtk_builder_get_object(builder, "midi_box"));
    GtkButton *choose_device = GTK_BUTTON(gtk_builder_get_object(builder, "choose_device"));
    GtkLabel *midi_device_label = GTK_LABEL(gtk_builder_get_object(builder, "midi_device_label"));

    gtk_label_set_text(midi_device_label, name);

    g_signal_connect(G_OBJECT(choose_device), "clicked", G_CALLBACK(on_choose_midi_device), new_info);
    g_signal_connect(G_OBJECT(box_midi), "destroy", G_CALLBACK(free_midi_device_info), new_info);

    gtk_list_box_insert(midi_device_list, GTK_WIDGET(box_midi), -1);
}

void midi_device_chooser_create(__attribute_maybe_unused__ GtkWidget *button, __attribute_maybe_unused__ gpointer userdata)
{
    GtkBuilder *builder = gtk_builder_new();
    GError *error = NULL;
    if (gtk_builder_add_from_file(builder, "visualiser/midi_device_chooser/midi_device_chooser.glade", &error) == 0)
    {
        g_printerr("Error loading file: %s\n", error->message);
        g_clear_error(&error);
    }

    GtkDialog *midi_device_chooser_dialog = GTK_DIALOG(gtk_builder_get_object(builder, "midi_device_chooser_dialog"));
    GtkButton *cancel_midi_device_choise = GTK_BUTTON(gtk_builder_get_object(builder, "cancel_midi_device_choise"));
    GtkButton *accept_midi_device_choise = GTK_BUTTON(gtk_builder_get_object(builder, "accept_midi_device_choise"));
    GtkListBox *midi_device_list = GTK_LIST_BOX(gtk_builder_get_object(builder, "midi_device_list"));

    snd_seq_client_info_t *cinfo;
    snd_seq_port_info_t *pinfo;
    snd_seq_client_info_alloca(&cinfo);
    snd_seq_port_info_alloca(&pinfo);

    snd_seq_client_info_set_client(cinfo, -1);
    while (snd_seq_query_next_client(midi_seq, cinfo) >= 0)
    {
        int client = snd_seq_client_info_get_client(cinfo);

        snd_seq_port_info_set_client(pinfo, client);
        snd_seq_port_info_set_port(pinfo, -1);
        while (snd_seq_query_next_port(midi_seq, pinfo) >= 0)
        {
            /* we need both READ and SUBS_READ */
            if ((snd_seq_port_info_get_capability(pinfo) & (SND_SEQ_PORT_CAP_READ | SND_SEQ_PORT_CAP_SUBS_READ)) != (SND_SEQ_PORT_CAP_READ | SND_SEQ_PORT_CAP_SUBS_READ))
                continue;
            if (snd_seq_port_info_get_client(pinfo) != 0 && snd_seq_port_info_get_client(pinfo) != 1 && snd_seq_port_info_get_client(pinfo) != 14)
            {
                midi_device_add_row(midi_device_list, snd_seq_port_info_get_client(pinfo), snd_seq_client_info_get_name(cinfo));
            }
        }
    }
    g_signal_connect(G_OBJECT(cancel_midi_device_choise), "clicked", G_CALLBACK(on_cancel_dialog), midi_device_chooser_dialog);
    g_signal_connect(G_OBJECT(accept_midi_device_choise), "clicked", G_CALLBACK(on_cancel_dialog), midi_device_chooser_dialog);

    gtk_dialog_run(GTK_DIALOG(midi_device_chooser_dialog));
}
