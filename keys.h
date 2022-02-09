#include <SDL2/SDL.h>

#ifndef KEYS_H
#define KEYS_H

/*
Keys structure:
keys -> is an array of keys-state, 1 for on, 0 for off
effects -> the table of effects, has 
all the amplitudes calculated dynamically for each note
size -> number of keys
*/
typedef struct 
{
    Uint8 *keys;
    double *effects;
    size_t size;
}Keys;


Keys *init_keys(size_t size);
void free_keys(Keys *keys);
#endif