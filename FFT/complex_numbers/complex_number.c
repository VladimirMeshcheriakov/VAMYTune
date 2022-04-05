#include "complex_number.h"

// Frequency bin
float complex_coef(float n, float k, float N)
{
    return (2.0 * M_PI * n * k) / N;
}


complex_number *table_complex_init(size_t size)
{
    complex_number *table = malloc(sizeof(complex_number) * size);
    for (size_t i = 0; i < size; i++)
    {
        complex_number number;
        number.imaginary = number.real = 0.0;
        table[i] = number;
    }
    return table;
}


complex_number complex_exp(float num)
{
    complex_number number;
    number.imaginary = sin(num);
    number.real = cos(num);
    return number;
}

complex_number  complex_plus(complex_number num1, complex_number num2)
{
    complex_number num;
    num.imaginary = num1.imaginary + num2.imaginary;
    num.real = num1.real + num2.real;
    return num;
}

complex_number  complex_minus(complex_number num1, complex_number num2)
{
    complex_number num;
    num.imaginary = num1.imaginary - num2.imaginary;
    num.real = num1.real - num2.real;
    return num;
}



complex_number inverse_complex_real_with_imaginary(complex_number num1)
{
    complex_number num;
    num.real = num1.imaginary;
    num.imaginary = num1.real;
    return num;
}

complex_number complex_times(complex_number num1, complex_number num2)
{
    complex_number num;
    num.real = (num1.real*num2.real) - (num1.imaginary * num2.imaginary);
    num.imaginary = (num1.real * num2.imaginary) + (num2.real * num1.imaginary);
    return num;
}

complex_number complex_time_natural(complex_number num, float number)
{
    num.imaginary *= number;
    num.real *= number;
    return num;
}

complex_number complex_divide_natural(complex_number num, float number)
{
    num.real /= number;
    return num;
}

void inverse_complex_array(complex_number * num, size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        num[i] = inverse_complex_real_with_imaginary(num[i]);
    }
}



void divide_complex_array(complex_number * num, size_t size, float number)
{
    for (size_t i = 0; i < size; i++)
    {
        num[i] = complex_divide_natural(num[i],number);
    }
}

void printf_complex(complex_number val)
{
    printf("| (real: %f,imaginary: %f) |", val.real, val.imaginary);
}


void print_arr_img(complex_number *arr, size_t size, size_t in_line)
{
    size_t mem = in_line;
    for (size_t i = 0; i < size; i++)
    {
        complex_number val = arr[i];

        if (!in_line)
        {
            printf("\n");
            in_line = mem;
        }

        printf_complex(val);

        in_line--;
    }
    printf("\n");
}

