/*
 * h264.h: H.264 bitstream decoding
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: h264.h,v 1.2 2008/03/16 22:12:56 phintuka Exp $
 *
 */

#ifndef _XINELIBOUTPUT_H264_H_
#define _XINELIBOUTPUT_H264_H_


#ifdef __cplusplus
extern "C" {
#endif

#include "mpeg.h"

typedef struct {
  int width;
  int height;
  double pixel_aspect;
  /* ... */
} h264_sps_data_t;

/*
 * input: start of NAL SPS (without 00 00 01 07)
 */
int h264_parse_sps(const uint8_t *buf, int len, h264_sps_data_t *sps);

/*
 * input: start of H.264 video data (not PES)
 */
int  h264_get_picture_type(const uint8_t *buf, int len);

/*
 * input: start of H.264 video data (not PES)
 */
int  h264_get_video_size(const uint8_t *buf, int len, video_size_t *size);


#ifdef __cplusplus
} /* extern "C" { */
#endif


#endif /* _XINELIBOUTPUT_H264_H_ */
