/*
 * h264.c: H.264 bitstream decoding
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: h264.c,v 1.9 2010-02-02 22:47:35 phintuka Exp $
 *
 */

#include <stdint.h>
#include <string.h>

#ifndef LOG_MODULENAME
#  define LOG_MODULENAME "[h264     ] "
#  define SysLogLevel    iSysLogLevel
#  include "../logdefs.h"
#endif

#define NOCACHE 1
#include "bitstream.h"

#include "mpeg.h"
#include "h264.h"

int h264_parse_sps(const uint8_t *buf, int len, h264_sps_data_t *sps)
{
  br_state br = BR_INIT(buf, len);
  int profile_idc, pic_order_cnt_type;
  int frame_mbs_only;
  int i, j;

  profile_idc = br_get_u8(&br);
  LOGDBG("H.264 SPS: profile_idc %d", profile_idc);
  /* constraint_set0_flag = br_get_bit(br);    */
  /* constraint_set1_flag = br_get_bit(br);    */
  /* constraint_set2_flag = br_get_bit(br);    */
  /* constraint_set3_flag = br_get_bit(br);    */
  /* reserved             = br_get_bits(br,4); */
  /* level_idc            = br_get_u8(br);     */ 
  br_skip_bits(&br, 16);
  br_skip_ue_golomb(&br);   /* seq_parameter_set_id */
  if (profile_idc >= 100) {
    if (br_get_ue_golomb(&br) == 3) /* chroma_format_idc      */
      br_skip_bit(&br);     /* residual_colour_transform_flag */
    br_skip_ue_golomb(&br); /* bit_depth_luma - 8             */
    br_skip_ue_golomb(&br); /* bit_depth_chroma - 8           */
    br_skip_bit(&br);       /* transform_bypass               */
    if (br_get_bit(&br))    /* seq_scaling_matrix_present     */
      for (i = 0; i < 8; i++)  
	if (br_get_bit(&br)) { /* seq_scaling_list_present    */
	  int last = 8, next = 8, size = (i<6) ? 16 : 64;
	  for (j = 0; j < size; j++) {
	    if (next)
	      next = (last + br_get_se_golomb(&br)) & 0xff;
	    last = next ?: last;
	  }
	}
  }

  br_skip_ue_golomb(&br);      /* log2_max_frame_num - 4 */
  pic_order_cnt_type = br_get_ue_golomb(&br);
  if (pic_order_cnt_type == 0)
    br_skip_ue_golomb(&br);    /* log2_max_poc_lsb - 4 */
  else if (pic_order_cnt_type == 1) {
    br_skip_bit(&br);          /* delta_pic_order_always_zero     */
    br_skip_se_golomb(&br);    /* offset_for_non_ref_pic          */
    br_skip_se_golomb(&br);    /* offset_for_top_to_bottom_field  */
    j = br_get_ue_golomb(&br); /* num_ref_frames_in_pic_order_cnt_cycle */
    for (i = 0; i < j; i++)
      br_skip_se_golomb(&br);  /* offset_for_ref_frame[i]         */
  }
  br_skip_ue_golomb(&br);      /* ref_frames                      */
  br_skip_bit(&br);            /* gaps_in_frame_num_allowed       */
  sps->width  /* mbs */ = br_get_ue_golomb(&br) + 1;
  sps->height /* mbs */ = br_get_ue_golomb(&br) + 1;
  frame_mbs_only        = br_get_bit(&br);
  LOGDBG("H.264 SPS: pic_width:  %u mbs", (unsigned) sps->width);
  LOGDBG("H.264 SPS: pic_height: %u mbs", (unsigned) sps->height);
  LOGDBG("H.264 SPS: frame only flag: %d", frame_mbs_only);
  
  sps->width  *= 16;
  sps->height *= 16 * (2-frame_mbs_only);
  
  if (!frame_mbs_only)
    if (br_get_bit(&br)) /* mb_adaptive_frame_field_flag */
      LOGDBG("H.264 SPS: MBAFF");
  br_skip_bit(&br);      /* direct_8x8_inference_flag    */
  if (br_get_bit(&br)) { /* frame_cropping_flag */
    uint32_t crop_left   = br_get_ue_golomb(&br);
    uint32_t crop_right  = br_get_ue_golomb(&br);
    uint32_t crop_top    = br_get_ue_golomb(&br);
    uint32_t crop_bottom = br_get_ue_golomb(&br);
    LOGDBG("H.264 SPS: cropping %d %d %d %d", 
	   crop_left, crop_top, crop_right, crop_bottom);

    sps->width -= 2*(crop_left + crop_right);
    if (frame_mbs_only)
      sps->height -= 2*(crop_top + crop_bottom); 
    else
      sps->height -= 4*(crop_top + crop_bottom);
  }

  /* VUI parameters */
  sps->pixel_aspect.num = 0;
  if (br_get_bit(&br)) {   /* vui_parameters_present flag */
    if (br_get_bit(&br)) { /* aspect_ratio_info_present */
      uint32_t aspect_ratio_idc = br_get_u8(&br);
      LOGDBG("H.264 SPS: aspect_ratio_idc %d", aspect_ratio_idc);

      if (aspect_ratio_idc == 255 /* Extended_SAR */) {
	sps->pixel_aspect.num = br_get_u16(&br); /* sar_width */
	sps->pixel_aspect.den = br_get_u16(&br); /* sar_height */
	LOGDBG("H.264 SPS: -> sar %dx%d", sps->pixel_aspect.num, sps->pixel_aspect.den);
      } else {
	static const mpeg_rational_t aspect_ratios[] =
	  { /* page 213: */
	    /* 0: unknown */
	    {0, 1},
	    /* 1...16: */
	    { 1,  1}, {12, 11}, {10, 11}, {16, 11}, { 40, 33}, {24, 11}, {20, 11}, {32, 11}, 
	    {80, 33}, {18, 11}, {15, 11}, {64, 33}, {160, 99}, { 4,  3}, { 3,  2}, { 2,  1}
	  };

	if (aspect_ratio_idc < sizeof(aspect_ratios)/sizeof(aspect_ratios[0])) {
	  memcpy(&sps->pixel_aspect, &aspect_ratios[aspect_ratio_idc], sizeof(mpeg_rational_t));
	  LOGDBG("H.264 SPS: -> aspect ratio %d / %d", sps->pixel_aspect.num, sps->pixel_aspect.den);
	} else {
	  LOGMSG("H.264 SPS: aspect_ratio_idc out of range !");
	}
      }
    }
  }

  LOGDBG("H.264 SPS: -> video size %dx%d, aspect %d:%d",
	 sps->width, sps->height, sps->pixel_aspect.num, sps->pixel_aspect.den);

  if(BR_EOF(&br)) {
    LOGDBG("H.264 SPS: not enough data ?");
    return 0;
  }
  return 1;
}

static int h264_nal_unescape(uint8_t *dst, const uint8_t *src, int len)
{
  int s = 0, d = 0;
  while (s < len) {
    if (!src[s] && !src[s+1]) {
      /* hit 00 00 xx */
      dst[d] = dst[d+1] = 0;
      s += 2;
      d += 2;
      if (src[s] == 3) {
	s++; /* 00 00 03 xx --> 00 00 xx */
	/*LOGDBG("h264_nal_unescape: hit 00 00 03 %02x", src[s]);*/
	if (s >= len)
	  return d;
      } /* else if (src[s] == 0 || src[s] == 1) {
	LOGDBG("h264_nal_unescape: invalid NAL sequence 00 00 %02x %02x", src[s], src[s+1]);
	return -1;
      }*/
    }
    dst[d++] = src[s++];
  }
  return d;
}

int h264_get_picture_type(const uint8_t *buf, int len)
{
  int i;
  for (i = 0; i < len-5; i++) {
    if (IS_NAL_AUD(buf + i)) {
      uint8_t type = (buf[i + 4] >> 5);
      switch (type) {
        case 0: case 3: case 5: return I_FRAME;
        case 1: case 4: case 6: return P_FRAME;
        case 2: case 7:         return B_FRAME;
        default:;
      }
    }
  }
  return NO_PICTURE;
}

int h264_get_video_size(const uint8_t *buf, int len, video_size_t *size)
{
  int i;

  /* if I-frame, search for NAL SPS */
  if (h264_get_picture_type(buf, len) != I_FRAME)
    return 0;

  /* scan video packet for sequence parameter set */
  for (i = 5; i < len-4; i++) 
    if (buf[i] == 0 && buf[i + 1] == 0 && buf[i + 2] == 1 && (buf[i + 3] & 0x1f) == NAL_SPS) {

      uint8_t nal_data[len];
      int     nal_len;

      LOGDBG("H.264: Found NAL SPS at offset %d/%d", i, len);

      if (0 < (nal_len = h264_nal_unescape(nal_data, buf+i+4, len-i-4))) {

	h264_sps_data_t sps = {0};

	if (h264_parse_sps(nal_data, nal_len, &sps)) {
	  size->width  = sps.width;
	  size->height = sps.height;
	  memcpy(&size->pixel_aspect, &sps.pixel_aspect, sizeof(mpeg_rational_t));
	  return 1;
	}
	LOGMSG("h264_get_video_size: not enough data ?");
      }
    }

  return 0;
}

