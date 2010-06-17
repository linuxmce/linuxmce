/*
 * rle.c: RLE utils
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: rle.c,v 1.4 2010/05/21 11:55:15 phintuka Exp $
 *
 */

#include <stdint.h>
#include <stdlib.h>

#include "../xine_osd_command.h"

#include "rle.h"


#undef  MAX
#define MAX(a,b) ((a) > (b) ? (a) : (b))

/*
 * rle_compress()
 *
 */
uint rle_compress(xine_rle_elem_t **rle_data, const uint8_t *data, uint w, uint h)
{
  xine_rle_elem_t rle, *rle_p = 0, *rle_base;
  uint x, y, num_rle = 0, rle_size = 8128;
  const uint8_t *c;

  rle_p = (xine_rle_elem_t*)malloc(4*rle_size);
  rle_base = rle_p;

  for (y = 0; y < h; y++) {
    rle.len = 0;
    rle.color = 0;
    c = data + y * w;
    for (x = 0; x < w; x++, c++) {
      if (rle.color != *c) {
        if (rle.len) {
          if ( (num_rle + h-y+1) > rle_size ) {
            rle_size *= 2;
            rle_base = (xine_rle_elem_t*)realloc( rle_base, 4*rle_size );
            rle_p = rle_base + num_rle;
          }
          *rle_p++ = rle;
          num_rle++;
        }
        rle.color = *c;
        rle.len = 1;
      } else {
        rle.len++;
      }
    }
    *rle_p++ = rle;
    num_rle++;
  }

  *rle_data = rle_base;
  return num_rle;
}

/*
 * rle_scale_nearest()
 *
 * - Simple nearest-neighbour scaling for RLE-compressed image
 * - fast scaling in compressed form without decompression
 */
xine_rle_elem_t *rle_scale_nearest(const xine_rle_elem_t *old_rle, int *rle_elems,
                                   uint w, uint h, uint new_w, uint new_h)
{
  #define FACTORBASE      0x100
  #define FACTOR2PIXEL(f) ((f)>>8)
  #define SCALEX(x) FACTOR2PIXEL(factor_x*(x))
  #define SCALEY(y) FACTOR2PIXEL(factor_y*(y))

  uint old_w = w, old_h = h;
  uint old_y = 0, new_y = 0;
  uint factor_x = FACTORBASE*new_w/old_w;
  uint factor_y = FACTORBASE*new_h/old_h;
  uint rle_size = MAX(8128, *rle_elems * new_h/h ); /* guess ... */
  uint num_rle  = 0;
  xine_rle_elem_t *new_rle = (xine_rle_elem_t*)malloc(sizeof(xine_rle_elem_t)*rle_size);
  xine_rle_elem_t *new_rle_start = new_rle;

  /* we assume rle elements are breaked at end of line */
  while (old_y < old_h) {
    uint elems_current_line = 0;
    uint old_x = 0, new_x = 0;

    while (old_x < old_w) {
      uint new_x_end = SCALEX(old_x + old_rle->len);

      if (new_x_end > new_w) {
        new_x_end = new_w;
      }

      new_rle->len   = new_x_end - new_x;
      new_rle->color = old_rle->color;

      old_x += old_rle->len;
      old_rle++; /* may be incremented to last element + 1 (element is not accessed anymore) */

      if (new_rle->len > 0) {
        new_x += new_rle->len;
        new_rle++;

        num_rle++;
        elems_current_line++;

        if ( (num_rle + 1) >= rle_size ) {
          rle_size *= 2;
          new_rle_start = (xine_rle_elem_t*)realloc( new_rle_start, 4*rle_size);
          new_rle = new_rle_start + num_rle;
        }
      }
    }
    if (new_x < new_w)
      (new_rle-1)->len += new_w - new_x;
    old_y++;
    new_y++;

    if (factor_y > FACTORBASE) {
      /* scale up -- duplicate current line ? */
      int dup = SCALEY(old_y) - new_y;

      /* if no lines left in (old) rle, copy all lines still missing from new */
      if (old_y == old_h)
        dup = new_h - new_y - 1;

      while (dup-- && (new_y+1<new_h)) {
        xine_rle_elem_t *prevline;
        uint n;
        if ( (num_rle + elems_current_line + 1) >= rle_size ) {
          rle_size *= 2;
          new_rle_start = (xine_rle_elem_t*)realloc( new_rle_start, 4*rle_size);
          new_rle = new_rle_start + num_rle;
        }

        /* duplicate previous line */
        prevline = new_rle - elems_current_line;
        for (n = 0; n < elems_current_line; n++) {
          *new_rle++ = *prevline++;
          num_rle++;
        }
        new_y++;
      }

    } else if (factor_y < FACTORBASE) {
      /* scale down -- drop next line ? */
      uint skip = new_y - SCALEY(old_y);
      if (old_y == old_h-1) {
        /* one (old) line left ; don't skip it if new rle is not complete */
        if (new_y < new_h)
          skip = 0;
      }
      while (skip-- &&
             old_y<old_h /* rounding error may add one line, filter it out */) {
        for (old_x = 0; old_x < old_w;) {
          old_x += old_rle->len;
          old_rle++;
        }
        old_y++;
      }
    }
  }

  *rle_elems = num_rle;
  return new_rle_start;
}
