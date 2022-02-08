#include <SDL2/SDL.h>
#include <math.h>
#include <stdlib.h>
#include "signals.h"
#include "userdata.h"
#include "adsr.h"
#include "note_state_utils.h"

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
        us_d->actual_time = time;
        float val = signal_treat(volume, time, all_keys,us_d);
        fstream[2 * sid + 0] = val; /* L */
        fstream[2 * sid + 1] = val; /* R */
    }
    *samples_played += (len / 8);
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
        if (data->all_keys->keys[i] || data->time_table[i]->release_stage)
        {
            data->all_keys->effects[i] = adsr_get_amplitude(data->actual_time, data->adsr, data->time_table[i]);
        }
        else
        {
            data->all_keys->effects[i] = 0; 
        }
    }
}

int main(int argc, char *argv[])
{
    // Init the ADSR envelope
    ADSR *env = init_ADSR_envelope(0.1, 0.1, 0.1, 0.0, 1.0, 0.3, 0.0);
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
        note_state(state, data);
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
