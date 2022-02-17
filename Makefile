# Makefile

CPPFLAGS = -MMD
CC = gcc
CFLAGS = -Wall -Wextra -O3 -g -fsanitize=address
LDFLAGS =
LDLIBS = -lm -lSDL2 -lasound

SRC = dynamic_array.c little_endian.c header_wav.c wav_management.c wav_interface.c time_stamp.c adsr.c keys.c userdata.c note_state_utils.c  basic_signals.c signals.c sdl_call_func.c main.c
OBJ = ${SRC:.c=.o}
DEP = ${SRC:.c=.d}

all: main

main: dynamic_array.o little_endian.o header_wav.o wav_management.o wav_interface.o time_stamp.o adsr.o keys.o userdata.o note_state_utils.o  basic_signals.o signals.o sdl_call_func.o 

-include ${DEP}

.PHONY: clean

clean:
	${RM} ${OBJ}
	${RM} ${DEP}
	${RM} dynamic_array little_endian header_wav wav_management wav_interface time_stamp adsr keys userdata note_state_utils basic_signals signals sdl_call_func main

# END