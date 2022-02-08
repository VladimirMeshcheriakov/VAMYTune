#include "note_state_utils.h"


void init_piano_keys(const Uint8 *state, Uint8 *piano_keys)
{
    piano_keys[0] = state[22];
    piano_keys[1] = state[8];
    piano_keys[2] = state[7];
    piano_keys[3] = state[21];
    piano_keys[4] = state[9];
    piano_keys[5] = state[10];
    piano_keys[6] = state[28];
    piano_keys[7] = state[11];
    piano_keys[8] = state[24];
    piano_keys[9] = state[13];
    piano_keys[10] = state[12];
    piano_keys[11] = state[14];
    piano_keys[12] = state[15];
}

void key_on(ud *data, int note_pos)
{
    if (!(data->time_table[note_pos]->press_time_set))
    {
        data->time_table[note_pos]->press_time = data->actual_time;
        data->time_table[note_pos]->press_time_set = 1;
        data->time_table[note_pos]->released = 0;
    }
}

void key_off(ud *data, int note_pos)
{
    data->time_table[note_pos]->press_time_set = 0;
    data->time_table[note_pos]->released = 1;
    data->time_table[note_pos]->release_stage = 1;
    data->time_table[note_pos]->stop_time = data->actual_time;
}

void note_state_change( ud *data, int key_id)
{
    if (!(data->all_keys->keys[key_id]))
    {
        // KEY IS ON
        key_on(data, key_id);
    }
    else
    {
        // KEY IS OFF
        key_off(data, key_id);
    }
}

void note_state(const Uint8 *state, ud *data)
{
    Uint8 *past_occ = data->all_keys->keys; 
    if (state[22] != past_occ[0])
    {
        // STATE CHANGED
        note_state_change( data, 0);
    }
    if (state[8] != past_occ[1])
    {
        // STATE CHANGED
        note_state_change(data, 1);
    }
    if (state[7] != past_occ[2])
    {
        // STATE CHANGED
        note_state_change(data, 2);
    }
    if (state[21] != past_occ[3])
    {
        // STATE CHANGED
        note_state_change( data, 3);
    }
    if (state[9] != past_occ[4])
    {
        // STATE CHANGED
        note_state_change(data, 4);
    }
    if (state[10] != past_occ[5])
    {
        // STATE CHANGED
        note_state_change(data, 5);
    }
    if (state[28] != past_occ[6])
    {
        // STATE CHANGED
        note_state_change(data, 6);
    }
    if (state[11] != past_occ[7])
    {
        // STATE CHANGED
        note_state_change(data, 7);
    }
    if (state[24] != past_occ[8])
    {
        // STATE CHANGED
        note_state_change( data, 8);
    }
    if (state[13] != past_occ[9])
    {
        // STATE CHANGED
        note_state_change(data, 9);
    }
    if (state[12] != past_occ[10])
    {
        // STATE CHANGED
        note_state_change( data, 10);
    }
    if (state[14] != past_occ[11])
    {
        // STATE CHANGED
        note_state_change( data, 11);
    }
    if (state[15] != past_occ[12])
    {
        // STATE CHANGED
        note_state_change(data, 12);
    }
    // update all the effect values for each note
}