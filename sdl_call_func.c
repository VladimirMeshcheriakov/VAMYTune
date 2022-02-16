#include "sdl_call_func.h"

// Updates the effect table
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

// Sets up a window on sdl
SDL_Window *sdl_window_set()
{
    SDL_Window *window;
    int window_width = 600;
    int window_height = 600;
    window = SDL_CreateWindow("SDL Tone Generator", SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED, window_width,
                              window_height,
                              SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

    if (!window)
    {
        fprintf(stderr, "Error creating SDL window. SDL_Error: %s\n",
                SDL_GetError());
        SDL_Quit();
        return NULL;
    }
    return window;
}

// Main loop of the app
void run_app(int running, ud *data, SDL_Event event, const Uint8 *state)
{
    data->fout = open_WAV("Bach.wav");
    data->fout_size = findSize("Bach.wav");
    while (running)
    {
        update_effects(data);
        note_state(state, data);
        init_piano_keys(state, data);

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                running = 0;
            }
        }
    }
}

// Frees all the memory after the run_app
void stop_app(size_t num_keys, SDL_Window *window, SDL_AudioDeviceID audio_device_id, ud *data)
{
    SDL_DestroyWindow(window);
    SDL_CloseAudioDevice(audio_device_id);
    free_user_data(data, num_keys);
    SDL_Quit();
}

// Main function that is called by the main
void init_run_app(ud *data, size_t num_keys, void *audio_callback)
{
    init_sdl_audio();
    SDL_AudioDeviceID audio_device_id = audio_spec_set_data(data, audio_callback);

    SDL_PauseAudioDevice(audio_device_id, 0);
    SDL_Window *window = sdl_window_set();
    SDL_Event event;
    const Uint8 *state = SDL_GetKeyboardState(NULL);

    init_piano_keys(state, data);
    int running = 1;
    run_app(running, data, event, state);
    fclose(data->fout);
    if(data->recorded_samples)
    {
        record((data->recorded_samples), data->fstream->array, "Bach.wav","wb");
    }
    
    stop_app(num_keys, window, audio_device_id, data);
}