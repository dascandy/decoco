/* crc32.c -- compute the CRC-32 of a data stream
 * Copyright (C) 1995-2006, 2010, 2011, 2012, 2016 Mark Adler
 * For conditions of distribution and use, see copyright notice in zlib.h
 *
 * Thanks to Rodney Brown <rbrown64@csc.com.au> for his contribution of faster
 * CRC methods: exclusive-oring 32 bits of data at a time, and pre-computing
 * tables for updating the shift register in one step with three exclusive-ors
 * instead of four steps with four exclusive-ors.  This results in about a
 * factor of two increase in speed on a Power PC G4 (PPC7455) using gcc -O3.
 */

#include <cstdint>
#include <cstddef>
#include "crc32_tables.h"

namespace Zlib {

uint32_t crc32(uint32_t crc, const uint8_t* buf, size_t len)
{
    if (buf == nullptr) return 0UL;
    uint32_t c = ~(uint32_t)crc;
    while (len >= 4) {
        c ^= ((uint32_t)buf[3] << 24) | (buf[2] << 16) | (buf[1] << 8) | (buf[0]);
        c = crc_table[3][c & 0xff] ^ 
            crc_table[2][(c >> 8) & 0xff] ^
            crc_table[1][(c >> 16) & 0xff] ^ 
            crc_table[0][c >> 24];
        len -= 4;
        buf += 4;
    }
    if (len) do {
        c = crc_table[0][(c ^ *buf++) & 0xff] ^ (c >> 8);
    } while (--len);
    return ~c;
}

}


