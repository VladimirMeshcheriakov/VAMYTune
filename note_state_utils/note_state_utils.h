#include "../userdata/userdata.h"

#ifndef NOTE_STATE_UTILS_H
#define NOTE_STATE_UTILS_H

void key_on(ud *data, int note_pos);
void key_off(ud *data, int note_pos);
void print_keyboard_state(Uint8 *piano_keys);

#endif