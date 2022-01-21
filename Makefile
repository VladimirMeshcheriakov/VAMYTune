all:
	gcc -Wall -g -fsanitize=address -lm -O3 -o main -lSDL2 -lasound signals.c adsr.c userdata.c  main.c