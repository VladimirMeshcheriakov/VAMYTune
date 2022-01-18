all:
	gcc -Wall -g -fsanitize=address -lm -O3 -o main -lSDL2 signals.c main.c