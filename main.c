#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <SDL2/SDL.h>
#include "signals.h"

typedef struct
{
    uint64_t *samples_played;
    float freq;
} ud;


void audio_callback(void *userdata, uint8_t *stream, int len)
{
    ud *us_d = (ud *)userdata;
    uint64_t *samples_played = (uint64_t *)(us_d->samples_played);
    float *fstream = (float *)(stream);
    float frequency = us_d->freq;
    static const float volume = 1;
    for (int sid = 0; sid < (len / 8); ++sid)
    {
        double time = (*samples_played + sid) / 44100.0;
        float val = tans(volume,frequency,time);
        fstream[2 * sid + 0] = val; /* L */
        fstream[2 * sid + 1] = val; /* R */
    }
    *samples_played += (len / 8);
}



int main(int argc, char *argv[])
{
    uint64_t samples_played = 0;
    ud data;
    data.freq = 250.0;
    data.samples_played = &samples_played;
    if (SDL_Init(SDL_INIT_AUDIO) < 0)
    {
        fprintf(stderr, "Error initializing SDL. SDL_Error: %s\n", SDL_GetError());
        return -1;
    }

    SDL_AudioSpec audio_spec_want, audio_spec;
    SDL_memset(&audio_spec_want, 0, sizeof(audio_spec_want));

    audio_spec_want.freq = 44100;
    audio_spec_want.format = AUDIO_F32;
    audio_spec_want.channels = 2;
    audio_spec_want.samples = 1024;
    audio_spec_want.callback = audio_callback;
    audio_spec_want.userdata = &data;

    SDL_AudioDeviceID audio_device_id = SDL_OpenAudioDevice(
        NULL, 0,
        &audio_spec_want, &audio_spec,
        SDL_AUDIO_ALLOW_FORMAT_CHANGE);

    if (!audio_device_id)
    {
        fprintf(stderr, "Error creating SDL audio device. SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    int window_width = 600;
    int window_height = 600;
    SDL_Window *window;
    {
        window = SDL_CreateWindow(
            "SDL Tone Generator",
            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            window_width, window_height,
            SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

        if (!window)
        {
            fprintf(stderr, "Error creating SDL window. SDL_Error: %s\n", SDL_GetError());
            SDL_Quit();
            return -1;
        }
    }

    SDL_PauseAudioDevice(audio_device_id, 1);

    SDL_Event event;

    bool running = true;
    while (running)
    {
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_KEYDOWN:
                const char *c = SDL_GetKeyName(event.key.keysym.sym);
                float freq = note_to_freq(c[0] + 32);
                data.freq = freq;
                SDL_PauseAudioDevice(audio_device_id, 0);
                break;
            case SDL_KEYUP:
                SDL_PauseAudioDevice(audio_device_id, 1);
                break;
            case SDL_QUIT:
                SDL_PauseAudioDevice(audio_device_id, 1);
                running = false;
                break;

            default:
                break;
            }
        }
    }

    SDL_DestroyWindow(window);
    SDL_CloseAudioDevice(audio_device_id);
    SDL_Quit();

    return 0;
}