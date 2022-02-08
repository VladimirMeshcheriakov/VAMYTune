#include "keys.h"


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