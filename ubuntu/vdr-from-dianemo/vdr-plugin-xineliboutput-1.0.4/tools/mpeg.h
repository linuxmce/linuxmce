/*
 * mpeg.h: MPEG definitions
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: mpeg.h,v 1.2 2008/03/16 22:12:56 phintuka Exp $
 *
 */

#ifndef XINELIBOUTPUT_MPEG_H_
#define XINELIBOUTPUT_MPEG_H_


#ifdef __cplusplus
extern "C" {
#endif


#define SC_PICTURE     0x00  /* picture atart code */
#define SC_SEQUENCE    0xb3  /* sequence header    */

/* Picture types */
#define NO_PICTURE  0
#define I_FRAME     1
#define P_FRAME     2
#define B_FRAME     3


typedef struct {
  int width;
  int height;
  double pixel_aspect;
} video_size_t;


extern const char * const picture_type_str[];

/*
 * input: start of MPEG video data (not PES)
 */
int mpeg2_get_picture_type(const uint8_t *buf, int len);

/*
 * input: start of MPEG video data (not PES)
 */
int mpeg2_get_video_size(const uint8_t *buf, int len, video_size_t *size);


#ifdef __cplusplus
} /* extern "C" { */
#endif


#endif
