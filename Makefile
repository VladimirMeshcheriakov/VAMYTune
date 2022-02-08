all:
	gcc -Wall -lm -O3 -g -fsanitize=address -o main -lSDL2 -lasound signals.c time_stamp.c adsr.c userdata.c  main.c
