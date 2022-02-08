#include <SDL2/SDL.h>
#include <math.h>
#include <stdlib.h>
#include "signals.h"
#include "userdata.h"
#include "adsr.h"

Keys *init_keys(size_t size)
{
    Keys *keys = malloc(sizeof(Keys));
    keys->size = size;
    keys->effects = calloc(size, sizeof(double));
    keys->keys = calloc(size, sizeof(Uint8));
    return keys;
}

void free_keys(Keys *keys)
{
    free(keys->effects);
    free(keys->keys);
    free(keys);
}

float signal(float volume, double time, float freq)
{
    // Orgue
    // sine(volume, freq, time) + 0.5 * saw2(volume, freq*2.0, time,4)+ 0.2 *
    // triangle(volume, freq*4.0, time)  +  0.1 * square(volume, freq*5.0,
    // time,0.25);

    return sine(volume, freq, time) + 0.5 * saw2(volume, freq * 2.0, time, 4) + 0.2 * triangle(volume, freq * 4.0, time) + 0.1 * square(volume, freq * 5.0, time, 0.25);
}

float octave(float volume, double time, float freq)
{
    return signal(volume, time, freq * 0.5);
}

float signal_treat(float volume, double time, Keys *keys)
{
    float val = 0.0;

    for (int i = 0; i < 13; i++)
    {
        if (keys->keys[i])
        {
            val += keys->effects[i] * signal(volume, time, piano_note_to_freq(i));
        }
    }

    return val;
}

void audio_callback(void *userdata, uint8_t *stream, int len)
{
    ud *us_d = (ud *)userdata;
    uint64_t *samples_played = (us_d->samples_played);
    Keys *all_keys = us_d->all_keys;
    float *fstream = (float *)(stream);
    static const float volume = 0.2;
    for (int sid = 0; sid < (len / 8); ++sid)
    {
        all_keys = us_d->all_keys;
        double time = (*samples_played + sid) / 44100.0;
        // printf("Time %f\n",time);
        us_d->actual_time = time;
        float val = signal_treat(volume, time, all_keys);
        // printf("%f\n", val);
        fstream[2 * sid + 0] = val; /* L */
        fstream[2 * sid + 1] = val; /* R */
    }
    // printf("%d\n",len);

    *samples_played += (len / 8);
}

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
        // Sets the time, using the constant time management
        // of the audio-callback function
        data->time_table[note_pos]->press_time = data->actual_time;
        data->time_table[note_pos]->press_time_set = 1;
        data->time_table[note_pos]->released = 0;
    }
}

void key_off(ud *data, int note_pos)
{
    data->time_table[note_pos]->press_time_set = 0;
    // The key has been released
    data->time_table[note_pos]->released = 1;
    data->time_table[note_pos]->release_stage = 1;
    // We need to erase the press time so that the next note's
    // Press_time can be set
    // Save the release timestamp
    data->time_table[note_pos]->stop_time = data->actual_time;
    // Delay SDL to be able to play the release phase
    // SDL_Delay((data->adsr->release_time) * 1000);
}

void note_state_change(Uint8 *piano_keys, ud *data, int key_id)
{
    if (!piano_keys[key_id])
    {
        // KEY IS ON

        key_on(data, key_id);
        // printf_time(data->time_table, 13);
        /*
        printf("key_id %d: On\n",key_id);
        printf("time %f\n",data->actual_time);
        printf("Press_time: %f\n",data->time_table[key_id]->press_time);
        printf("Stop_time: %f\n",data->time_table[key_id]->stop_time);
        */
    }
    else
    {
        // KEY IS OFF
        key_off(data, key_id);
        // printf_time(data->time_table, 13);
        /*
        printf("key_id %d: Off\n",key_id);
        printf("Note info\n");
        printf("time %f\n",data->actual_time);
        printf("Press_time: %f\n",data->time_table[key_id]->press_time);
        printf("Stop_time: %f\n",data->time_table[key_id]->stop_time);
        */
    }
}

void note_state(const Uint8 *state, Uint8 *past_occ, ud *data)
{
    if (state[22] != past_occ[0])
    {
        // STATE CHANGED
        note_state_change(past_occ, data, 0);
    }
    if (state[8] != past_occ[1])
    {
        // STATE CHANGED
        note_state_change(past_occ, data, 1);
    }
    if (state[7] != past_occ[2])
    {
        // STATE CHANGED
        note_state_change(past_occ, data, 2);
    }
    if (state[21] != past_occ[3])
    {
        // STATE CHANGED
        note_state_change(past_occ, data, 3);
    }
    if (state[9] != past_occ[4])
    {
        // STATE CHANGED
        note_state_change(past_occ, data, 4);
    }
    if (state[10] != past_occ[5])
    {
        // STATE CHANGED
        note_state_change(past_occ, data, 5);
    }
    if (state[28] != past_occ[6])
    {
        // STATE CHANGED
        note_state_change(past_occ, data, 6);
    }
    if (state[11] != past_occ[7])
    {
        // STATE CHANGED
        note_state_change(past_occ, data, 7);
    }
    if (state[24] != past_occ[8])
    {
        // STATE CHANGED
        note_state_change(past_occ, data, 8);
    }
    if (state[13] != past_occ[9])
    {
        // STATE CHANGED
        note_state_change(past_occ, data, 9);
    }
    if (state[12] != past_occ[10])
    {
        // STATE CHANGED
        note_state_change(past_occ, data, 10);
    }
    if (state[14] != past_occ[11])
    {
        // STATE CHANGED
        note_state_change(past_occ, data, 11);
    }
    if (state[15] != past_occ[12])
    {
        // STATE CHANGED
        note_state_change(past_occ, data, 12);
    }
    // update all the effect values for each note
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

void update_effects(ud *data)
{

    for (size_t i = 0; i < 13; i++)
    {

        if (data->all_keys->keys[i])
        {
            data->all_keys->effects[i] = adsr_get_amplitude(data->actual_time, data->adsr, data->time_table[i]);
        }
        else
        {
            data->all_keys->effects[i] = 0;
        }
        // printf(" i: %ld, %f | ",i,effects[i]);
    }
    // printf("\n");
}

int main(int argc, char *argv[])
{
    // Init the ADSR envelope
    ADSR *env = init_ADSR_envelope(0.5, 1.5, 1.5, 0.0, 1.0, 0.4, 0.0);
    TimeStamp **table_time = init_time_table(13);
    // Datat to pass to the audio_callback function
    uint64_t samples_played = 0.;

    ud *data = init_ud(&samples_played, env, table_time);

    if (SDL_Init(SDL_INIT_AUDIO) < 0)
    {
        fprintf(stderr, "Error initializing SDL. SDL_Error: %s\n",
                SDL_GetError());
        return -1;
    }

    SDL_AudioSpec audio_spec_want, audio_spec;
    SDL_memset(&audio_spec_want, 0, sizeof(audio_spec_want));

    audio_spec_want.freq = 44100;
    audio_spec_want.format = AUDIO_F32;
    audio_spec_want.channels = 2;
    audio_spec_want.samples = 1024;
    audio_spec_want.callback = audio_callback;
    audio_spec_want.userdata = data;

    const SDL_AudioDeviceID audio_device_id = SDL_OpenAudioDevice(
        NULL, 0, &audio_spec_want, &audio_spec, SDL_AUDIO_ALLOW_FORMAT_CHANGE);

    if (!audio_device_id)
    {
        fprintf(stderr, "Error creating SDL audio device. SDL_Error: %s\n",
                SDL_GetError());
        SDL_Quit();
        return -1;
    }

    int window_width = 600;
    int window_height = 600;
    SDL_Window *window;
    {
        window = SDL_CreateWindow("SDL Tone Generator", SDL_WINDOWPOS_UNDEFINED,
                                  SDL_WINDOWPOS_UNDEFINED, window_width,
                                  window_height,
                                  SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

        if (!window)
        {
            fprintf(stderr, "Error creating SDL window. SDL_Error: %s\n",
                    SDL_GetError());
            SDL_Quit();
            return -1;
        }
    }

    SDL_PauseAudioDevice(audio_device_id, 0);

    SDL_Event event;
    Keys *all_keys = init_keys(13);
    data->all_keys = all_keys;
    int *numkeys = malloc(sizeof(int));
    const Uint8 *state = SDL_GetKeyboardState(numkeys);
    init_piano_keys(state, all_keys->keys);
    data->all_freq = all_keys->keys;
    int running = 1;
    while (running)
    {

        // print_keyboard_state(state);

        // note_state(state, piano_keys, env, audio_device_id, data);
        update_effects(data);
        note_state(state, all_keys->keys, data);
        init_piano_keys(state, all_keys->keys);

        // print_keyboard_state(all_keys->keys);

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                running = 0;
            }
        }
    }

    SDL_DestroyWindow(window);
    SDL_CloseAudioDevice(audio_device_id);
    SDL_Quit();
    free_time_table(table_time, 13);
    free_keys(all_keys);
    free(numkeys);
    free(env);
    free(data);
    return 0;
}
