#include "load_save_triton.h"

signal_params *init_signal_params()
{
  signal_params *params = calloc(1, sizeof(signal_params));
  params->attack_amp = 0;
  params->attack_phase = 0;
  params->band_cut_active = 0;
  params->band_cut_high = 0;
  params->band_cut_low = 0;
  params->band_pass_active = 0;
  params->band_pass_high = 0;
  params->band_pass_low = 0;
  params->decay_amp = 0;
  params->decay_phase = 0;
  params->sustain_amp = 0;
  params->high_active = 0;
  params->high_pass_cut = 0;
  params->low_active = 0;
  params->low_pass_cut = 0;
  params->release_phase = 0;
  params->signals = node_build_sentinel();
  return params;
}

char *create_string_from_signal(sig_info *info)
{
  char *buffer = calloc(100, sizeof(char));
  if (info->type < 3)
  {
    sprintf(buffer, "[%d;,\n%.3f;,\n%.3f;,\n%.3f;,\n],\n", info->type, info->amp * 100.0, info->freq * 100.0, info->phase * 100);
  }
  else
  {
    sprintf(buffer, "[%d;,\n%.3f;,\n%.3f;,\n%.3f;,\n%.3f;,\n],\n", info->type, info->amp * 100.0, info->freq * 100.0, info->form * 100, info->phase * 100);
  }
  return buffer;
}

void find_scopes(char *arr, size_t len, signal_params *params)
{
  int open_brace_occ = 0;
  int closed_brace_occ = 0;
  for (size_t i = 0; i < len; i++)
  {
    if (arr[i] == '{' && open_brace_occ == 0)
    {

      // printf("Beg of Signals\n");
      open_brace_occ += 1;
      // Signals parameters open
      while (arr[i] != '}')
      {

        // Parse Signals params
        if (arr[i] == '[')
        {
          i += 1;
          // printf("Beg of Sig Parameter\n");
          //  A param was spotted
          sig_info *info = init_null_struct();
          int param_num = 0;
          while (arr[i] != ']')
          {

            while (arr[i] != ',')
            {
              int cpt = 0;
              char val[10];
              // One line params
              while (arr[i] != ';')
              {
                val[cpt] = arr[i];
                // Single value
                cpt += 1;
                i += 1;
              }
              val[cpt] = '\n';
              float test = atof(val);
              switch (param_num)
              {
              case 0:
                info->type = test;
                // printf("Info type is set to %d \n", info->type);
                break;
              case 1:
                info->amp = (test / 100.00);
                // printf("Info amp is set to %f \n", test);
                break;
              case 2:
                info->freq = (test / 100.00);
                // printf("Info freq is set to %f \n", test);
                break;
              case 3:
                if (info->type < 3)
                {
                  info->phase = (test / 100.00);
                }
                else
                {
                  info->form = (test / 100.00);
                }
                // printf("Info form is set to %f \n", test);
                break;
              case 4:
                info->phase = (test / 100.00);
                break;
              case 5:
                info->inverse = (test / 100.00);
                break;
              }
              param_num += 1;
              // printf("para_num is at %d\n", param_num);
              i += 1;
            }
            i += 2;
            closed_brace_occ += 1;
          }
          node_insert_end(params->signals, info);
          // printf("\n");
          // printf("End of Sig Parameter\n");
          //  Add param
        }
        // Onto next parameter
        i += 1;
        closed_brace_occ += 1;
      }
      // printf("End of Signals\n");
      //  End of parsing
      break;
    }
  }
  // node_print(params->signals);
}

void write_to_triton(node *nodes, const char *name)
{
  // Beg of file
  FILE *file;
  file = fopen(name, "w");
  if (file == NULL)
  {
    printf("error\n");
  }
  fputc('{', file);
  fputc('\n', file);

  char *buffer_full = calloc(100 * node_val_count(nodes), sizeof(char));
  while (nodes->next != NULL)
  {
    nodes = nodes->next;
    strcat(buffer_full, create_string_from_signal(nodes->value));
  }
  // printf("%s\n",buffer_full);
  fputs(buffer_full, file);
  free(buffer_full);
  // End of file
  fputc('}', file);
  fputc('\n', file);
  fclose(file);
}

int load_from_triton(const char *uri, int uri_or_path)
{
  GFile *file;
  if (uri_or_path)
  {
    file = g_file_new_for_path(uri);
    if (file == NULL)
    {
      g_print("file null\n");
      return G_SOURCE_REMOVE;
    }
  }
  else
  {
    file = g_file_new_for_uri(uri);
    if (file == NULL)
    {
      g_print("file null\n");
      return G_SOURCE_REMOVE;
    }
  }

  GBytes *file_bytes = g_file_load_bytes(file, NULL, NULL, NULL);
  if (file_bytes == NULL)
  {
    g_print("bytes null\n");
    return G_SOURCE_REMOVE;
  }
  size_t data_size = 0;
  const char *pointer = g_bytes_get_data(file_bytes, &data_size);
  if (pointer == NULL)
  {
    g_print("pointer null\n");
    return G_SOURCE_REMOVE;
  }
  signal_params *params = init_signal_params();
  find_scopes((char*)pointer, data_size, params);
  free((char*)pointer);
  node *tmp = params->signals;
  params->signals = params->signals->next;
  while (params->signals != NULL)
  {
    if (params->signals->value->type < 3)
    {
      // Simple types
      sig_info *sig_data = params->signals->value;
      sig_data->id = global_id;
      node_insert_end(nodes, sig_data);
      row_create(NULL, (gpointer)sig_data);
    }
    else
    {
      sig_info *sig_data = params->signals->value;
      sig_data->id = global_id;
      node_insert_end(nodes, sig_data);
      row_create_composite(NULL, (gpointer)sig_data);
    }
    params->signals = params->signals->next;
  }
  // Free the signal of the sentinell node
  free(tmp->value->signal);
  // Free the sig info of a sentinell node
  free(tmp->value);

  node_free(tmp);
  free(params);
  g_object_unref(file);
  return G_SOURCE_REMOVE;
}

gboolean update_preview_cb(GtkFileChooser *file_chooser, __attribute_maybe_unused__ gpointer data)
{
  const char *uri = gtk_file_chooser_get_uri(file_chooser);
  
  printf("%s\n", uri);
  if (uri == NULL)
  {
    g_print("uri null\n");
    return G_SOURCE_REMOVE;
  }
  load_from_triton(uri,0);
  return G_SOURCE_REMOVE;
}

gboolean update_new_wav_file(GtkFileChooser *file_chooser, __attribute_maybe_unused__ gpointer data)
{
  const char *uri = gtk_file_chooser_get_filename(file_chooser);
  ud* us_d = (ud*) data;
  printf("%s\n", uri);
  if (uri == NULL)
  {
    g_print("uri null\n");
    return G_SOURCE_REMOVE;
  }
  set_new_working_wav_file(us_d,uri);

  return G_SOURCE_REMOVE;
}

void on_save_file(__attribute_maybe_unused__ GtkWidget *widget, __attribute_maybe_unused__ gpointer data)
{
  GtkEntry *entry = (GtkEntry *)data;
  GtkWidget *parent = gtk_widget_get_parent(GTK_WIDGET(entry));
  parent = gtk_widget_get_parent(parent);
  parent = gtk_widget_get_parent(parent);
  const gchar *text = gtk_entry_get_text(GTK_ENTRY(entry));
  write_to_triton(nodes, text);
  gtk_widget_destroy(GTK_WIDGET(parent));
}

void on_cancel_file(__attribute_maybe_unused__ GtkWidget *widget, __attribute_maybe_unused__ gpointer user_data)
{
  GtkDialog *dialog = (GtkDialog *)user_data;
  gtk_widget_destroy(GTK_WIDGET(dialog));
}

gboolean on_save_state(__attribute_maybe_unused__ GtkButton *a_button)
{
  GtkBuilder *builder = gtk_builder_new();
  GError *error = NULL;

  if (gtk_builder_add_from_file(builder, "visualiser/load_save_triton/load_save.glade", &error) == 0)
  {
    g_printerr("Error loading file: %s\n", error->message);
    g_clear_error(&error);
  }

  GtkDialog *file_chooser_dialog = GTK_DIALOG(gtk_builder_get_object(builder, "file_chooser_dialog"));
  GtkButton *file_chooser_cancel = GTK_BUTTON(gtk_builder_get_object(builder, "file_chooser_cancel"));
  GtkButton *file_chooser_save = GTK_BUTTON(gtk_builder_get_object(builder, "file_chooser_save"));
  GtkEntry *file_chooser_text_entry = GTK_ENTRY(gtk_builder_get_object(builder, "file_chooser_text_entry"));

  g_signal_connect(G_OBJECT(file_chooser_save), "clicked", G_CALLBACK(on_save_file), file_chooser_text_entry);
  g_signal_connect(G_OBJECT(file_chooser_cancel), "clicked", G_CALLBACK(on_cancel_file), file_chooser_dialog);

  gtk_dialog_run(GTK_DIALOG(file_chooser_dialog));
  return G_SOURCE_REMOVE;
}