#include "../userdata/userdata.h"

#ifndef NOTE_STATE_UTILS_H
#define NOTE_STATE_UTILS_H

void init_piano_keys( Uint8 *state, ud *data);
void key_on(ud *data, int note_pos);
void key_off(ud *data, int note_pos);
void note_state( Uint8 *state, ud *data);
void note_state_change(ud *data, int key_id);
void print_keyboard_state(Uint8 *piano_keys);

#endif