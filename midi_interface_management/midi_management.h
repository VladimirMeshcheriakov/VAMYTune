#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <signal.h>
#include <getopt.h>
#include <sys/poll.h>
#include <alsa/asoundlib.h>
#include <SDL2/SDL.h>

#ifndef MIDI_MANAGEMENT_H
#define MIDI_MANAGEMENT_H


void fatal(const char *msg, ...);
void check_mem(void *p);
void check_snd(const char *operation, int err);
void init_seq(void);
void parse_ports(const char *arg);
snd_seq_t *  create_port(void);
int connect_ports(snd_seq_t * seq1);
void dump_event(const snd_seq_event_t *ev, Uint8 *keyboard);
void list_ports(void);
void help(const char *argv0);
void sighandler(int sig);
int parse_input(int argc, char *argv[]);

#endif