/*
 * rle.h: RLE utils
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: rle.h,v 1.2 2009/02/16 16:14:58 phintuka Exp $
 *
 */

#ifndef XINELIBOUTPUT_RLE_H_
#define XINELIBOUTPUT_RLE_H_

#if defined __cplusplus
extern "C" {
#endif

typedef enum {
  scale_fast = 0,         /* simple pixel doubling/dropping */
  scale_good_BW = 1,      /* linear interpolation, palette re-generation */
} scale_mode_t;


struct xine_rle_elem_s;
struct xine_clut_s;


int  rle_compress(struct xine_rle_elem_s **rle_data, const uint8_t *data, uint w, uint h);

void rle_uncompress_lut8(const struct xine_rle_elem_s *rle_data,
                         uint8_t *data, uint w, uint h);
void rle_uncompress_argb(uint32_t *dst,
                         const struct xine_rle_elem_s *rle_data, uint num_rle,
                         uint w, uint h, uint stride,
                         struct xine_clut_s *palette);

/*
 * rle_scale_nearest()
 *
 * - Simple nearest-neighbour scaling for RLE-compressed image
 * - fast scaling in compressed form without decompression
 */
struct xine_rle_elem_s *rle_scale_nearest(const struct xine_rle_elem_s *old_rle,
                                          int *rle_elems,
                                          uint w, uint h, uint new_w, uint new_h);


#if defined __cplusplus
}
#endif

#endif /* XINELIBOUTPUT_RLE_H_ */
