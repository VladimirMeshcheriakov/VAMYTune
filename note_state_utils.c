#include "note_state_utils.h"

//Init the vector of keys and their state
void init_piano_keys(const Uint8 *state, ud *data)
{
    Uint8 *piano_keys = data->all_keys->keys;
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
    //Side effects
    data->side_effect[0] = state[35];
    data->side_effect[1] = state[46];
    //Enter
    data->side_effect[2] = state[40];
    //Backspace
    data->side_effect[3] = state[42];
    //P - > playback
    data->side_effect[4] = state[0];
    //O -> stop playback
    data->side_effect[5] = state[0];
}


//Do the key on properties
void key_on(ud *data, int note_pos)
{
    //if this key has been pressed already, it does not have to be re-set
    if (!(data->time_table[note_pos]->press_time_set))
    {
        //However if it has not been set
        //Actual time set
        data->time_table[note_pos]->press_time = data->actual_time;
        //Press was set
        data->time_table[note_pos]->press_time_set = 1;
        //It has not yet been released
        data->time_table[note_pos]->released = 0;
    }
}

//Do the key off properties
void key_off(ud *data, int note_pos)
{
    //It is no longer pressed
    data->time_table[note_pos]->press_time_set = 0;
    //It is now released
    data->time_table[note_pos]->released = 1;
    //We are enterring the release stage
    data->time_table[note_pos]->release_stage = 1;
    //The stop time is saved
    data->time_table[note_pos]->stop_time = data->actual_time;
}

//Based on the state change executes a function 
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

void print_keyboard_state(const Uint8 *piano_keys)
{
    printf(" _______________________________________________________\n");
    printf("|  |     |  |     |   |   |     |  |     |  |     |  |  |     \n");
    printf("|  |     |  |     |   |   |     |  |     |  |     |  |  |     \n");
    printf(
        "|  |  %d  |  |  %d  |   |   |  %d  |  |  %d  |  |  %d  |  |  |    "
        " \n",
        piano_keys[1], piano_keys[3], piano_keys[6], piano_keys[8],
        piano_keys[10]);
    printf("|  |     |  |     |   |   |     |  |     |  |     |  |  |     \n");
    printf("|  |_____|  |_____|   |   |_____|  |_____|  |_____|  |  |__   \n");
    printf("|     |        |      |      |        |        |     |     |  \n");
    printf("|     |        |      |      |        |        |     |     |  \n");
    printf(
        "|  %d  |   %d    |  %d   |  %d   |   %d    |   %d    |  %d  |  %d "
        " |  \n",
        piano_keys[0], piano_keys[2], piano_keys[4], piano_keys[5],
        piano_keys[7], piano_keys[9], piano_keys[11], piano_keys[12]);
    printf("|     |        |      |      |        |        |     |     |  \n");
    printf("|_____|________|______|______|________|________|_____|_____|  \n");
}

void octave_set(const Uint8 *state, ud *data)
{
    //Minus
    if(state[35] != data->side_effect[0])
    {
        if(state[35])
        {
            data->octave = data->octave/2.0;
        }
    }
    //Plus
    if(state[46] != data->side_effect[1])

    {
        if(state[46])
        {
            data->octave = data->octave*2.0;
        }
    }
}

void recording_set(const Uint8 *state, ud *data)
{
    //Start
    if(state[40] != data->side_effect[2])
    {
        if(state[40])
        {
            data->record = 1;
        }
    }
    //Stop
    if(state[42] != data->side_effect[3])
    {
        if(state[42])
        {
            data->record = 0;
        }
    }
}

void playback_set(const Uint8 *state, ud *data)
{
    //P
    if(state[19] != data->side_effect[4])
    {
        if(state[19])
        {
            data->playback = 1;
        }
    }
    //O
    if(state[18] != data->side_effect[5])
    {
        if(state[18])
        {
            data->playback= 0;
        }
    }
}




//Main state function that verifies and triggers a state change on each note
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
    octave_set(state,data);
    recording_set(state,data);
    playback_set(state,data);
}