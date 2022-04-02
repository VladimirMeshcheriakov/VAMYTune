#include "sdl_call_func.h"

// Inits the sdl audio
int init_sdl_audio()
{
    if (SDL_Init(SDL_INIT_AUDIO) < 0)
    {
        fprintf(stderr, "Error initializing SDL. SDL_Error: %s\n",
                SDL_GetError());
        return -1;
    }
    return 0;
}

// Sets and an audio device, or sends  -1 if error
SDL_AudioDeviceID audio_spec_set_data(ud *data, void *audio_callback)
{
    SDL_AudioSpec audio_spec_want, audio_spec;

    SDL_memset(&audio_spec_want, 0, sizeof(audio_spec_want));

    audio_spec_want.freq = 44100;
    audio_spec_want.format = AUDIO_F32;
    audio_spec_want.channels = 2;
    audio_spec_want.samples = 1024;
    audio_spec_want.callback = audio_callback;
    audio_spec_want.userdata = data;

    SDL_AudioDeviceID audio_device_id = SDL_OpenAudioDevice(NULL, 0, &audio_spec_want, &audio_spec, SDL_AUDIO_ALLOW_FORMAT_CHANGE);
    if (!audio_device_id)
    {
        fprintf(stderr, "Error creating SDL audio device. SDL_Error: %s\n",
                SDL_GetError());
        SDL_Quit();
        return -1;
    }
    else
    {
        return audio_device_id;
    }
}


// Frees all the memory after the run_app
void stop_app(SDL_AudioDeviceID audio_device_id, ud *data)
{
    SDL_CloseAudioDevice(audio_device_id);
    free_user_data(data);
    SDL_Quit();
}

Uint8 *init_state_midi_keyboard(size_t size)
{
    Uint8 *state = calloc(size, sizeof(Uint8));
    return state;
}

// Main function that is called by the main
void init_run_app(ud *data, void *audio_callback, int argc, char *argv[])
{
    init_sdl_audio();
    SDL_AudioDeviceID audio_device_id = audio_spec_set_data(data, audio_callback);

    SDL_PauseAudioDevice(audio_device_id, 0);
    Uint8 *state = init_state_midi_keyboard(127);

    init_piano_keys(state, data);

    gtk_run_zbi(data,state, argc, argv);

    fclose(data->fout);
    if (data->wav_manager->recorded_samples)
    {
        record(data->wav_manager->recorded_samples, data->fstream->array, "Bach.wav", "wb");
    }

    free(state);
    stop_app(audio_device_id, data);
}