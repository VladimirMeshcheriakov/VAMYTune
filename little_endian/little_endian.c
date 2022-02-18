#include "little_endian.h"

bool is_little_endian()
{
    volatile uint32_t i = 0x01234567;
    return (*((uint8_t *)(&i))) == 0x67;
}

uint16_t little_endian_uint16_t(uint16_t num)
{
    return (((0xff00 & num) >> 8) | ((0xff & num) << 8));
}

uint32_t little_endian_uint32_t(uint32_t num)
{
    return ((((0xff000000 & num) >> 24) |
             ((0xff & num) << 24) |
             ((0xff0000 & num) >> 8)) |
            ((0xff00 & num) << 8));
}

float little_endian_float(const float inFloat)
{
    float retVal;
    char *floatToConvert = (char *)&inFloat;
    char *returnFloat = (char *)&retVal;

    // swap the bytes into a temporary buffer
    returnFloat[0] = floatToConvert[3];
    returnFloat[1] = floatToConvert[2];
    returnFloat[2] = floatToConvert[1];
    returnFloat[3] = floatToConvert[0];

    return retVal;
}