#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288
#endif

#ifndef COMPLEX_NUMBERS_H
#define COMPLEX_NUMBERS_H

typedef struct
{
    float real;
    float imaginary;
} complex_number;

float complex_coef(float n, float k, float N);
complex_number *table_complex_init(size_t size);
complex_number complex_exp(float num);
complex_number complex_plus(complex_number num1, complex_number num2);
complex_number complex_minus(complex_number num1, complex_number num2);
complex_number complex_times(complex_number num1, complex_number num2);
complex_number complex_time_natural(complex_number num, float number);
complex_number inverse_complex_real_with_imaginary(complex_number num1);
complex_number complex_divide_natural(complex_number num, float number);
void inverse_complex_array(complex_number * num, size_t size);
void divide_complex_array(complex_number * num, size_t size, float number);
void printf_complex(complex_number val);
void print_arr_img(complex_number *arr, size_t size, size_t in_line);
#endif
