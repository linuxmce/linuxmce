/*
 * rle.h: RLE utils
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: rle.h,v 1.8 2014-06-23 12:14:15 phintuka Exp $
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


struct osd_rle_elem_s;
struct osd_clut_s;


uint rle_compress(struct osd_rle_elem_s **rle_data, const uint8_t *data, uint w, uint h);
uint rle_compress_net(uint8_t **rle_data, uint *elems, const uint8_t *data, uint w, uint h);
uint rle_recompress_net(uint8_t *raw, osd_rle_elem_t *data, uint elems);

void rle_palette_to_argb(uint32_t *argb, const struct osd_clut_s *palette, uint entries);
void rle_palette_to_rgba(uint32_t *rgba, const struct osd_clut_s *palette, uint entries);

void rle_uncompress_lut8(uint8_t *dst,
                         uint w, uint h, uint stride,
                         const struct osd_rle_elem_s *rle_data, uint num_rle);
void rle_uncompress_argb(uint32_t *dst,
                         uint w, uint h, uint stride,
                         const struct osd_rle_elem_s *rle_data, uint num_rle,
                         const struct osd_clut_s *palette, uint palette_entries);
void rle_uncompress_rgba(uint32_t *dst,
                         uint w, uint h, uint stride,
                         const struct osd_rle_elem_s *rle_data, uint num_rle,
                         const struct osd_clut_s *palette, uint palette_entries);

/*
 * rle_scale_nearest()
 *
 * - Simple nearest-neighbour scaling for RLE-compressed image
 * - fast scaling in compressed form without decompression
 */
struct osd_rle_elem_s *rle_scale_nearest(const struct osd_rle_elem_s *old_rle,
                                          int *rle_elems,
                                          uint w, uint h, uint new_w, uint new_h);


/*
 * HDMV (BluRay) presentation graphics format
 */

size_t rle_compress_hdmv(uint8_t **rle_data, const uint8_t *data, uint w, uint h, int *num_rle);
int rle_uncompress_hdmv(struct osd_rle_elem_s **data,
                        uint w, uint h,
                        const uint8_t *rle_data, uint num_rle, size_t rle_size);


#if defined __cplusplus
}
#endif

#endif /* XINELIBOUTPUT_RLE_H_ */
