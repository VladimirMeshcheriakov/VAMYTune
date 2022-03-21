#include "note_state_utils.h"

// Init the vector of keys and their state
void init_piano_keys(Uint8 *state, ud *data)
{
    Uint8 *piano_keys = data->all_keys->keys;
    for (int i = 0; i < 127; i++)
    {
        piano_keys[i] = state[i];
    }
    /*
    Uint8 *past_occ = data->all_keys->keys;
    for (int i = 0; i < 127; i++)
    {
        if (state[i] != past_occ[i])
        {
            // STATE CHANGED
            note_state_change(data, i);
        }
    }
    */
    /*
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
    */
}

// Do the key on properties
void key_on(ud *data, int note_pos)
{
    // if this key has been pressed already, it does not have to be re-set
    if (!(data->time_management->time_table[note_pos]->press_time_set))
    {
        // Actual time set
        data->time_management->time_table[note_pos]->press_time = data->time_management->actual_time;
        //printf("Press time %f\n",data->time_management->time_table[note_pos]->press_time );
        // Press was set
        data->time_management->time_table[note_pos]->press_time_set = 1;
        // It has not yet been released
        data->time_management->time_table[note_pos]->released = 0;
    }
}

// Do the key off properties
void key_off(ud *data, int note_pos)
{
    // It is no longer pressed
    data->time_management->time_table[note_pos]->press_time_set = 0;
    // It is now released
    data->time_management->time_table[note_pos]->released = 1;
    // We are enterring the release stage
    data->time_management->time_table[note_pos]->release_stage = 1;
    // The stop time is saved
    data->time_management->time_table[note_pos]->stop_time = data->time_management->actual_time;
    //printf("STOP time %f\n",data->time_management->time_table[note_pos]->stop_time );
}

// Based on the state change executes a function
void note_state_change(ud *data, int key_id)
{
    if (!(data->all_keys->keys[key_id]))
    {
        // KEY IS ON
        //printf("Key: %d is now on\n",key_id);
        key_on(data, key_id);
    }
    else
    {
        // KEY IS OFF
        //printf("Key: %d is now off\n",key_id);
        key_off(data, key_id);
    }
}

void print_keyboard_state(Uint8 *piano_keys)
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

void octave_set(Uint8 *state, ud *data)
{
    // Minus
    if (state[35] != data->side_effect[0])
    {
        if (state[35])
        {
            data->all_keys->octave = data->all_keys->octave / 2.0;
        }
    }
    // Plus
    if (state[46] != data->side_effect[1])

    {
        if (state[46])
        {
            data->all_keys->octave = data->all_keys->octave * 2.0;
        }
    }
}

void recording_set(Uint8 *state, ud *data)
{
    // Start
    if (state[40] != data->side_effect[2])
    {
        if (state[40])
        {
            data->wav_manager->record = 1;
        }
    }
    // Stop
    if (state[42] != data->side_effect[3])
    {
        if (state[42])
        {
            data->wav_manager->record = 0;
        }
    }
}

void playback_set(Uint8 *state, ud *data)
{
    // P
    if (state[19] != data->side_effect[4])
    {
        if (state[19])
        {
            data->wav_manager->playback = 1;
        }
    }
    // O
    if (state[18] != data->side_effect[5])
    {
        if (state[18])
        {
            data->wav_manager->playback = 0;
        }
    }
}

// Main state function that verifies and triggers a state change on each note
void note_state(Uint8 *state, ud *data)
{
    Uint8 *past_occ = data->all_keys->keys;
    
    for (int i = 0; i < 127; i++)
    {
       
        if (state[i] != past_occ[i])
        {
            //printf("State change on note %d\n",i);
            // STATE CHANGED
            note_state_change(data, i);
        }
    }
    /*
    octave_set(state, data);
    recording_set(state, data);
    playback_set(state, data);
    */
}