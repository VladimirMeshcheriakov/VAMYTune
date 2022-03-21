#include "../userdata/userdata.h"

#ifndef NOTE_STATE_UTILS_H
#define NOTE_STATE_UTILS_H

void init_piano_keys( Uint8 *state, ud *data);
void note_state( Uint8 *state, ud *data);
void print_keyboard_state(Uint8 *piano_keys);

#endif