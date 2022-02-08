#include <stdlib.h>
#include "userdata.h"

#ifndef NOTE_STATE_UTILS_H
#define NOTE_STATE_UTILS_H

void init_piano_keys(const Uint8 *state, Uint8 *piano_keys);
void note_state(const Uint8 *state, ud *data);


#endif