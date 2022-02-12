#include <stdlib.h>
#include <stdio.h>

#ifndef DYNAMIC_ARRAY_H
#define DYNAMIC_ARRAY_H
/*
Dynamic array structure
*/
typedef struct {
  float *array;
  size_t used;
  size_t size;
} Array;

Array * initArray(size_t initialSize);
void insertArray(Array *a, float element);
void freeArray(Array *a);
void print_array(Array *a);

#endif