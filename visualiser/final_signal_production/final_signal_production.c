#include "final_signal_production.h"

// Calculates one value of one instance
float instance_signal(float volume, sig_info *vs, double time, float freq)
{
  float he = 0;
  switch (vs->type)
  {
  case 0:
    he += vs->amp * sine(volume, freq * vs->freq, time, vs->phase);
    break;
  case 1:
    he += vs->amp * triangle(volume, freq * vs->freq, time, vs->phase);
    break;
  case 2:
    he += vs->amp * saw(volume, freq * vs->freq, time, vs->phase, vs->inverse);
    break;
  case 3:
    he += vs->amp * saw2(volume, freq * vs->freq, time, vs->form + 1, vs->phase);
    break;
  case 4:
    he += vs->amp * square(volume, freq * vs->freq, time, vs->form, vs->phase);
    break;
  }
  return he;
}

void global_signal_drawing()
{
  node *node_copy = nodes;
  float *final_sig = node_copy->value->signal;
  memset(final_sig, 0, 4096);
  while (node_copy->next)
  {
    node_copy = node_copy->next;
    if (node_copy->value->mute == 0)
    {
      int cpt = 0;
      for (size_t i = 0; i < 1024; i++)
      {
        double time = cpt / 44100.0;
        final_sig[i] += instance_signal(0.5, node_copy->value, time, global_freq);
        cpt += 1;
      }
    }
  }
}

void update_current_note_index(unsigned short freq, unsigned short * indexes)
{
  if(indexes[freq]+freq < 44100)
  {
    indexes[freq] += freq;
  }
  else
  {
    indexes[freq] += freq;
    indexes[freq] -= 44100;
  }
}

// Calculates the global signal
float global_signal(float volume, float freq)
{
  float he = 0;
  node *id_node = nodes->next;
  while (id_node != NULL)
  {
    sig_info *vs = id_node->value;
    if (!vs->mute)
    {
      update_current_note_index((unsigned short)freq,vs->current_index);
      he += volume * vs->ewt[vs->current_index[(unsigned short)freq]];
    }
    id_node = id_node->next;
  }
  return he;
}

void apply_filter_to_sample(vis_data *data, size_t size)
{

  float *buf = data->sig_sample;
  float *mag = data->harmonics_sample;
  float *rep = data->filtered_sample;

  for (size_t i = 0; i < 1024; i++)
  {
    float val = global_signal(0.6,  440);
    buf[i] = val;
    rep[i] = val;
  }

  complex_number *arr = fft(buf, size);

  // put your filter here

  if (data->low_active)
  {
    filter_cut_from(arr, size, (size_t)((data->low_pass_cut * 1024) / 44000));
  }
  if (data->high_active)
  {
    filter_cut_before(arr, (size_t)((data->high_pass_cut * 1024) / 44000), size);
  }
  if (data->band_pass_active)
  {
    filter_cut_around(arr, (size_t)((data->band_pass_low * 1024) / 44000), (size_t)((data->band_pass_high * 1024) / 44000), size);
  }
  if (data->band_cut_active)
  {
    filter_cut_between(arr, (size_t)((data->band_cut_low * 1024) / 44000), (size_t)((data->band_cut_high * 1024) / 44000), size);
  }

  mag_table(arr, mag, size);

  ifft(arr, rep, size);
  free(arr);
}

// Evaluate the stage execution of a signal and send the final value
float signal_treat(float volume, ud *data)
{
  float val = 0.0;

  for (int i = 0; i < 127; i++)
  {
    if (data->all_keys->keys[i] || (data->time_management->time_table[i]->release_stage && (data->all_keys->effects[i] > 0.0)))
    {
      val += data->all_keys->effects[i] * global_signal(volume,  piano_note_to_freq(i));
    }
    else
    {
      data->time_management->time_table[i]->release_stage = 0;
    }
  }
  return val;
}
