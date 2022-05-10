#include "../note_state_utils/note_state_utils.h"
#include "../wav_interface/wav_interface.h"
#include "../basic_signals/basic_signals.h"
#include "../FFT/dft.h"

#ifndef SIGNALS_H
#define SIGNALS_H

float piano_note_to_freq(int c);
void update_effects(vis_data *data);
#endif
