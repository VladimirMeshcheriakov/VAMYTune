#include "keys.h"

//Init the key structure, size is the number of keys
Keys *init_keys(size_t size)
{
    Keys *keys = malloc(sizeof(Keys));
    keys->size = size;
    keys->effects = calloc(size, sizeof(double));
    keys->keys = calloc(size, sizeof(Uint8));
    keys->octave = 1.0;
    return keys;
}
//Free the keys structure
void free_keys(Keys *keys)
{
    free(keys->effects);
    free(keys->keys);
    free(keys);
}