/* deflate.h -- internal compression state
 * Copyright (C) 1995-2016 Jean-loup Gailly
 * For conditions of distribution and use, see copyright notice in zlib.h
 */

#ifndef DEFLATE_H
#define DEFLATE_H

#include "zutil.h"

namespace Zlib {

#define LENGTH_CODES 29
#define LITERALS  256
#define L_CODES (LITERALS+1+LENGTH_CODES)
#define D_CODES   30
#define BL_CODES  19
#define HEAP_SIZE (2*L_CODES+1)
#define MAX_BITS 15
#define Buf_size 16

enum State {
  INIT_STATE = 42,
  GZIP_STATE = 57,
  EXTRA_STATE = 69,
  NAME_STATE = 73,
  COMMENT_STATE = 91,
  HCRC_STATE = 103,
  BUSY_STATE = 113,
  FINISH_STATE = 666,
};

/* Data structure describing a single value and its code string. */
typedef struct ct_data_s {
    union {
        uint16_t  freq;       /* frequency count */
        uint16_t  code;       /* bit string */
    } fc;
    union {
        uint16_t  dad;        /* father node in Huffman tree */
        uint16_t  len;        /* length of bit string */
    } dl;
}  ct_data;

#define Freq fc.freq
#define Code fc.code
#define Dad  dl.dad
#define Len  dl.len

typedef struct static_tree_desc_s  static_tree_desc;

typedef struct tree_desc_s {
    ct_data *dyn_tree;           /* the dynamic tree */
    int     max_code;            /* largest code with non zero frequency */
    const static_tree_desc *stat_desc;  /* the corresponding static tree */
}  tree_desc;

typedef uint16_t Pos;
typedef uint32_t IPos;

typedef struct internal_state {
    z_stream* strm;      /* pointer back to this zlib stream */
    int   status;        /* as the name implies */
    uint8_t *pending_buf;  /* output still pending */
    uint64_t   pending_buf_size; /* size of pending_buf */
    uint8_t *pending_out;  /* next pending byte to output to the stream */
    uint64_t   pending;       /* nb of bytes in the pending buffer */
    int   wrap;          /* bit 0 true for zlib, bit 1 true for gzip */
    gz_header*  gzhead;  /* gzip header information to write */
    uint64_t   gzindex;       /* where in extra, name, or comment */
    uint8_t  method;        /* can only be DEFLATED */
    int   last_flush;    /* value of flush param for previous deflate call */

                /* used by deflate.c: */

    uint32_t  w_size;        /* LZ77 window size (32K by default) */
    uint32_t  w_bits;        /* log2(w_size)  (8..16) */
    uint32_t  w_mask;        /* w_size - 1 */

    uint8_t *window;
    uint32_t window_size;
    Pos *prev;
    Pos *head;
    uint32_t  ins_h;          /* hash index of string to be inserted */
    uint32_t  hash_size;      /* number of elements in hash table */
    uint32_t  hash_bits;      /* log2(hash_size) */
    uint32_t  hash_mask;      /* hash_size-1 */

    uint32_t  hash_shift;
    long block_start;
    uint64_t match_length;           /* length of best match */
    IPos prev_match;             /* previous match */
    int match_available;         /* set if previous match exists */
    uint32_t strstart;               /* start of string to insert */
    uint32_t match_start;            /* start of matching string */
    uint32_t lookahead;              /* number of valid bytes ahead in window */

    uint64_t prev_length;
    uint64_t max_chain_length;
    uint64_t max_lazy_match;
#   define max_insert_length  max_lazy_match
    int level;    /* compression level (1..9) */
    int strategy; /* favor or force Huffman coding*/

    uint32_t good_match;
    /* Use a faster search when the previous match is longer than this */

    int nice_match;

    struct ct_data_s dyn_ltree[HEAP_SIZE];   /* literal and length tree */
    struct ct_data_s dyn_dtree[2*D_CODES+1]; /* distance tree */
    struct ct_data_s bl_tree[2*BL_CODES+1];  /* Huffman tree for bit lengths */

    struct tree_desc_s l_desc;               /* desc. for literal tree */
    struct tree_desc_s d_desc;               /* desc. for distance tree */
    struct tree_desc_s bl_desc;              /* desc. for bit length tree */

    uint16_t bl_count[MAX_BITS+1];
    /* number of codes at each bit length for an optimal tree */

    int heap[2*L_CODES+1];      /* heap used to build the Huffman trees */
    int heap_len;               /* number of elements in the heap */
    int heap_max;               /* element of largest frequency */
    uint8_t depth[2*L_CODES+1];
    uint8_t *l_buf;          /* buffer for literals or lengths */

    uint32_t  lit_bufsize;
    uint32_t last_lit;      /* running index in l_buf */

    uint16_t *d_buf;

    uint32_t opt_len;        /* bit length of current block with optimal trees */
    uint32_t static_len;     /* bit length of current block with static trees */
    uint64_t matches;       /* number of string matches in current block */
    uint64_t insert;        /* bytes at end of window left to insert */

    uint16_t bi_buf;
    int bi_valid;

    uint64_t high_water;
}  deflate_state;

/* Output a byte on the stream.
 * IN assertion: there is enough room in pending_buf.
 */
#define put_byte(s, c) {s->pending_buf[s->pending++] = (uint8_t)(c);}


#define MIN_LOOKAHEAD (MAX_MATCH+MIN_MATCH+1)
/* Minimum amount of lookahead, except at the end of the input file.
 * See deflate.c for comments about the MIN_MATCH+1.
 */

#define MAX_DIST(s)  ((s)->w_size-MIN_LOOKAHEAD)
/* In order to simplify the code, particularly on 16 bit machines, match
 * distances are limited to MAX_DIST instead of WSIZE.
 */

#define WIN_INIT MAX_MATCH
/* Number of bytes after end of data in window to initialize in order to avoid
   memory checker errors from longest match routines */

        /* in trees.c */
void _tr_init (deflate_state *s);
int _tr_tally (deflate_state *s, unsigned dist, unsigned lc);
void _tr_flush_block (deflate_state *s, char *buf,
                        uint32_t stored_len, int last);
void _tr_flush_bits (deflate_state *s);
void _tr_align (deflate_state *s);
void _tr_stored_block (deflate_state *s, char *buf,
                        uint32_t stored_len, int last);

#define d_code(dist) \
   ((dist) < 256 ? _dist_code[dist] : _dist_code[256+((dist)>>7)])
/* Mapping from a distance to a distance code. dist is the distance - 1 and
 * must not have side effects. _dist_code[256] and _dist_code[257] are never
 * used.
 */

/* Inline versions of _tr_tally for speed: */

extern const uint8_t _length_code[];
extern const uint8_t _dist_code[];

# define _tr_tally_lit(s, c, flush) \
  { uint8_t cc = (c); \
    s->d_buf[s->last_lit] = 0; \
    s->l_buf[s->last_lit++] = cc; \
    s->dyn_ltree[cc].Freq++; \
    flush = (s->last_lit == s->lit_bufsize-1); \
   }
# define _tr_tally_dist(s, distance, length, flush) \
  { uint8_t len = (uint8_t)(length); \
    uint16_t dist = (uint16_t)(distance); \
    s->d_buf[s->last_lit] = dist; \
    s->l_buf[s->last_lit++] = len; \
    dist--; \
    s->dyn_ltree[_length_code[len]+LITERALS+1].Freq++; \
    s->dyn_dtree[d_code(dist)].Freq++; \
    flush = (s->last_lit == s->lit_bufsize-1); \
  }

}

#endif /* DEFLATE_H */
