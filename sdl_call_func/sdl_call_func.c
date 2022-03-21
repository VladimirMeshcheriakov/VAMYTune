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


// Updates the effect table
void update_effects(ud *data)
{
    // printf_time(data->time_management->time_table,1);
    for (size_t i = 0; i < 127; i++)
    {
        if (data->all_keys->keys[i] || data->time_management->time_table[i]->release_stage)
        {
            data->all_keys->effects[i] = adsr_get_amplitude(data->time_management->actual_time, data->adsr, data->time_management->time_table[i]);
            // printf("%f\n", data->all_keys->effects[i]);
        }
        else
        {
            data->all_keys->effects[i] = 0;
        }
    }
}



// Main loop of the app
void run_app(int running, ud *data,  Uint8 *state, int argc, char *argv[])
{
    data->fout = open_WAV("Bach.wav");
    data->fout_size = findSize("Bach.wav");
    struct pollfd *pfds;
    int npfds;
    int err;

    init_seq();

    if (parse_input(argc, argv) != -1)
    {
        printf("Parsing error\n");
    }

    snd_seq_t * seq = create_port();
    int port_count = connect_ports(seq);

    if (port_count > 0)
        printf("Waiting for data.");
    else
        printf("Waiting for data at port %d:0.",
               snd_seq_client_id(seq));
    printf(" Press Ctrl+C to end.\n");
    printf("Source  Event                  Ch  Data\n");

    signal(SIGINT, sighandler);
    signal(SIGTERM, sighandler);

    npfds = snd_seq_poll_descriptors_count(seq, POLLIN);
    pfds = alloca(sizeof(*pfds) * npfds);

    while (running)
    {
        snd_seq_poll_descriptors(seq, pfds, npfds, POLLIN);
        int p = poll(pfds, npfds, 20);
        if (p == 0)
        {
            update_effects(data);
            note_state(state, data);
            init_piano_keys(state, data);
        }
        else
        {
            if (p < 0)
            {
                break;
            }
            snd_seq_event_t *event_;
            err = snd_seq_event_input(seq, &event_);
            if (event_)
            {
                dump_event(event_, state);
                update_effects(data);
                note_state(state, data);
                init_piano_keys(state, data);
            }
        }
    }
}

// Frees all the memory after the run_app
void stop_app(SDL_Window *window, SDL_AudioDeviceID audio_device_id, ud *data)
{
    SDL_DestroyWindow(window);
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
    SDL_Window *window = sdl_window_set();
    Uint8 *state = init_state_midi_keyboard(127);

    init_piano_keys(state, data);

    int running = 1;
    run_app(running, data,  state, argc, argv);
    fclose(data->fout);
    if (data->wav_manager->recorded_samples)
    {
        record(data->wav_manager->recorded_samples, data->fstream->array, "Bach.wav", "wb");
    }
    free(state);
    stop_app(window, audio_device_id, data);
}