#include <SDL2/SDL.h>
#include <math.h>
#include <stdlib.h>

#include "signals.h"
#include "userdata.h"

float signal(float volume, double time, float freq) {
    // Orgue
    // sine(volume, freq, time) + 0.5 * saw2(volume, freq*2.0, time,4)+ 0.2 *
    // triangle(volume, freq*4.0, time)  +  0.1 * square(volume, freq*5.0,
    // time,0.25);
    return sine(volume, freq, time) + 0.5 * saw2(volume, freq * 2.0, time, 4) +
           0.2 * triangle(volume, freq * 4.0, time) +
           0.1 * square(volume, freq * 5.0, time, 0.25);
}

float signal_treat(float volume, double time, Uint8 *keys) {
    float val = 0.0;

    for (int i = 0; i < 13; i++) {
        if (keys[i]) {
            val += signal(volume, time, piano_note_to_freq(i));
        }
    }

    return val;
}

void audio_callback(void *userdata, uint8_t *stream, int len) {
    ud *us_d = (ud *)userdata;
    uint64_t *samples_played = (uint64_t *)(us_d->samples_played);
    ADSR *adsr = us_d->adsr;
    Uint8 *keys = us_d->all_freq;
    float *fstream = (float *)(stream);
    static const float volume = 0.2;
    for (int sid = 0; sid < (len / 8); ++sid) {
        keys = us_d->all_freq;
        double time = (*samples_played + sid) / 44100.0;
        // printf("Time %f\n",time);
        us_d->actual_time = time;

        float val = signal_treat(volume, time, keys);
        // printf("%f\n", val);
        fstream[2 * sid + 0] = val; /* L */
        fstream[2 * sid + 1] = val; /* R */
    }
    *samples_played += (len / 8);
}
/*
void key_on(SDL_AudioDeviceID audio_device_id, ADSR *env, ud *data, char note) {
    if (!(env->press_time_set)) {
        // Sets the time, using the constant time management
        // of the audio-callback function
        env->press_time = data->actual_time;
    }
    env->press_time_set = 1;
    // If it is pressed it is no longer released
    env->released = 0;
    // Convert it to a frequency value in Hertz
    float freq = note_to_freq(note);
    // modify the frequency
    data->freq = freq;
    // Data is set so start to play
    SDL_PauseAudioDevice(audio_device_id, 0);
}

void key_off(SDL_AudioDeviceID audio_device_id, ADSR *env, ud *data) {
    // The key has been released
    env->released = 1;
    // We need to erase the press time so that the next note's
    // Press_time can be set
    env->press_time_set = 0;
    // Save the release timestamp
    env->stop_time = data->actual_time;
    // Delay SDL to be able to play the release phase
    SDL_Delay((env->release_time) * 1000);
    // data->freq = 0.0;
    //  Stop the  audio
    // SDL_PauseAudioDevice(audio_device_id, 1);
}

void note_change_state(const Uint8 *piano_keys, ADSR *env, Uint8 *past_occ,
                       SDL_AudioDeviceID audio_device_id, ud *data, int key_id,
                       char note) {
    // STATE CHANGED
    if (piano_keys[key_id]) {
        // KEY IS ON
        key_on(audio_device_id, env, data, note);

    } else {
        // KEY IS OFF
        key_off(audio_device_id, env, data);
    }
}

void note_state(const Uint8 *piano_keys, Uint8 *past_occ, ADSR *env,
                SDL_AudioDeviceID audio_device_id, ud *data) {
    if (piano_keys[22] != past_occ[0]) {
        // STATE CHANGED
        note_change_state(piano_keys, env, past_occ, audio_device_id, data, 22,
                          's');
    }
    if (piano_keys[8] != past_occ[1]) {
        // STATE CHANGED
        note_change_state(piano_keys, env, past_occ, audio_device_id, data, 8,
                          'e');
    }
    if (piano_keys[7] != past_occ[2]) {
        // STATE CHANGED
        note_change_state(piano_keys, env, past_occ, audio_device_id, data, 7,
                          'd');
    }
    if (piano_keys[21] != past_occ[3]) {
        // STATE CHANGED
        note_change_state(piano_keys, env, past_occ, audio_device_id, data, 21,
                          'r');
    }
    if (piano_keys[9] != past_occ[4]) {
        // STATE CHANGED
        note_change_state(piano_keys, env, past_occ, audio_device_id, data, 9,
                          'f');
    }
    if (piano_keys[10] != past_occ[5]) {
        // STATE CHANGED
        note_change_state(piano_keys, env, past_occ, audio_device_id, data, 10,
                          'g');
    }
    if (piano_keys[28] != past_occ[6]) {
        // STATE CHANGED
        note_change_state(piano_keys, env, past_occ, audio_device_id, data, 28,
                          'y');
    }
    if (piano_keys[11] != past_occ[7]) {
        // STATE CHANGED
        note_change_state(piano_keys, env, past_occ, audio_device_id, data, 11,
                          'h');
    }
    if (piano_keys[24] != past_occ[8]) {
        // STATE CHANGED
        note_change_state(piano_keys, env, past_occ, audio_device_id, data, 24,
                          'u');
    }
    if (piano_keys[13] != past_occ[9]) {
        // STATE CHANGED
        note_change_state(piano_keys, env, past_occ, audio_device_id, data, 13,
                          'j');
    }
    if (piano_keys[12] != past_occ[10]) {
        // STATE CHANGED
        note_change_state(piano_keys, env, past_occ, audio_device_id, data, 12,
                          'i');
    }
    if (piano_keys[14] != past_occ[11]) {
        // STATE CHANGED
        note_change_state(piano_keys, env, past_occ, audio_device_id, data, 14,
                          'k');
    }
    if (piano_keys[15] != past_occ[12]) {
        // STATE CHANGED
        note_change_state(piano_keys, env, past_occ, audio_device_id, data, 15,
                          'l');
    }
    // update all the effect values for each note
}
*/
void init_piano_keys(const Uint8 *state, Uint8 *piano_keys) {
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
/*
void print_keyboard_state(const Uint8 *piano_keys) {
    printf(" _______________________________________________________\n");
    printf("|  |     |  |     |   |   |     |  |     |  |     |  |  |     \n");
    printf("|  |     |  |     |   |   |     |  |     |  |     |  |  |     \n");
    printf(
        "|  |  %d  |  |  %d  |   |   |  %d  |  |  %d  |  |  %d  |  |  |    "
        " \n",
        piano_keys[8], piano_keys[21], piano_keys[28], piano_keys[24],
        piano_keys[12]);
    printf("|  |     |  |     |   |   |     |  |     |  |     |  |  |     \n");
    printf("|  |_____|  |_____|   |   |_____|  |_____|  |_____|  |  |__   \n");
    printf("|     |        |      |      |        |        |     |     |  \n");
    printf("|     |        |      |      |        |        |     |     |  \n");
    printf(
        "|  %d  |   %d    |  %d   |  %d   |   %d    |   %d    |  %d  |  %d "
        " |  \n",
        piano_keys[22], piano_keys[7], piano_keys[9], piano_keys[10],
        piano_keys[11], piano_keys[13], piano_keys[14], piano_keys[15]);
    printf("|     |        |      |      |        |        |     |     |  \n");
    printf("|_____|________|______|______|________|________|_____|_____|  \n");
}

void update_effects(float *effects, ud *data, ADSR *env,
                    TimeStamp **time_data) {
    for (size_t i = 0; i < 1; i++) {
        effects[i] = adsr_get_amplitude(data->actual_time, env, time_data[i]);
        // printf(" i: %ld, %f | ",i,effects[i]);
    }
    // printf("\n");
}
*/
int main(int argc, char *argv[]) {
    // Init the ADSR envelope
    ADSR *env = init_ADSR_envelope(0.05, 0.05, 0.05, 0.0, 1.0, 0.4, 0.0);
    // Datat to pass to the audio_callback function
    uint64_t samples_played = 0;

    ud *data = init_ud(samples_played, env);

    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
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

    SDL_AudioDeviceID audio_device_id = SDL_OpenAudioDevice(
        NULL, 0, &audio_spec_want, &audio_spec, SDL_AUDIO_ALLOW_FORMAT_CHANGE);

    if (!audio_device_id) {
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

        if (!window) {
            fprintf(stderr, "Error creating SDL window. SDL_Error: %s\n",
                    SDL_GetError());
            SDL_Quit();
            return -1;
        }
    }

    SDL_PauseAudioDevice(audio_device_id, 0);

    TimeStamp **table_time = init_time_table(13);
    SDL_Event event;
    int *numkeys = malloc(sizeof(int));
    const Uint8 *state = SDL_GetKeyboardState(numkeys);
    Uint8 *piano_keys = malloc(sizeof(Uint8) * 13);
    init_piano_keys(state, piano_keys);
    data->all_freq = piano_keys;
    int running = 1;
    float *effects = malloc(13 * sizeof(float));
    while (running) {
        while (SDL_PollEvent(&event)) {
            // print_keyboard_state(state);

            // note_state(state, piano_keys, env, audio_device_id, data);
            //  update_effects(effects,data,env,table_time);
            init_piano_keys(state, piano_keys);
            if (event.type == SDL_QUIT) {
                running = 0;
            }

            /*
            switch (event.type)
            {
            case SDL_KEYDOWN:
                SDL_Keycode note = event.key.keysym.sym;
                const char *c = SDL_GetKeyName(note);
                key_on(audio_device_id, env, data, c[0]+32);
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
            */
        }
    }

    SDL_DestroyWindow(window);
    SDL_CloseAudioDevice(audio_device_id);
    SDL_Quit();
    free_time_table(table_time, 13);
    free(effects);

    free(numkeys);
    free(piano_keys);
    free(env);
    free(data);
    return 0;
}
