all:
	gcc -Wall -lm -O3 -g -fsanitize=address -o main -lSDL2 -lasound time_stamp.c adsr.c userdata.c note_state_utils.c keys.c basic_signals.c signals.c main.c
