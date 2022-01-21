#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <SDL2/SDL.h>
#include "signals.h"
#include "userdata.h"

void audio_callback(void *userdata, uint8_t *stream, int len)
{
    ud *us_d = (ud *)userdata;
    uint64_t *samples_played = (uint64_t *)(us_d->samples_played);
    ADSR *adsr = us_d->adsr;
    float *fstream = (float *)(stream);
    float frequency = us_d->freq;
    static const float volume = 1;
    for (int sid = 0; sid < (len / 8); ++sid)
    {
        double time = (*samples_played + sid) / 44100.0;
        us_d->actual_time = time;
        // printf("Time %f\n",us_d->actual_time);
        float val = ( sine(volume, frequency, time)) * adsr_get_amplitude(time, adsr);
        fstream[2 * sid + 0] = val; /* L */
        fstream[2 * sid + 1] = val; /* R */
    }
    *samples_played += (len / 8);
}

void key_on(SDL_AudioDeviceID audio_device_id,ADSR *env, ud *data,SDL_Keycode note)
{
    if (!(env->press_time_set))
    {
        // Sets the time, using the constant time management
        // of the audio-callback function
        env->press_time = data->actual_time;
    }
    env->press_time_set = true;
    // If it is pressed it is no longer released
    env->released = false;

    const char *c = SDL_GetKeyName(note);
    // Convert it to a frequency value in Hertz
    float freq = note_to_freq(c[0] + 32);
    // modify the frequency
    data->freq = freq;

    // Data is set so start to play
    SDL_PauseAudioDevice(audio_device_id, 0);
}

void key_off(SDL_AudioDeviceID audio_device_id, ADSR *env, ud *data)
{
    // The key has been released
    env->released = true;
    // We need to erase the press time so that the next note's
    // Press_time can be set
    env->press_time_set = false;
    // Save the release timestamp
    env->stop_time = data->actual_time;
    // Delay SDL to be able to play the release phase
    SDL_Delay((env->release_time) * 1000);
    // Stop the  audio
    SDL_PauseAudioDevice(audio_device_id, 1);
}




int main(int argc, char *argv[])
{
    // Init the ADSR envelope
    ADSR *env = init_ADSR_envelope(0.2, 0.2, 0.2, 0.0, 1.0, 0.4, 0.0);
    // Datat to pass to the audio_callback function
    uint64_t samples_played = 0;

    ud *data = init_ud(samples_played,env);

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
    audio_spec_want.userdata = data;

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
                SDL_Keycode note = event.key.keysym.sym;
                key_on(audio_device_id,env,data,note);
                break;
            case SDL_KEYUP:
                key_off(audio_device_id, env, data);
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
    free(env);
    free(data);
    return 0;
}