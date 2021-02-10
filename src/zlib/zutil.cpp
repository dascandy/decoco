/* zutil.c -- target dependent utility functions for the compression library
 * Copyright (C) 1995-2017 Jean-loup Gailly
 * For conditions of distribution and use, see copyright notice in zlib.h
 */

/* @(#) $Id$ */

#include "zutil.h"

namespace Zlib {

const char * const z_errmsg[10] = {
    (const char *)"need dictionary",     /* Z_NEED_DICT       2  */
    (const char *)"stream end",          /* Z_STREAM_END      1  */
    (const char *)"",                    /* Z_OK              0  */
    (const char *)"file error",          /* Z_ERRNO         (-1) */
    (const char *)"stream error",        /* Z_STREAM_ERROR  (-2) */
    (const char *)"data error",          /* Z_DATA_ERROR    (-3) */
    (const char *)"insufficient memory", /* Z_MEM_ERROR     (-4) */
    (const char *)"buffer error",        /* Z_BUF_ERROR     (-5) */
    (const char *)"incompatible version",/* Z_VERSION_ERROR (-6) */
    (const char *)""
};

}


