#include "header_wav.h"

#ifndef WAV_INTERFACE_H
#define WAV_INTERFACE_H

/*
Wav files management
*/
bool record(uint64_t sample, float * sound, const char * filename, const char *type);
long int findSize(const char *file_name);
FILE *open_WAV(const char *filename);
void read_from_wav(FILE* fout, const char *filename, float *sound);

#endif