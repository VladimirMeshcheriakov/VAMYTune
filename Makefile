# Makefile

CPPFLAGS = -MMD
CC = gcc
CFLAGS =  `pkg-config --cflags gtk+-3.0` -Wall -Wextra -O3 -g -fsanitize=address
LDFLAGS =
LDLIBS = -lm -lSDL2 -lasound `pkg-config --libs gtk+-3.0`

SRC = midi_interface_management/midi_management.c \
dynamic_array/dynamic_array.c \
little_endian/little_endian.c \
header_wav/header_wav.c \
wav_management/wav_management.c \
wav_interface/wav_interface.c \
time_stamp/time_stamp.c \
adsr/adsr.c \
keyboard_keys/keys.c \
userdata/userdata.c \
note_state_utils/note_state_utils.c \
basic_signals/basic_signals.c \
signals/signals.c \
visualiser/vis.c \
sdl_call_func/sdl_call_func.c \
audio_callback/audio_callback.c 
OBJ = ${SRC:.c=.o}
DEP = ${SRC:.c=.d}

all: main

main: midi_interface_management/midi_management.o dynamic_array/dynamic_array.o little_endian/little_endian.o header_wav/header_wav.o wav_management/wav_management.o wav_interface/wav_interface.o time_stamp/time_stamp.o adsr/adsr.o keyboard_keys/keys.o userdata/userdata.o note_state_utils/note_state_utils.o basic_signals/basic_signals.o signals/signals.o visualiser/vis.o sdl_call_func/sdl_call_func.o audio_callback/audio_callback.o

-include ${DEP}

.PHONY: clean

clean:
	${RM} ${OBJ}
	${RM} ${DEP}
	${RM} midi_interface_management/midi_management dynamic_array/dynamic_array little_endian/little_endian header_wav/header_wav wav_management/wav_management wav_interface/wav_interface time_stamp/time_stamp adsr/adsr keyboard_keys/keys userdata/userdata note_state_utils/note_state_utils basic_signals/basic_signals signals/signals visualiser/vis  sdl_call_func/sdl_call_func audio_callback/audio_callback main

# END