#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#ifndef WAV_INTERFACE_H
#define WAV_INTERFACE_H
/*
Wav files management
*/
void record(uint64_t sample, float * sound, const char * filename);
#endif