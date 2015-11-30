/*
 * rle.c: RLE utils
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: rle.c,v 1.12 2014-06-23 12:20:28 phintuka Exp $
 *
 */

#include <stdint.h>
#include <stdlib.h>
#ifdef __FreeBSD__
#include <sys/types.h>
#endif

#include "osd_command.h"

#include "rle.h"


#undef  MAX
#define MAX(a,b) ((a) > (b) ? (a) : (b))

/*
 * rle_compress()
 *
 */
uint rle_compress(osd_rle_elem_t **rle_data, const uint8_t *data, uint w, uint h)
{
  osd_rle_elem_t rle, *rle_p = 0, *rle_base;
  uint x, y, num_rle = 0, rle_size = 8128;
  const uint8_t *c;

  rle_p = (osd_rle_elem_t*)malloc(4*rle_size);
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
            rle_base = (osd_rle_elem_t*)realloc( rle_base, 4*rle_size );
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

uint rle_compress_net(uint8_t **rle_data, uint *elems, const uint8_t *data, uint w, uint h)
{
  *elems = rle_compress((osd_rle_elem_t**)rle_data, data, w, h);
  return rle_recompress_net(*rle_data, *(osd_rle_elem_t **)rle_data, *elems);
}

/*
 * rle_recompress_net()
 *
 * recompress RLE-compressed OSD using variable sized RLE codewords
*/
uint rle_recompress_net(uint8_t *raw, osd_rle_elem_t *data, uint elems)
{
  uint8_t *raw0 = raw;
  uint i;

  for (i = 0; i < elems; i++) {
    uint16_t len   = data[i].len;
    uint16_t color = data[i].color;
    if (len >= 0x80) {
      *(raw++) = (len>>8) | 0x80;
      *(raw++) = (len & 0xff);
    } else {
      *(raw++) = (len & 0x7f);
    }
    *(raw++) = color;
  }

  return (raw - raw0);
}

/*
 * rle_scale_nearest()
 *
 * - Simple nearest-neighbour scaling for RLE-compressed image
 * - fast scaling in compressed form without decompression
 */
osd_rle_elem_t *rle_scale_nearest(const osd_rle_elem_t *old_rle, int *rle_elems,
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
  osd_rle_elem_t *new_rle = (osd_rle_elem_t*)malloc(sizeof(osd_rle_elem_t)*rle_size);
  osd_rle_elem_t *new_rle_start = new_rle;

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
          new_rle_start = (osd_rle_elem_t*)realloc( new_rle_start, 4*rle_size);
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
        osd_rle_elem_t *prevline;
        uint n;
        if ( (num_rle + elems_current_line + 1) >= rle_size ) {
          rle_size *= 2;
          new_rle_start = (osd_rle_elem_t*)realloc( new_rle_start, 4*rle_size);
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

/*
 * encode single HDMV PG rle element
 */
static uint8_t *write_rle_hdmv(uint8_t *rle_data, uint color, uint len)
{
  /* short non-transparent sequences are uncompressed */
  if (color && len < 4) {
    uint i;
    for (i = 0; i < len; i++) {
      *rle_data++ = color;
    }
    return rle_data;
  }

  /* rle code marker */
  *rle_data++ = 0;

  if (!color) {
    /* transparent */
    if (len < 64) {
      *rle_data++ = len;
    } else {
      *rle_data++ = 0x40 | ((len >> 8) & 0x3f);
      *rle_data++ = len & 0xff;
    }
  } else {
    if (len < 64) {
      *rle_data++ = 0x80 | len;
    } else {
      *rle_data++ = 0x80 | 0x40 | ((len >> 8) & 0x3f);
      *rle_data++ = len & 0xff;
    }
    *rle_data++ = color;
  }

  return rle_data;
}

/*
 * compress LUT8 image using HDMV PG compression algorithm
 */
size_t rle_compress_hdmv(uint8_t **rle_data, const uint8_t *data, uint w, uint h, int *num_rle)
{
  uint     y;
  size_t   rle_size = 0;
  uint8_t *rle = NULL;

  *rle_data = NULL;
  *num_rle = 0;

  for (y = 0; y < h; y++) {

    /* grow buffer ? */
    if ((ssize_t)(rle_size - (rle - *rle_data)) < w * 4) {
      size_t used = rle - *rle_data;
      rle_size = rle_size < 1 ? w*h/16 : rle_size*2;
      *rle_data = realloc(*rle_data, rle_size);
      rle = *rle_data + used;
    }

    /* compress line */
    uint color = *data;
    uint len   = 1;
    uint x     = 1;

    for (x = 1; x < w; x++) {
      if (data[x] == color) {
        len++;
      } else {
        rle = write_rle_hdmv(rle, color, len);
        (*num_rle)++;
        color = data[x];
        len   = 1;
      }
    }

    if (len) {
      rle = write_rle_hdmv(rle, color, len);
      (*num_rle)++;
    }

    /* end of line marker */
    rle = write_rle_hdmv(rle, 0, 0);
    (*num_rle)++;
    data += w;
  }

  return rle - *rle_data;
}


int rle_uncompress_hdmv(osd_rle_elem_t **data,
                        uint w, uint h,
                        const uint8_t *rle_data, uint num_rle, size_t rle_size)
{
  uint rle_count = 0, x = 0, y = 0;
  osd_rle_elem_t *rlep = calloc(2*num_rle, sizeof(osd_rle_elem_t));
  const uint8_t *end = rle_data + rle_size;

  *data = rlep;

  /* convert to xine-lib rle format */
  while (y < h) {

    if (rle_data >= end || rle_count >= 2*num_rle) {
      free(*data);
      *data = NULL;
      return -1 - (rle_data >= end);
    }

    /* decode RLE element */
    uint byte = *rle_data++;
    if (byte) {
      rlep->color = byte;
      rlep->len   = 1;
    } else {
      byte = *rle_data++;
      if (!(byte & 0x80)) {
        rlep->color = 0;
        if (!(byte & 0x40))
          rlep->len = byte & 0x3f;
        else
          rlep->len = ((byte & 0x3f) << 8) | *rle_data++;
      } else {
        if (!(byte & 0x40))
          rlep->len = byte & 0x3f;
        else
          rlep->len = ((byte & 0x3f) << 8) | *rle_data++;
        rlep->color = *rle_data++;
      }
    }

    /* move to next element */
    if (rlep->len > 0) {

      if (rlep->len == 1 && x && rlep[-1].color == rlep->color) {
        rlep[-1].len++;
        x++;
      } else {
        x += rlep->len;
        rlep++;
        rle_count++;
      }

      if (x > w) {
        return -9999;
      }

    } else {
      /* end of line marker (00 00) */
      if (x < w-1) {
        //return -1-rlep->color - (w-x);
        rlep->len = w - x;
        rlep->color = 0xff;
        rlep++;
        rle_count++;
      }
      x = 0;
      y++;
    }
  }

  return rle_count;
}

void rle_uncompress_lut8(uint8_t *dst,
                         uint w, uint h, uint stride,
                         const struct osd_rle_elem_s *rle_data, uint num_rle)
{
  uint i, pixelcounter = 0;
  uint idx = 0, line = 0;

  for(i = 0; i < num_rle; ++i) {
    uint8_t color = (rle_data + i)->color;
    uint    len   = (rle_data + i)->len;
    uint    j;

    for (j = 0; j < len; ++j) {
      if (pixelcounter >= w) {
        idx += stride - pixelcounter;
        pixelcounter = 0;
        if (++line >= h)
          return;
      }
      dst[idx] = color;
      ++idx;
      ++pixelcounter;
    }
  }
}

void rle_palette_to_argb(uint32_t *argb, const struct osd_clut_s *palette, uint entries)
{
  uint i;
  for (i = 0; i < entries; i++) {
    argb[i] = (palette[i].alpha << 24) |
              (palette[i].r     << 16) |
              (palette[i].g     << 8 ) |
              (palette[i].b          );
  }
}

void rle_palette_to_rgba(uint32_t *rgba, const struct osd_clut_s *palette, uint entries)
{
  uint i;
  for (i = 0; i < entries; i++) {
    rgba[i] = (palette[i].r     << 24) |
              (palette[i].g     << 16) |
              (palette[i].b     << 8 ) |
              (palette[i].alpha      );
  }
}

static void rle_uncompress_u32(uint32_t *dst,
                               uint w, uint h, uint stride,
                               const struct osd_rle_elem_s *rle_data, uint num_rle,
                               uint32_t *lut)
{
  uint i, pixelcounter = 0;
  uint idx = 0, line = 0;

  for(i = 0; i < num_rle; ++i) {
    uint32_t color = lut[(rle_data + i)->color];
    uint     len   = (rle_data + i)->len;
    uint     j;

    for (j = 0; j < len; ++j) {
      if (pixelcounter >= w) {
        idx += stride - pixelcounter;
        pixelcounter = 0;
        if (++line >= h)
          return;
      }
      dst[idx] = color;
      ++idx;
      ++pixelcounter;
    }
  }
}

void rle_uncompress_argb(uint32_t *dst,
                         uint w, uint h, uint stride,
                         const struct osd_rle_elem_s *rle_data, uint num_rle,
                         const struct osd_clut_s *palette, uint palette_entries)
{
  uint32_t lut[256] = {0};

  if (palette_entries > 256)
    return;

  rle_palette_to_argb(lut, palette, palette_entries);

  rle_uncompress_u32(dst, w, h, stride, rle_data, num_rle, lut);
}

void rle_uncompress_rgba(uint32_t *dst,
                         uint w, uint h, uint stride,
                         const struct osd_rle_elem_s *rle_data, uint num_rle,
                         const struct osd_clut_s *palette, uint palette_entries)
{
  uint32_t lut[256] = {0};

  if (palette_entries > 256)
    return;

  rle_palette_to_rgba(lut, palette, palette_entries);

  rle_uncompress_u32(dst, w, h, stride, rle_data, num_rle, lut);
}
