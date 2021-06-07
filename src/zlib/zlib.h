/* zlib.h -- interface of the 'zlib' general purpose compression library
  version 1.2.11++, February 10th, 2021

  Copyright (C) 1995-2017 Jean-loup Gailly and Mark Adler

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.

  Jean-loup Gailly        Mark Adler
  jloup@gzip.org          madler@alumni.caltech.edu

  Updated to C++ 2021 Peter Bindels

  The data format used by the zlib library is described by RFCs (Request for
  Comments) 1950 to 1952 in the files http://tools.ietf.org/html/rfc1950
  (zlib format), rfc1951 (deflate format) and rfc1952 (gzip format).
*/

/* Consider this the mark that this is *not* the "original software" K&R C version. */

#ifndef ZLIB_H
#define ZLIB_H

#include <cstdint>
#include <cstdarg>
#include <cstddef>

namespace Zlib {

static constexpr int MAX_MEM_LEVEL = 9;
static constexpr int MAX_WBITS = 15;

static constexpr uint8_t ZLIB_VER_MAJOR = 1;
static constexpr uint8_t ZLIB_VER_MINOR = 2;
static constexpr uint8_t ZLIB_VER_REVISION = 11;
static constexpr uint8_t ZLIB_VER_SUBREVISION = 1;
static constexpr const char* ZLIB_VERSION = "1.2.11++";
static constexpr uint16_t ZLIB_VERNUM = 0x1000 * ZLIB_VER_MAJOR + 0x100 * ZLIB_VER_MINOR + 0x10 * ZLIB_VER_REVISION + ZLIB_VER_SUBREVISION;

struct internal_state;

struct z_stream {
    const uint8_t *next_in;     /* next input byte */
    uint64_t     avail_in;  /* number of bytes available at next_in */
    uint64_t    total_in;  /* total number of input bytes read so far */

    uint8_t    *next_out; /* next output byte will go here */
    uint64_t     avail_out; /* remaining free space at next_out */
    uint64_t    total_out; /* total number of bytes output so far */

    const char *msg;  /* last error message, NULL if no error */
    struct internal_state  *state; /* not visible by applications */

    int     data_type;  /* best guess about the data type: binary or text
                           for deflate, or the decoding state for inflate */
    uint32_t   adler;      /* Adler-32 or CRC-32 value of the uncompressed data */
    uint32_t   reserved;   /* reserved for future use */
};

struct gz_header {
    int     text;       /* true if compressed data believed to be text */
    uint64_t   time;       /* modification time */
    int     xflags;     /* extra flags (not used when writing a gzip file) */
    int     os;         /* operating system */
    uint8_t   *extra;     /* pointer to extra field or nullptr if none */
    uint64_t    extra_len;  /* extra field length (valid if extra != nullptr) */
    uint64_t    extra_max;  /* space at extra (only when reading header) */
    uint8_t   *name;      /* pointer to zero-terminated file name or nullptr */
    uint64_t    name_max;   /* space at name (only when reading header) */
    uint8_t   *comment;   /* pointer to zero-terminated comment or nullptr */
    uint64_t    comm_max;   /* space at comment (only when reading header) */
    int     hcrc;       /* true if there was or will be a header crc */
    int     done;       /* true when done reading gzip header (not used
                           when writing a gzip file) */
};

enum FlushValues {
  Z_NO_FLUSH = 0,
  Z_PARTIAL_FLUSH = 1,
  Z_SYNC_FLUSH = 2,
  Z_FULL_FLUSH = 3,
  Z_FINISH = 4,
  Z_BLOCK = 5,
  Z_TREES = 6,
};

enum ZlibReturnValues {
  Z_OK = 0,
  Z_STREAM_END = 1,
  Z_NEED_DICT = 2,
  Z_ERRNO = (-1),
  Z_STREAM_ERROR = (-2),
  Z_DATA_ERROR = (-3),
  Z_MEM_ERROR = (-4),
  Z_BUF_ERROR = (-5),
  Z_VERSION_ERROR = (-6),
};

enum CompressionLevel {
  Z_NO_COMPRESSION = 0,
  Z_BEST_SPEED = 1,
  Z_BEST_COMPRESSION = 9,
  Z_DEFAULT_COMPRESSION = (-1),
};

enum CompressionStrategy {
  Z_DEFAULT_STRATEGY = 0,
  Z_FILTERED = 1,
  Z_HUFFMAN_ONLY = 2,
  Z_RLE = 3,
  Z_FIXED = 4,
};

enum DataType {
  Z_BINARY = 0,
  Z_TEXT = 1,
  Z_UNKNOWN = 2,
};

static constexpr uint8_t Z_DEFLATED = 8;

extern int deflateInit (z_stream* strm, int level, const char *version = ZLIB_VERSION, int stream_size = (int)sizeof(z_stream));
extern int deflate(z_stream* strm, int flush);
extern int deflateInit2 (z_stream* strm, int  level, int  method, int windowBits, int memLevel, int strategy, const char *version = ZLIB_VERSION, int stream_size = (int)sizeof(z_stream));
extern int deflateEnd (z_stream* strm);

extern int inflateInit2 (z_stream* strm, int  windowBits, const char *version = ZLIB_VERSION, int stream_size = (int)sizeof(z_stream));
extern int inflateInit (z_stream* strm, const char *version = ZLIB_VERSION, int stream_size = (int)sizeof(z_stream));
extern int inflate (z_stream* strm, int flush);
extern int inflateEnd (z_stream* strm);

extern uint32_t adler32 (uint32_t adler, const uint8_t *buf, size_t len);
extern uint32_t crc32 (uint32_t crc, const uint8_t *buf, size_t len);

}

#endif /* ZLIB_H */
