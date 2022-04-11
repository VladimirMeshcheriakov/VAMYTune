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


char * create_string_from_signal(sig_info * info)
{
    char * buffer = calloc(100,sizeof(char));
    if(info->type< 3)
    {
        sprintf(buffer,"[%d;,\n%.3f;,\n%.3f;,\n],\n",info->type,info->amp*100.0, info->freq*100.0);
    }
    else
    {
        sprintf(buffer,"[%d;,\n%.3f;,\n%.3f;,\n%.3f;,\n],\n",info->type,info->amp*100.0, info->freq*100.0, info->form*100);
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

      //printf("Beg of Signals\n");
      open_brace_occ += 1;
      // Signals parameters open
      while (arr[i] != '}')
      {

        // Parse Signals params
        if (arr[i] == '[')
        {
          i += 1;
          //printf("Beg of Sig Parameter\n");
          // A param was spotted
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
                //printf("Info type is set to %d \n", info->type);
                break;
              case 1:
                info->amp = (test / 100.00);
                //printf("Info amp is set to %f \n", test);
                break;
              case 2:
                info->freq = (test / 100.00);
                //printf("Info freq is set to %f \n", test);
                break;
              case 3:
                info->form = test;
                //printf("Info form is set to %f \n", test);
                break;
              }
              param_num += 1;
              //printf("para_num is at %d\n", param_num);
              i += 1;
            }
            i += 2;
            closed_brace_occ += 1;
          }
          node_insert_end(params->signals, info);
          //printf("\n");
          //printf("End of Sig Parameter\n");
          // Add param
        }
        // Onto next parameter
        i += 1;
        closed_brace_occ += 1;
      }
      //printf("End of Signals\n");
      // End of parsing
      break;
    }
  }
}



void write_to_triton(node * nodes)
{
    //Beg of file
    FILE *file;
    file = fopen("out.triton", "w");
    if(file == NULL)
    {
        printf("error\n");
    }
    fputc('{',file);
    fputc('\n',file);
    
   char * buffer_full = calloc(100 * node_val_count(nodes) ,sizeof(char));
    while (nodes->next != NULL)
    {
        nodes = nodes->next;
        strcat(buffer_full,create_string_from_signal(nodes->value));
        
    }
    //printf("%s\n",buffer_full);
    fputs(buffer_full,file);
    free(buffer_full);
    //End of file
    fputc('}',file);
    fputc('\n',file);
    fclose(file);
}