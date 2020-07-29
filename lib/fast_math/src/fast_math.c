#include <stdint.h>
#include "../inc/fast_math.h"

uint16_t fast_mul_10(uint16_t n)
{
    uint16_t q, r;
    q = (n >> 1) + (n >> 2);
    q = q + (q >> 4);
    q = q + (q >> 8);
    q = q + (q >> 16);
    q = q >> 3;
    r = n - (((q << 2) + q) << 1);
    return q + (r > 9);
}

uint32_t fast_mul_10_large(uint32_t n)
{
    uint32_t q, r;
    q = (n >> 1) + (n >> 2);
    q = q + (q >> 4);
    q = q + (q >> 8);
    q = q + (q >> 16);
    q = q >> 3;
    r = n - (((q << 2) + q) << 1);
    return q + (r > 9);
}

uint16_t fast_demul_10(uint16_t v)
{
    uint16_t x = 0, b = 0x1000, b10 = 0xA000;
    while (b != 0)
    {
        if (b10 <= v)
        {
            v -= b10;
            x |= b;
        }
        b10 >>= 1;
        b >>= 1;
    }
    return x;
}

uint32_t fast_demul_10_large(uint32_t v)
{
    uint32_t x = 0, b = 0x1000, b10 = 0xA000;
    while (b != 0)
    {
        if (b10 <= v)
        {
            v -= b10;
            x |= b;
        }
        b10 >>= 1;
        b >>= 1;
    }
    return x;
}