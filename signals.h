#include <stdlib.h>

#ifndef SIGNALS_H
#define SIGNALS_H
float logsin(float volume, float frequency, double time);
float logcos(float volume, float frequency, double time);
float sine(float volume, float frequency, double time);
float tans(float volume, float frequency, double time);
float ttans(float volume, float frequency, double time);
float square(float volume, float frequency, double time, double shape);
float triangle(float volume, float frequency, double time);
float saw(float volume, float frequency, double time);
float saw2(float volume, float frequency, double time, int limit);
float note_to_freq(char c);
char sdlk_to_freq(int c);
int sdlk_to_piano_keys(int c);
float piano_note_to_freq(int c);
#endif
