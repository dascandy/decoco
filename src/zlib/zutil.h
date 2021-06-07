/* zutil.h -- internal interface and configuration of the compression library
 * Copyright (C) 1995-2016 Jean-loup Gailly, Mark Adler
 * For conditions of distribution and use, see copyright notice in zlib.h
 */

/* WARNING: this file should *not* be used by applications. It is
   part of the implementation of the compression library and is
   subject to change. Applications should only use zlib.h.
 */

#ifndef ZUTIL_H
#define ZUTIL_H

#include "zlib.h"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <cstdlib>

namespace Zlib {

extern const char * const z_errmsg[10]; /* indexed by 2-zlib_error */
/* (size given to avoid silly warnings with Visual C++) */

#define ERR_MSG(err) z_errmsg[Z_NEED_DICT-(err)]

#define ERR_RETURN(strm,err) \
  return (strm->msg = ERR_MSG(err), (err))
/* To be used only when the state is known to be valid */

enum BlockType {
  STORED_BLOCK = 0,
  STATIC_TREES = 1,
  DYN_TREES = 2,
};

static constexpr const uint16_t MIN_MATCH = 3;
static constexpr const uint16_t MAX_MATCH = 258;

#define PRESET_DICT 0x20 /* preset dictionary flag in zlib header */

/* Reverse the bytes in a 32-bit value */
#define ZSWAP32(q) (uint32_t)((((q) >> 24) & 0xff) + (((q) >> 8) & 0xff00) + \
                    (((q) & 0xff00) << 8) + (((q) & 0xff) << 24))

}

#endif /* ZUTIL_H */
