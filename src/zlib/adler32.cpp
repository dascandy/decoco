/* adler32.cpp -- compute the Adler-32 checksum of a data stream
 * Copyright (C) 1995-2011, 2016 Mark Adler
 * For conditions of distribution and use, see copyright notice in zlib.h
 */

#include "zutil.h"

namespace Zlib {

constexpr size_t BASE = 65521U;     /* largest prime smaller than 65536 */
constexpr size_t NMAX = 5552; /* NMAX is the largest n such that 255n(n+1)/2 + (n+1)(BASE-1) <= 2^32-1 */

uint32_t adler32(uint32_t adler, const uint8_t* buf, size_t len)
{
    if (buf == nullptr)
        return 1;

    /* split Adler-32 into component sums */
    uint32_t sum2 = (adler >> 16) & 0xffff;
    adler &= 0xffff;

    /* Nested loops to avoid modulos when not necessary */
    while (len > 0) {
        uint16_t n;
        if (len < NMAX)
          n = (uint16_t)len;
        else 
          n = NMAX;
        len -= n;
        do {
            adler += *buf++; sum2 += adler;
        } while (--n);
        adler %= BASE;
        sum2 %= BASE;
    }

    /* return recombined sums */
    return adler | (sum2 << 16);
}

}


