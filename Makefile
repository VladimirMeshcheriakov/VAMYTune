all:
	gcc -Wall -lm -O3 -g -fsanitize=address -o main -lSDL2 -lasound dynamic_array.c little_endian.c header_wav.c wav_interface.c time_stamp.c adsr.c keys.c userdata.c note_state_utils.c  basic_signals.c signals.c sdl_call_func.c main.c
