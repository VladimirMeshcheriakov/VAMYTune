#include "dynamic_array.h"

Array *initArray(size_t initialSize)
{
  Array *a = malloc(sizeof(Array));
  a->array = malloc(initialSize * sizeof(float));
  a->used = 0;
  a->size = initialSize;
  return a;
}

void insertArray(Array *a, float element)
{
  // a->used is the number of used entries, because a->array[a->used++] updates a->used only *after* the array has been accessed.
  // Therefore a->used can go up to a->size
  if (a->used == a->size)
  {
    a->size *= 2;
    a->array = realloc(a->array, a->size * sizeof(float));
  }
  a->array[a->used++] = element;
}

void freeArray(Array *a)
{
  free(a->array);
  a->array = NULL;
  a->used = a->size = 0;
  free(a);
}

void print_array(Array *a)
{
  for (size_t i = 0; i < a->used; i++)
  {
    if (a->array[i] != 0.0)
    {
      printf("At index: %ld, value:%f", i, a->array[i]);
    }
  }
}