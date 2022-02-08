#include <SDL2/SDL.h>

#ifndef KEYS_H
#define KEYS_H

typedef struct 
{
    Uint8 *keys;
    double *effects;
    size_t size;
}Keys;


Keys *init_keys(size_t size);
void free_keys(Keys *keys);
#endif