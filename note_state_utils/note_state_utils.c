#include "note_state_utils.h"

// Do the key on properties
void key_on(ud *data, int note_id)
{
    // if this key has been pressed already, it does not have to be re-set
    if (!(data->time_management->time_table[note_id]->press_time_set))
    {
        // Actual time set
        data->time_management->time_table[note_id]->press_time = data->time_management->actual_time;
        printf("Press time %f\n", data->time_management->time_table[note_id]->press_time);
        // Press was set
        data->time_management->time_table[note_id]->press_time_set = 1;
        // It has not yet been released
        data->time_management->time_table[note_id]->released = 0;
        // It has not yet been prematurely released
        data->time_management->time_table[note_id]->premature_release = 0;
    }
}

// Do the key off properties
void key_off(ud *data, int note_id)
{
    if ((data->time_management->time_table[note_id]->press_time_set))
    {
        // It is no longer pressed
        data->time_management->time_table[note_id]->press_time_set = 0;
        // It is now released
        data->time_management->time_table[note_id]->released = 1;
        // We are enterring the release stage
        data->time_management->time_table[note_id]->release_stage = 1;
        // The stop time is saved
        data->time_management->time_table[note_id]->stop_time = data->time_management->actual_time;
        // The total time of the AD phase
        double ad_phase = data->adsr->attack_to_decay_time + data->adsr->decay_to_sustain_time;
        // The time the signal actually took
        double sig_time = data->time_management->time_table[note_id]->stop_time - data->time_management->time_table[note_id]->press_time;
        // If the signal time is shorter than the AD phase, it is a premature release
        if (sig_time < ad_phase)
        {
            // Set the premature release
            data->time_management->time_table[note_id]->premature_release = 1;
            // Se the amplitude to the current amplitude
            data->time_management->time_table[note_id]->release_amp = data->all_keys->effects[note_id];
        }
        printf("STOP time %f\n", data->time_management->time_table[note_id]->stop_time);
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
