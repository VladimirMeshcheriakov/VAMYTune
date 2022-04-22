#include "../header_wav/header_wav.h"

#ifndef WAV_INTERFACE_H
#define WAV_INTERFACE_H

#define BYTES_PER_SECOND 352800

/*
Wav files management
*/
bool record(uint64_t sample, float * sound, const char * filename, const char *type);
uint64_t findSize(const char *file_name);
FILE *open_WAV(const char *filename);
void read_from_wav(FILE* fout,float *sound);
void read_from_sec(FILE * fout,uint64_t file_size, double sec);
void loop_between(const char * filename, double start, double end, float * sound );

#endif