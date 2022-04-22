#include "list_node_adder.h"



void row_create(__attribute_maybe_unused__  GtkWidget *button, gpointer userdata)
{
  sig_info *sig_data = (sig_info *)userdata;
  print_sine_info(sig_data);
  GtkBuilder *builder = gtk_builder_new();
  GError *error = NULL;
  if (gtk_builder_add_from_file(builder, global_file_name, &error) == 0)
  {
    g_printerr("Error loading file: %s\n", error->message);
    g_clear_error(&error);
  }


  GtkBox *sine_box = GTK_BOX(gtk_builder_get_object(builder, "main_box"));
  GtkBox *control_box = GTK_BOX(gtk_builder_get_object(builder, "control_box"));
  GtkButton *delete_button = GTK_BUTTON(gtk_builder_get_object(builder, "delete"));
  GtkDrawingArea *sin_da = GTK_DRAWING_AREA(gtk_builder_get_object(builder, "sig_da"));
  GtkScale *amp = GTK_SCALE(gtk_builder_get_object(builder, "sig_amp"));
  GtkScale *freq = GTK_SCALE(gtk_builder_get_object(builder, "sig_freq"));
  GtkScale *phase = GTK_SCALE(gtk_builder_get_object(builder,"sig_phase"));
  GtkToggleButton *mute = GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder, "mute"));
  GtkLabel *name_label = GTK_LABEL(gtk_builder_get_object(builder, "main_label"));

  all_params_and_id *all_params = prepare_all_params(&sig_data->id);

  switch (sig_data->type)
  {
  case 0:
    gtk_label_set_text(name_label, "Sine");
    break;
  case 1:
    gtk_label_set_text(name_label, "Triangle");
    break;
  case 2:
    gtk_label_set_text(name_label, "Saw");
    GtkAdjustment * adjustment = gtk_adjustment_new(0,0,1,1,0,0);
    GtkWidget* invert = gtk_scale_new(GTK_ORIENTATION_HORIZONTAL,adjustment);
    gtk_box_pack_end(control_box,GTK_WIDGET(invert),TRUE,TRUE,0);
    g_signal_connect(G_OBJECT(invert), "value_changed", G_CALLBACK(on_scale_change_param), (gpointer)all_params->inverse);
    break;
  }

  // Append the row to the list, the last parameter must be at -1 to allow to append at the end
  gtk_list_box_insert(list, GTK_WIDGET(sine_box), -1);

  

  g_signal_connect(G_OBJECT(delete_button), "clicked", G_CALLBACK(on_delete_node_params), (gpointer)all_params);
  g_signal_connect(G_OBJECT(sin_da), "draw", G_CALLBACK(on_draw_created_or_full_signal), (gpointer)&sig_data->id);
  g_signal_connect(G_OBJECT(amp), "value_changed", G_CALLBACK(on_scale_change_param), (gpointer)all_params->amp);
  g_signal_connect(G_OBJECT(freq), "value_changed", G_CALLBACK(on_scale_change_param), (gpointer)all_params->freq);
  g_signal_connect(G_OBJECT(phase), "value_changed", G_CALLBACK(on_scale_change_param), (gpointer)all_params->phase);
  g_signal_connect(G_OBJECT(mute), "toggled", G_CALLBACK(on_toggle), (gpointer)&sig_data->id);
  global_id += 1;

  gtk_range_set_value(GTK_RANGE(amp), sig_data->amp);
  gtk_range_set_value(GTK_RANGE(freq), sig_data->freq);
  gtk_range_set_value(GTK_RANGE(phase), sig_data->phase);

  gtk_widget_show_all(GTK_WIDGET(list));
  gtk_widget_show_all(GTK_WIDGET(sin_da));
}

void row_create_composite(__attribute_maybe_unused__  GtkWidget *button, gpointer userdata)
{
  sig_info *sig_data = (sig_info *)userdata;
  GtkBuilder *builder = gtk_builder_new();
  GError *error = NULL;
  if (gtk_builder_add_from_file(builder, global_file_name_components, &error) == 0)
  {
    g_printerr("Error loading file: %s\n", error->message);
    g_clear_error(&error);
  }

  GtkBox *sine_box = GTK_BOX(gtk_builder_get_object(builder, "main_box"));
  GtkButton *delete_button = GTK_BUTTON(gtk_builder_get_object(builder, "delete"));
  GtkDrawingArea *sin_da = GTK_DRAWING_AREA(gtk_builder_get_object(builder, "sig_da"));
  GtkScale *amp = GTK_SCALE(gtk_builder_get_object(builder, "sig_amp"));
  GtkScale *freq = GTK_SCALE(gtk_builder_get_object(builder, "sig_freq"));
  GtkScale *components = GTK_SCALE(gtk_builder_get_object(builder, "sig_components"));
  GtkScale *phase = GTK_SCALE(gtk_builder_get_object(builder,"sig_phase"));
  GtkToggleButton *mute = GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder, "mute"));
  GtkLabel *name_label = GTK_LABEL(gtk_builder_get_object(builder, "main_label"));
  GtkAdjustment *adj_format = GTK_ADJUSTMENT(gtk_builder_get_object(builder, "adj3"));

  switch (sig_data->type)
  {
  case 3:
    gtk_label_set_text(name_label, "Saw Composite");
    break;
  case 4:
    gtk_label_set_text(name_label, "Square");
    // Change the adjustment parameter
    gtk_adjustment_configure(adj_format, 0.0, -1.0, 1.0, 0.01, 10, 0);
    gtk_scale_set_digits(components, 2);
    break;
  }

  // Append the row to the list, the last parameter must be at -1 to allow to append at the end
  gtk_list_box_insert(list, GTK_WIDGET(sine_box), -1);
  all_params_and_id *all_params = prepare_all_params(&sig_data->id);

  g_signal_connect(G_OBJECT(delete_button), "clicked", G_CALLBACK(on_delete_node_params), (gpointer)all_params);
  g_signal_connect(G_OBJECT(sin_da), "draw", G_CALLBACK(on_draw_created_or_full_signal), (gpointer)&sig_data->id);
  g_signal_connect(G_OBJECT(amp), "value_changed", G_CALLBACK(on_scale_change_param), (gpointer)all_params->amp);
  g_signal_connect(G_OBJECT(freq), "value_changed", G_CALLBACK(on_scale_change_param), (gpointer)all_params->freq);
  g_signal_connect(G_OBJECT(components), "value_changed", G_CALLBACK(on_scale_change_param), (gpointer)all_params->composite);
  g_signal_connect(G_OBJECT(phase), "value_changed", G_CALLBACK(on_scale_change_param), (gpointer)all_params->phase);
  g_signal_connect(G_OBJECT(mute), "toggled", G_CALLBACK(on_toggle), (gpointer)&sig_data->id);
  global_id += 1;

  gtk_range_set_value(GTK_RANGE(amp), sig_data->amp);
  gtk_range_set_value(GTK_RANGE(freq), sig_data->freq);
  gtk_range_set_value(GTK_RANGE(phase), sig_data->phase);
  gtk_range_set_value(GTK_RANGE(components), sig_data->form);

  gtk_widget_show_all(GTK_WIDGET(list));
  gtk_widget_show_all(GTK_WIDGET(sin_da));
}

sig_info *create_and_append_null_sig_info(int *type)
{
  sig_info *sig_data = init_null_struct();
  sig_data->type = *type;
  sig_data->id = global_id;
  node_insert_end(nodes, sig_data);
  return sig_data;
}

gboolean init_and_create_row(GtkWidget *button, gpointer userdata)
{
  int *type = (int *)userdata;
  sig_info *sig_data = create_and_append_null_sig_info(type);
  row_create(button, sig_data);
  return G_SOURCE_REMOVE;
}

gboolean init_and_create_row_composite(GtkWidget *button, gpointer userdata)
{
  int *type = (int *)userdata;
  sig_info *sig_data = create_and_append_null_sig_info(type);
  row_create_composite(button, sig_data);
  return G_SOURCE_REMOVE;
}


