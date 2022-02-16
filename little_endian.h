#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#ifndef LITTLE_ENDIAN_H
#define LITTLE_ENDIAN_H
bool is_little_endian();
uint16_t little_endian_uint16_t(uint16_t num);
uint32_t little_endian_uint32_t(uint32_t num);
float little_endian_float(const float inFloat);

#endif