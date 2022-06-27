#include "../header_wav/header_wav.h"
#include "../userdata/userdata.h"
#ifndef WAV_INTERFACE_H
#define WAV_INTERFACE_H

#define BYTES_PER_SECOND 352800

/*
Wav files management
*/
bool record(uint64_t sample, float * sound, const char * filename, const char *type);
uint64_t findSize(const char *file_name);
FILE *open_WAV(const char *filename);
int read_from_wav(ud * data);
void read_from_sec(FILE * fout,uint64_t file_size, double sec);
void loop_between(const char * filename, double start, double end);
void set_new_working_wav_file(ud *data, const char *file_name);
void create(const char * file_name,const char *type);

#endif