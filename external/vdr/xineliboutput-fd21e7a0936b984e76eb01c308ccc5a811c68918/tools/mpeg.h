/*
 * mpeg.h: MPEG definitions
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: mpeg.h,v 1.9 2012-12-31 16:42:45 phintuka Exp $
 *
 */

#ifndef XINELIBOUTPUT_MPEG_H_
#define XINELIBOUTPUT_MPEG_H_


#ifdef __cplusplus
extern "C" {
#endif


#define SC_PICTURE      0x00  /* picture atart code */
#define SC_SEQUENCE     0xb3  /* sequence header    */
#define SC_SEQUENCE_END 0xb7

#if defined(__i386__) || defined(__x86_64__)
#  define IS_SC_PICTURE(buf)  (*(const uint32_t *)(buf) == 0x00010000U)
#  define IS_SC_SEQUENCE(buf) (*(const uint32_t *)(buf) == 0xb3010000U)
#else
#  define IS_SC_PICTURE(buf)  ((buf)[0] == 0 && (buf)[1] == 0 && (buf)[2] == 1 && (buf)[3] == SC_PICTURE)
#  define IS_SC_SEQUENCE(buf) ((buf)[0] == 0 && (buf)[1] == 0 && (buf)[2] == 1 && (buf)[3] == SC_SEQUENCE)
#endif

/* Picture types */
#define NO_PICTURE  0
#define I_FRAME     1
#define P_FRAME     2
#define B_FRAME     3

typedef struct mpeg_rational_s {
  int num;
  int den;
} mpeg_rational_t;

typedef struct video_size_s {
  uint16_t        width;
  uint16_t        height;
  mpeg_rational_t pixel_aspect;
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

/*
 * 
 */
int mpeg2_is_sequence_header(const uint8_t *buf, int len);

#ifdef __cplusplus
} /* extern "C" { */
#endif


#endif
