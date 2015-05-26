/*
 * xine_post_autocrop.c: xine post plugin
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: xine_post_autocrop.c,v 1.52 2011-10-30 16:59:02 phintuka Exp $
 *
 */

/*
 * Copyright (C) 2006 the xine project
 * 
 * This file is part of xine, a free video player.
 * 
 * xine is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * xine is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 *
 * autocrop video filter by Petri Hintukainen 25/03/2006
 *
 * Automatically crop 4:3 letterbox frames to 16:9
 * 
 * based on expand.c
 *
 *
 * TODO: 
 *  - more reliable border detection, including channel logo detection
 *  - OSD re-positioning (?)
 *
 */


#include <stdint.h>

#include <xine/xine_internal.h>
#include <xine/post.h>

#ifndef FABS
#  define FABS(x) ((x) < 0.0 ? -(x) : (x))
#endif

/*
 *  Configuration
 */

/*#define MARK_FRAME       Draw markers on detected boundaries */
/*#define ENABLE_64BIT 1   Force using of 64-bit routines */
/*#undef __MMX__           Disable MMX */
/*#undef __SSE__           Disable SSE */
/*#define FILTER2          Tighter Y-filter */

#undef LOG_MODULE
#define LOG_MODULE      "autocrop"
#define LOG_INFO        (this->debug_level > 0)
#define LOG_DEBUG       (this->debug_level > 1)
#define LOG_TRACE       (this->debug_level > 2)
#define LOG_ACCEL       1

#define DEFAULT_AUTODETECT_RATE          4      /* unit: frames */
#define DEFAULT_STABILIZE_TIME         (5*25)   /* 5 seconds, unit: frames */
#define DEFAULT_SOFT_START_STEP          4      /* unit: lines per frame */
#define DEFAULT_SUBS_DETECT_LIFETIME   (60*25)  /* 1 minute, unit: frames */
#define DEFAULT_SUBS_DETECT_STABILIZE_TIME 12    /* unit: frames */
#define DEFAULT_LOGO_WIDTH              20  /* percentage of frame width */

/* parameters for avards algorithm */
#define DEFAULT_OVERSCAN_COMPENSATE     0   /* percentage of frame height for overscan compensation */
#define DEFAULT_BAR_TONE_TOLERANCE      0   /* 0...255 */

/*
 * Plugin
 */

typedef struct autocrop_parameters_s {
  int    enable_autodetect;
  int    autodetect_rate;
  int    enable_subs_detect;
  int    subs_detect_lifetime;
  int    subs_detect_stabilize_time;
  int    soft_start;
  int    soft_start_step;
  int    stabilize;
  int    stabilize_time;
  int    logo_width;

  int    use_driver_crop;
  int    use_avards_analysis;
  int    overscan_compensate;
  int    bar_tone_tolerance;
  int    debug_level;
} autocrop_parameters_t;

START_PARAM_DESCR(autocrop_parameters_t)
PARAM_ITEM(POST_PARAM_TYPE_BOOL, enable_autodetect, NULL, 0, 1, 0,
  "enable automatic border detecton")
PARAM_ITEM(POST_PARAM_TYPE_INT, autodetect_rate, NULL, 1, 30, 0,
  "rate of automatic letterbox detection")
PARAM_ITEM(POST_PARAM_TYPE_INT, stabilize_time, NULL, 1, 9999, 0,
  "number of frames with equal bars before cropping value changes")
PARAM_ITEM(POST_PARAM_TYPE_BOOL, enable_subs_detect, NULL, 0, 1, 0,
  "enable automatic subtitle detecton")
PARAM_ITEM(POST_PARAM_TYPE_INT, subs_detect_lifetime, NULL, 0, 9999, 0,
  "lifetime of automatic subtitle detection")
PARAM_ITEM(POST_PARAM_TYPE_INT, subs_detect_stabilize_time, NULL, 0, 9999, 0,
  "stabilize time of automatic subtitle detection")
PARAM_ITEM(POST_PARAM_TYPE_BOOL, soft_start, NULL, 0, 1, 0,
  "enable soft start of cropping")
PARAM_ITEM(POST_PARAM_TYPE_INT, soft_start_step, NULL, 1, 999, 0,
  "soft start step width of cropping")
PARAM_ITEM(POST_PARAM_TYPE_BOOL, stabilize, NULL, 0, 1, 0,
  "stabilize cropping to 14:9, 16:9, (16:9+subs), 20:9, (20:9+subs)")
PARAM_ITEM(POST_PARAM_TYPE_INT, logo_width, NULL, 0, 99, 0,
  "maximum width of logo for automatic logo detection (percentage of frame width)")
PARAM_ITEM(POST_PARAM_TYPE_BOOL, use_driver_crop, NULL, 0, 1, 0,
  "use video driver to crop frames (default is to copy frames in post plugin)")
PARAM_ITEM(POST_PARAM_TYPE_BOOL, use_avards_analysis, NULL, 0, 1, 0,
  "use avards algorithm for frame analysis")
PARAM_ITEM(POST_PARAM_TYPE_INT, overscan_compensate, NULL, 0, 100, 0,
  "compensation of output device overscan applied when cropping frames (%1000 of frame height)")
PARAM_ITEM(POST_PARAM_TYPE_INT, bar_tone_tolerance, NULL, 0, 255, 0,
  "tolerance of bar color")
PARAM_ITEM(POST_PARAM_TYPE_INT, debug_level, NULL, 0, 4, 0,
  "debug level")
END_PARAM_DESCR(autocrop_param_descr)


typedef struct autocrop_post_plugin_s
{
  post_plugin_t  post_plugin;

  xine_post_in_t parameter_input;

  /* setup */
  int autodetect;
  int autodetect_rate;
  int subs_detect;
  int subs_detect_lifetime;
  int subs_detect_stabilize_time;
  int soft_start;
  int soft_start_step;
  int stabilize;
  int stabilize_time;
  int logo_width;
  int always_use_driver_crop;
  int use_avards_analysis;
  int overscan_compensate;
  int bar_tone_tolerance;
  int debug_level;

  /* Current cropping status */
  int cropping_active;

  /* Detected bars */
  int detected_end_line;
  int prev_detected_end_line;

  /* Stabilized bars */
  int stabilized_start_line;
  int stabilized_end_line;

  /* Active bars */
  int start_line;
  int end_line;
  int crop_total;

  /* Delayed start for cropping */
  int stabilize_timer;

  /* frame counter (used to select frames for analysis) */
  int analyze_timer;

  /* Last seen frame */
  uint32_t prev_height;
  uint32_t prev_width;
  int64_t  prev_pts;

  /* eliminate jumping when there are subtitles inside bottom bar:
     - when cropping is active and one frame has larger end_line
       than previous, we enlarge frame.
     - after this, cropping is not reseted to previous value unless
       bottom bar has been empty for certain time */
  int height_limit_active;  /* true if detected possible subtitles in bottom area */
  int height_limit;         /* do not crop bottom above this value (bottom of subtitles) */
  int height_limit_timer;   /* counter how many following frames must have black
			       bottom bar until returning to full cropping
			       (used to reset height_limit when there are no subtitles) */
  int end_line_stabilize_timer; /* counter for detecting changes of end line */

  int use_driver_crop;  /* true if non standard frame format (e.g. vdpau) used */
  int has_driver_crop;  /* true if driver has cropping capability */
  int has_unscaled_overlay; /* true if driver has unscaled overlay capability */
  int prev_autodetect_rate;

  pthread_mutex_t crop_lock;

  /* retrieval of standard frame format image */
  vo_frame_t frame;
  uint8_t *img;
  int img_size;

} autocrop_post_plugin_t;


#if (XINE_VERSION_CODE < 10190) && defined(VO_CAP_ARGB_LAYER_OVERLAY)
/* xine-lib-vdpau r134+ */
#  define HAVE_PROC_PROVIDE_STANDARD_FRAME_DATA
#endif
#if XINE_VERSION_CODE >= 10190
/* xine-lib 1.2 r10718+ (2008-12-30) */
#  define HAVE_PROC_PROVIDE_STANDARD_FRAME_DATA
#endif

# if defined(__SSE__)
#  warning Compiling with SSE support
#  include <xmmintrin.h>
# elif defined(__MMX__)
#  warning Compiling with MMX support
#  include <mmintrin.h>
# endif

#if defined(__WORDSIZE)
#  if __WORDSIZE == 64
#    warning Compiling with 64-bit integer support
#    define ENABLE_64BIT (sizeof(int) > 32)
#  endif
#endif

/*
 * Constants
 */ 

#define YNOISEFILTER    (0xE0U)
#define YSHIFTUP        (0x05U)
#define UVBLACK         (0x80U)
#define UVSHIFTUP       (0x03U)
#define UVNOISEFILTER   (0xF8U)

/* YV12 */
#define YNOISEFILTER32  (YNOISEFILTER  * 0x01010101U)
#define YSHIFTUP32      (YSHIFTUP      * 0x01010101U)
#define UVBLACK32       (UVBLACK       * 0x01010101U)
#define UVSHIFTUP32     (UVSHIFTUP     * 0x01010101U)
#define UVNOISEFILTER32 (UVNOISEFILTER * 0x01010101U)

#define YNOISEFILTER64  (YNOISEFILTER  * UINT64_C(0x0101010101010101))
#define YSHIFTUP64      (YSHIFTUP      * UINT64_C(0x0101010101010101))
#define UVBLACK64       (UVBLACK       * UINT64_C(0x0101010101010101))
#define UVSHIFTUP64     (UVSHIFTUP     * UINT64_C(0x0101010101010101))
#define UVNOISEFILTER64 (UVNOISEFILTER * UINT64_C(0x0101010101010101))

/* YUY2 */
/* TODO: should use normal/inverse order based on endianess */
#if 0
#define YUY2BLACK32       (UVBLACK       * 0x00010001U)
#define YUY2SHIFTUP32     (UVSHIFTUP     * 0x00010001U)
#define YUY2NOISEFILTER32 ((YNOISEFILTER * 0x01000100U)|(UVNOISEFILTER * 0x00010001U))
#else
#define YUY2BLACK32       (UVBLACK       * 0x01000100U)
#define YUY2SHIFTUP32     (UVSHIFTUP     * 0x01000100U)
#define YUY2NOISEFILTER32 ((YNOISEFILTER * 0x00010001U)|(UVNOISEFILTER * 0x01000100U))
#endif

#define YUY2BLACK64       (YUY2BLACK32       * UINT64_C(0x0000000100000001))
#define YUY2SHIFTUP64     (YUY2SHIFTUP32     * UINT64_C(0x0000000100000001))
#define YUY2NOISEFILTER64 (YUY2NOISEFILTER32 * UINT64_C(0x0000000100000001))

#ifdef FILTER2
/* tighter Y-filter: original black threshold is 0x1f ; here it is 0x1f - 0x0b = 0x14 */
# define YUY2SHIFTUP32  ((UVSHIFTUP    * 0x00010001U)|(YSHIFTUP      * 0x01000100U))
# undef __SSE__
#endif


/*
 * Black bar detection
 *
 *  Detect black lines with simple noise filtering.
 *  Line is "black" if Y-valus are less than 0x20 and 
 *  U/V values inside range 0x7d...0x84.
 *  ~ 32 first and last pixels are not checked.
 *
 */

static int blank_line_Y_C(uint8_t *data, int length);
static int blank_line_UV_C(uint8_t *data, int length);
static int blank_line_YUY2_C(uint8_t *data, int length);
#if defined(ENABLE_64BIT)
static int blank_line_Y_C64(uint8_t *data, int length);
static int blank_line_UV_C64(uint8_t *data, int length);
static int blank_line_YUY2_C64(uint8_t *data, int length);
#endif
#if defined(__MMX__)
static int blank_line_Y_mmx(uint8_t *data, int length);
static int blank_line_UV_mmx(uint8_t *data, int length);
static int blank_line_YUY2_mmx(uint8_t *data, int length);
#endif
#if defined(__SSE__)
static int blank_line_Y_sse(uint8_t *data, int length);
static int blank_line_UV_sse(uint8_t *data, int length);
static int blank_line_YUY2_sse(uint8_t *data, int length);
#endif

static int blank_line_Y_INIT(uint8_t *data, int length);
static int blank_line_UV_INIT(uint8_t *data, int length);
static int blank_line_YUY2_INIT(uint8_t *data, int length);

static void autocrop_init_mm_accel(void);

int (*blank_line_Y)(uint8_t *data, int length)  = blank_line_Y_INIT;
int (*blank_line_UV)(uint8_t *data, int length) = blank_line_UV_INIT;
int (*blank_line_YUY2)(uint8_t *data, int length) = blank_line_YUY2_INIT;

static int blank_line_Y_C(uint8_t *data, int length)
{
  uint32_t *data32 = (uint32_t*)((((long int)data) + 32 + 3) & (~3)), r = 0;

  length -= 64; /* skip borders (2 x 32 pixels) */
  length /= 4;  /* 4 bytes / loop */

#ifdef FILTER2
  while(length) {
    /* shiftdown needs saturated unsigned element-wise substraction, available only in MMX ...*/
    /* -> use shiftup and looser noise filter for same result. */
    /*    this needs special handling for large values */
    r = r | data32[--length]; /* this catches large values (0xf9 : 0xf9+0x7=0x100 === black) */
    r = r | (data32[length] + YSHIFTUP32); /* this catches small walues (0x1d...0x1f) */
  }
#else
  while(length)
    r = r | data32[--length];
#endif

  return !(r & YNOISEFILTER32);
}

static int blank_line_UV_C(uint8_t *data, int length)
{
  uint32_t *data32 = (uint32_t*)((((long int)data) + 16 + 3) & (~3));
  uint32_t r1 = 0, r2 = 0;

  length -= 32; /* skip borders (2 x 32 pixels, 2 pix/byte) */
  length /= 4;  /* 2 x 4 bytes / loop */

  while(length>0) {
    r1 = r1 | ((data32[--length] + UVSHIFTUP32) ^ UVBLACK32);
    r2 = r2 | ((data32[--length] + UVSHIFTUP32) ^ UVBLACK32);
  }
  return !((r1|r2) & (UVNOISEFILTER32));
}

#if defined(ENABLE_64BIT)
static int blank_line_Y_C64(uint8_t *data, int length)
{
  uint64_t *data64 = (uint64_t*)((((long int)data) + 32 + 7) & (~7)), r = 0;

  length -= 64; /* skip borders (2 x 32 pixels) */
  length /= 8;  /* 8 bytes / loop */

#ifdef FILTER2
  while(length) {
    r = r | data64[--length];
    r = r | (data64[length] + YSHIFTUP64);
  }
#else
  while(length)
    r = r | data64[--length];
#endif

  return !(r & YNOISEFILTER64);
}
#endif

#if defined(ENABLE_64BIT)
static int blank_line_UV_C64(uint8_t *data, int length)
{
  uint64_t *data64 = (uint64_t*)((((long int)data) + 16 + 7) & (~7));
  uint64_t r1 = UINT64_C(0), r2 = UINT64_C(0);

  length -= 32; /* skip borders (2x32 pixels, 2 pix/byte) */
  length /= 8;  /* 2 x 8 bytes / loop */

  while(length>0) {
    r1 = r1 | ((data64[--length] + UVSHIFTUP64) ^ UVBLACK64);
    r2 = r2 | ((data64[--length] + UVSHIFTUP64) ^ UVBLACK64);
  }
  return !((r1|r2) & (UVNOISEFILTER64));
}
#endif


#if defined(__MMX__)
typedef union {
  uint32_t u32[2];
  __m64    m64;
} __attribute__((__aligned__ (8))) __m64_wrapper;
#endif

#if defined(__MMX__)
 int blank_line_Y_mmx(uint8_t *data, int length)
{
#ifdef FILTER2
  static const __m64_wrapper mask   = {{YNOISEFILTER32, YNOISEFILTER32}};
  static const __m64_wrapper gshift = {{YSHIFTUP32,     YSHIFTUP32}};
  register __m64 sum, sum2, shift = gshift.m64, val;
#else
  static const __m64_wrapper mask  = {{YNOISEFILTER32, YNOISEFILTER32}};
  register __m64 sum;
#endif
  __m64 *data64 = (__m64*)(((long int)(data + 32 + 7)) & (~7));

  /*sum = _m_pxor(sum,sum);*/
  __asm__("pxor %0,%0" : "=y"(sum));

  length -= 64; /* skip borders (2 x 32 pixels) */
  length /= 8;  /* 8 bytes / loop */

#ifdef FILTER2
  __asm__("pxor %0,%0" : "=y"(sum2));
  while(length) {
    val  = data64[--length];
    sum  = _m_por(sum, val);
    sum2 = _m_por(sum2, _m_paddb(val, shift));
  }
  sum = _m_por(sum, sum2);
#else
  while(length)
    sum = _m_por(sum, data64[--length]);
#endif

  sum = _m_pand(sum, mask.m64);
  return 0 == _m_to_int(_m_packsswb(sum, sum));
}
#endif

#if defined(__MMX__)
static int blank_line_UV_mmx(uint8_t *data, int length)
{
  static const __m64_wrapper gm_03 = {{UVSHIFTUP32,     UVSHIFTUP32}};
  static const __m64_wrapper gm_f8 = {{UVNOISEFILTER32, UVNOISEFILTER32}};
  static const __m64_wrapper gm_80 = {{UVBLACK32,       UVBLACK32}};
  __m64 *data64 = (__m64*)(((long int)(data) + 16 + 7) & (~7));
  register __m64 sum1, sum2, m_03, /*m_f8,*/ m_80;

  /*sum1 = _m_pxor(sum1, sum1); sum1 = _mm_setzero_si64(); */
  /*sum2 = _m_pxor(sum2, sum2); sum2 = _mm_setzero_si64(); */
  __asm__("pxor %0,%0" : "=y"(sum1));
  __asm__("pxor %0,%0" : "=y"(sum2));

  /* fetch static data to MMX registers */
  m_03 = gm_03.m64;
  /*m_f8 = gm_f8.m64;*/
  m_80 = gm_80.m64;

  length -= 32; /* skip borders (2 x 32 pixels, 2pix/byte) */
  length /= 8;  /* 8 bytes / vector */

  do {
    /* process two 8-byte vectors */
    sum1 = _m_por(sum1,
		  /* grab every byte that is not black (x ^ 0x80 != 0) */
		  _m_pxor(
			  /* filter noise: U/V of each "black" pixel should be 0x7d..0x84 
			     -> each black pixel should be 0x80 after (x+3) & 0xf8 */
			  /*_m_pand(*/
				  /* each black pixel should be 0x80..0x87 after adding 3 */
				  _m_paddb(
					     data64[length-1], 
					     m_03)/*,
				  m_f8)*/,
			  m_80));
    sum2 = _m_por(sum2, 
		  _m_pxor( 
			  /*_m_pand( */
				  _m_paddb( 
					     data64[length-2], 
					     m_03)/*, 
				  m_f8)*/,
			    m_80));
    length -= 2;
  } while(length>0);
  
  /* combine two result vectors (or), filter noise (and) */
  sum1 = _m_pand(_m_por(sum1,
			sum2), 
		 gm_f8.m64);
  /* result vector of black line is 0 */
  return 0 == _m_to_int(_m_packsswb(sum1, sum1));
}
#endif

#if defined(__SSE__)
typedef  union {
  uint32_t u32[4];
  __m128   m128;
} __attribute((__aligned__ (16))) __m128_wrapper;
#endif

#if defined(__SSE__)
static int blank_line_Y_sse(uint8_t *data, int length)
{
  static const __m128_wrapper gmask = {{YNOISEFILTER32, YNOISEFILTER32,
					YNOISEFILTER32, YNOISEFILTER32}};
  __m128 *data128 = (__m128*)(((long int)(data) + 32 + 15) & (~15));
  register __m128 sum1, sum2, zero, mask;

  length -= 64; /* skip borders (2 x 32 pixels) */
  length /= 16; /* 16 bytes / loop */

  /* Start prefetching data to CPU cache */
  _mm_prefetch(data128+length-1, _MM_HINT_NTA);
  _mm_prefetch(data128+length-3, _MM_HINT_NTA);

  /* 
   * Process in two paraller loops, one 16 byte vector / each sub-loop 
   * - grabs bytes with value larger than treshold
   */

  zero = _mm_setzero_ps();
  mask = gmask.m128;
  sum1 = zero;
  sum2 = zero;

  do {
    _mm_prefetch(data128+length-5, _MM_HINT_NTA);
    sum1 = _mm_or_ps(sum1, data128[--length]);
    sum2 = _mm_or_ps(sum2, data128[--length]);
  } while(length>0);

  return 0x0f == _mm_movemask_ps(_mm_cmpeq_ps(_mm_and_ps(_mm_or_ps(sum1, 
								   sum2), 
							 gmask.m128), 
					      _mm_setzero_ps()));
}
#endif

#if defined(__SSE__)
static int blank_line_UV_sse(uint8_t *data, int length)
{
  uint8_t *top = data + length - 1;
  do {
    _mm_prefetch(top,    _MM_HINT_NTA);
    _mm_prefetch(top-32, _MM_HINT_NTA);
    _mm_prefetch(top-64, _MM_HINT_NTA);
    _mm_prefetch(top-72, _MM_HINT_NTA);
    top -= 128;
  } while(top >= data);

  return blank_line_UV_mmx(data, length);
}
#endif

static int blank_line_YUY2_C(uint8_t *data, int length)
{
  uint32_t *data32 = (uint32_t*)((((long int)data) + 64 + 3) & (~3));
  uint32_t r1 = 0, r2 = 0;

  length -= 128; /* skip borders (2 x 32 pixels, 2 bytes/pixel) */
  length /= 4;   /* 2 x 4 bytes / loop */

  while(length) {
    r1 = r1 | ((data32[--length] + YUY2SHIFTUP32) ^ YUY2BLACK32);
    r2 = r2 | ((data32[--length] + YUY2SHIFTUP32) ^ YUY2BLACK32);
  }
  return !((r1|r2) & YUY2NOISEFILTER32);
}

#if defined(ENABLE_64BIT)
static int blank_line_YUY2_C64(uint8_t *data, int length)
{
  uint64_t *data64 = (uint64_t*)((((long int)data) + 64 + 7) & (~7));
  uint64_t r1 = 0, r2 = 0;

  length -= 128; /* skip borders (2 x 32 pixels, 2 bytes/pixel) */
  length /= 8;   /* 2 x 8 bytes / loop */

  while(length) {
    r1 = r1 | ((data64[--length] + YUY2SHIFTUP64) ^ YUY2BLACK64);
    r2 = r2 | ((data64[--length] + YUY2SHIFTUP64) ^ YUY2BLACK64);
  }
  return !((r1|r2) & YUY2NOISEFILTER64);
}
#endif

#if defined(__MMX__)
static int blank_line_YUY2_mmx(uint8_t *data, int length)
{
  /* not implemented */

# if !defined(ENABLE_64BIT)
  return blank_line_YUY2_C(data, length);
# else
  return blank_line_YUY2_C64(data, length);
# endif
}
#endif

#if defined(__SSE__)
static int blank_line_YUY2_sse(uint8_t *data, int length)
{
  uint8_t *top = data + length - 1;
  do {
    _mm_prefetch(top,    _MM_HINT_NTA);
    _mm_prefetch(top-32, _MM_HINT_NTA);
    _mm_prefetch(top-64, _MM_HINT_NTA);
    _mm_prefetch(top-72, _MM_HINT_NTA);
    top -= 128;
  } while(top >= data);

  return blank_line_YUY2_mmx(data, length);
}
#endif

static void autocrop_init_mm_accel(void)
{
  blank_line_Y  = blank_line_Y_C;
  blank_line_UV = blank_line_UV_C;
  blank_line_YUY2 = blank_line_YUY2_C;

#if defined(__SSE__)
  if(xine_mm_accel() & MM_ACCEL_X86_SSE) {
    llprintf(LOG_ACCEL, "autocrop_init_mm_accel: using SSE\n");
    blank_line_Y  = blank_line_Y_sse;
    blank_line_UV = blank_line_UV_sse;
    blank_line_YUY2 = blank_line_YUY2_sse;
    return;
  }
#endif
#if defined(ENABLE_64BIT)
  if(ENABLE_64BIT) {
    llprintf(LOG_ACCEL, "autocrop_init_mm_accel: using 64-bit integer operations\n");
    blank_line_Y  = blank_line_Y_C64;
    blank_line_UV = blank_line_UV_C64;
    blank_line_YUY2 = blank_line_YUY2_C64;
    return;
  }
#endif
#if defined(__MMX__)
  if(xine_mm_accel() & MM_ACCEL_X86_MMX) {
    /* mmx not faster than normal x64 (?) */
    llprintf(LOG_ACCEL, "autocrop_init_mm_accel: using MMX\n");
    blank_line_Y  = blank_line_Y_mmx;
    blank_line_UV = blank_line_UV_mmx;
    blank_line_YUY2 = blank_line_YUY2_mmx;
    return;
  }
#endif
  llprintf(LOG_ACCEL, "autocrop_init_mm_accel: no compatible acceleration methods found\n");
}

static int blank_line_Y_INIT(uint8_t *data, int length)
{
  autocrop_init_mm_accel();
  return (*blank_line_Y)(data, length);
}

static int blank_line_UV_INIT(uint8_t *data, int length)
{
  autocrop_init_mm_accel();
  return (*blank_line_UV)(data, length);
}

static int blank_line_YUY2_INIT(uint8_t *data, int length)
{
  autocrop_init_mm_accel();
  return (*blank_line_YUY2)(data, length);
}

/*
 * Analyze frame
 *  - if frame needs cropping set crop_top & crop_bottom
 */

#ifdef MARK_FRAME
int dbg_top=0, dbg_bottom=0;
#endif

static int analyze_frame_yv12(autocrop_post_plugin_t *this, vo_frame_t *frame,
                              int *crop_top, int *crop_bottom)
{
  int y;
  int ypitch = frame->pitches[0];
  int upitch = frame->pitches[1];
  int vpitch = frame->pitches[2];
  uint8_t *ydata = frame->base[0];
  uint8_t *udata = frame->base[1];
  uint8_t *vdata = frame->base[2];
  int max_crop = (frame->height / 4) / 2; /* 4:3 --> 16:9 */
  int logo_width      = frame->width  * this->logo_width / 100;
  int check_width     = frame->width - logo_width;

  /* from top -> down */
  ydata += 8 * ypitch;  /* skip 8 first lines */
  udata += 4 * upitch;
  vdata += 4 * vpitch;

  for(y = 8; y <= max_crop   *2 /* *2 = 20:9+subs -> 16:9 */ ; y += 2) {
    if(  ! ( blank_line_UV(udata,                  check_width/2) ||
             blank_line_UV(udata+logo_width/2,     check_width/2)    ) ||
         ! ( blank_line_UV(vdata,                  check_width/2) ||
             blank_line_UV(vdata+logo_width/2,     check_width/2)    ) ||
         ! ( blank_line_Y( ydata,                  check_width  ) ||
             blank_line_Y( ydata+logo_width,       check_width  )    ) ||
         ! ( blank_line_Y( ydata+ypitch,           check_width  ) ||
             blank_line_Y( ydata+ypitch+logo_width,check_width  )    )) {
      break;
    } else {
      ydata += 2 * ypitch;
      udata += upitch;
      vdata += vpitch;
    }
  }
  *crop_top = y>8 ? y : 0;

  /* from bottom -> up */
  ydata = frame->base[0] + ((frame->height-4)   -1 ) * ypitch;
  udata = frame->base[1] + ((frame->height-4)/2 -1 ) * upitch;
  vdata = frame->base[2] + ((frame->height-4)/2 -1 ) * vpitch;
  for(y = frame->height - 5; y >= frame->height-max_crop; y -=2 ) {
    if( ! blank_line_Y(ydata,        frame->width) ||
	! blank_line_Y(ydata-ypitch, frame->width) ||  
	! blank_line_UV(udata,       frame->width/2) ||
	! blank_line_UV(vdata,       frame->width/2)) {
      break;
    } else {
      ydata -= 2*ypitch;
      udata -= upitch;
      vdata -= vpitch;
    }
  }
  *crop_bottom = y;

  /* test for black in center - don't crop if frame is empty */
  if(*crop_top >= max_crop*2 && *crop_bottom <= frame->height-max_crop) {
    int center_line = (frame->height >> 2) << 1;
    ydata = frame->base[0] + (center_line  )*ypitch;
    udata = frame->base[1] + (center_line/2)*upitch;
    vdata = frame->base[2] + (center_line/2)*vpitch;
    if( blank_line_Y(ydata,        frame->width) &&
        blank_line_Y(ydata+ypitch, frame->width) &&
	blank_line_UV(udata,       frame->width/2) &&
	blank_line_UV(vdata,       frame->width/2)) {
      llprintf(LOG_DEBUG, "not cropping black frame\n");
      return 0;
    }
  }
  return 1;
}

static int analyze_frame_yuy2(autocrop_post_plugin_t *this, vo_frame_t *frame,
                              int *crop_top, int *crop_bottom)
{
  int y;
  int pitch = frame->pitches[0];
  uint8_t *data = frame->base[0];
  int max_crop = (frame->height / 4) / 2; /* 4:3 --> 16:9 */
  int logo_width      = frame->width  * this->logo_width / 100;
  int check_width     = frame->width - logo_width;

  /* from top -> down */
  data += 6 * pitch;  /* skip 6 first lines */
  for(y = 6; y <= max_crop  *2 /* *2 = 20:9+subs -> 16:9 */ ; y ++)
    if(  ! ( blank_line_YUY2(data,              check_width*2) ||
             blank_line_YUY2(data+2*logo_width, check_width*2)))
      break;
    else 
      data += pitch;

  *crop_top = y;

  /* from bottom -> up */
  data = frame->base[0] + ((frame->height-4)   -1 ) * pitch;
  for(y = frame->height - 5; y >= frame->height-max_crop; y -- )
    if( ! blank_line_YUY2(data,  frame->width * 2))
      break;
    else 
      data -= pitch;
  
  *crop_bottom = y;
  
  /* test for black in center - don't crop if frame is empty */
  if(*crop_top >= max_crop*2 && *crop_bottom <= frame->height-max_crop) {
    data = frame->base[0] + (frame->height/2)*pitch;
    if( blank_line_YUY2(data, frame->width * 2)) {
      llprintf(LOG_DEBUG, "not cropping black frame\n");
      return 0;
    }
  }

  return 1;
}


#define AVARDS_FN(fn_name, pixel_pitch, top_overscan_compensate) \
static int fn_name (autocrop_post_plugin_t *this, vo_frame_t *frame, int *crop_top, int *crop_bottom) \
{ \
  int i, top, bottom, histogram[256]; \
  uint8_t *left, *right; \
  uint8_t *img = frame->base[0]; \
  const int bottom_overscan_compensate = 4; \
  const int ignored_side_width = 16 * pixel_pitch; \
  const int width = frame->width * pixel_pitch; \
  const int top_logo_width = (frame->width * this->logo_width / 100) * pixel_pitch; \
  const int bottom_logo_width = this->subs_detect ? -1: top_logo_width - 1; \
  const int pitch = frame->pitches[0]; \
  const int half_height = frame->height / 2; \
\
  memset(histogram, 0, sizeof(histogram)); \
  left = img + top_overscan_compensate * pitch; \
  right = left + width - ignored_side_width; \
  left += ignored_side_width; \
  while (left < right) { \
    histogram[*left]++; \
    left += pixel_pitch; \
  } \
\
  int bar_tone = 0; \
  for (i = 1; i < 256; ++i) { \
    if (histogram[i] > histogram[bar_tone]) \
      bar_tone = i; \
  } \
  const uint8_t min_bar_tone = (bar_tone > this->bar_tone_tolerance) ? bar_tone - this->bar_tone_tolerance: 0; \
  const uint8_t max_bar_tone = (bar_tone + this->bar_tone_tolerance < 255) ? bar_tone + this->bar_tone_tolerance: 255; \
\
  for (top = top_overscan_compensate; top < half_height; ++top) { \
    left = img + top * pitch; \
    right = left + width - pixel_pitch - ignored_side_width; \
    left += ignored_side_width; \
    while (left <= right && *left >= min_bar_tone && *left <= max_bar_tone) \
      left += pixel_pitch; \
    while (right > left && *right >= min_bar_tone && *right <= max_bar_tone) \
      right -= pixel_pitch; \
    if ((right - left) > top_logo_width) \
      break; \
  } \
\
  for (bottom = frame->height - 1 - bottom_overscan_compensate; bottom > half_height; --bottom) { \
    left = img + bottom * pitch; \
    right = left + width - pixel_pitch - ignored_side_width; \
    left += ignored_side_width; \
    while (left <= right && *left >= min_bar_tone && *left <= max_bar_tone) \
      left += pixel_pitch; \
    while (right > left && *right >= min_bar_tone && *right <= max_bar_tone) \
      right -= pixel_pitch; \
    if ((right - left) > bottom_logo_width) \
      break; \
  } \
\
  *crop_top = top; \
  *crop_bottom = bottom; \
  return ((bottom - top) > 0); \
}

AVARDS_FN(analyze_frame_yv12_avards, 1, 8)
AVARDS_FN(analyze_frame_yuy2_avards, 2, 6)


static void analyze_frame(vo_frame_t *frame, int *crop_top, int *crop_bottom)
{
  post_video_port_t *port = (post_video_port_t *)frame->port;
  autocrop_post_plugin_t *this = (autocrop_post_plugin_t *)port->post;
  int start_line, end_line;

#ifdef HAVE_PROC_PROVIDE_STANDARD_FRAME_DATA
  if ((frame->format != XINE_IMGFMT_YV12 && frame->format != XINE_IMGFMT_YUY2) && frame->proc_provide_standard_frame_data) {
    xine_current_frame_data_t data;
    memset(&data, 0, sizeof(data));
    frame->proc_provide_standard_frame_data(frame->next, &data);
    if (data.img_size > this->img_size) {
      free(this->img);
      this->img = calloc(1, data.img_size);
      if (!this->img) {
        this->img_size = 0;
        return;
      }
      this->img_size = data.img_size;
    }

    data.img = this->img;
    frame->proc_provide_standard_frame_data(frame->next, &data);

    if (data.format == XINE_IMGFMT_YV12) {
      this->frame.pitches[0] = frame->width;
      this->frame.pitches[1] = frame->width / 2;
      this->frame.pitches[2] = frame->width / 2;
      this->frame.base[0] = this->img;
      this->frame.base[1] = this->img + frame->width * frame->height;
      this->frame.base[2] = this->img + frame->width * frame->height + frame->width * frame->height / 4;
    } else if (data.format == XINE_IMGFMT_YUY2) {
      this->frame.pitches[0] = frame->width * 2;
      this->frame.base[0] = this->img;
    }
    this->frame.format = data.format;
    this->frame.height = frame->height;
    this->frame.width = frame->width;

    frame = &this->frame;
  }
#endif

  int result = 0;

  if (this->use_avards_analysis) {
    if (frame->format == XINE_IMGFMT_YV12)
      result = analyze_frame_yv12_avards(this, frame, &start_line, &end_line);
    else if (frame->format == XINE_IMGFMT_YUY2)
      result = analyze_frame_yuy2_avards(this, frame, &start_line, &end_line);
  } else {
    if(frame->format == XINE_IMGFMT_YV12)
      result = analyze_frame_yv12(this, frame, &start_line, &end_line);
    else if(frame->format == XINE_IMGFMT_YUY2)
      result = analyze_frame_yuy2(this, frame, &start_line, &end_line);

#if defined(__MMX__)
    _mm_empty();
#endif
  }

  /* Ignore empty frames */
  if(!result) {
    llprintf(LOG_TRACE, "not cropping black frame\n");
    return;
  }

#ifdef MARK_FRAME
  dbg_top = start_line; dbg_bottom = end_line;
#endif

  if(this->stabilize) {
    int bottom = frame->height - end_line;
    int wide = 0;

    /* bottom bar size */
    if(bottom < frame->height/32) {
      llprintf(LOG_TRACE, "bottom: %d ->  4:3       ", end_line);
      end_line = frame->height - 1;  /* no cropping */
    } else if(bottom < frame->height*3/32) {
      llprintf(LOG_TRACE, "bottom: %d -> 14:9 (%d) ", end_line, frame->height * 15 / 16 - 1);
      end_line = frame->height * 15 / 16 - 1; /* 14:9 */
    } else if(bottom < frame->height*3/16) {
      llprintf(LOG_TRACE, "bottom: %d -> 16:9 (%d) ", end_line, frame->height * 7 / 8 - 1);
      end_line = frame->height * 7 / 8 - 1;   /* 16:9 */
      wide = 1;
    } else {
      llprintf(LOG_TRACE, "bottom: %d -> 20:9 (%d) ", end_line, frame->height * 3 / 4 - 1);
      end_line = frame->height * 3 / 4 - 1;   /* 20:9 */
      wide = 2;
    }

    /* top bar size */
    if(start_line < frame->height/32) {
      llprintf(LOG_TRACE, "top:    %3d ->  4:3      \n", start_line);
      start_line = 0;        /* no cropping */
    } else if(start_line < frame->height*3/32) {
      llprintf(LOG_TRACE, "top:    %3d -> 14:9 (%d)\n", start_line, frame->height / 16);
      start_line = frame->height / 16; /* 14:9 */
    } else if(start_line < frame->height*3/16 || wide) {
      llprintf(LOG_TRACE, "top:    %3d -> 16:9 (%d)\n", start_line, frame->height / 8);
      start_line = frame->height / 8;   /* 16:9 */
    } else { 
      llprintf(LOG_TRACE, "top:    %3d -> 20:9 (%d)\n", start_line, frame->height / 4);
      start_line = frame->height / 4;   /* 20:9 */
      wide++;
    }
    switch(wide) {
    case 3: start_line -= frame->height / 8;
            if(start_line < 0)
              start_line = 0;
            llprintf(LOG_TRACE, "        wide -> center top\n");
    case 2: end_line += frame->height / 8;
            if(end_line >= frame->height)
              end_line = frame->height-1;
            llprintf(LOG_TRACE, "        wide -> center bottom\n");
    default:;
    }

  } else {

    if(start_line > (frame->height/8  *2))  /* *2 --> 20:9 -> 16:9 + subtitles */
      start_line = frame->height/8  *2 ;
    if(end_line < (frame->height*7/8))
      end_line = frame->height*7/8;

    if(start_line > (frame->height/8)) {
      /* if wider than 16:9, prefer cropping top if subtitles are inside bottom bar */
      if(start_line + (frame->height - end_line) > frame->height/4) {
        int diff = start_line + (frame->height - end_line) - frame->height/4;
	diff &= ~1;
        llprintf(LOG_TRACE, "balance: %d,%d -> %d,%d\n",
              start_line, end_line,
              start_line, end_line + diff);
#if 0
        /* this moves image to top (crop only top) */ 
        end_line += diff;
#endif
#if 0
        /* this moves image to center */ 
	/* may cause problems with subtitles ... */
        start_line -= diff;
#endif
#if 1
        /* this moves image to center when there are no 
	   detected subtitles inside bottom bar */
        if(this->cropping_active && this->height_limit_active) {
          int reserved = this->height_limit - end_line;
	  if(reserved>0) {
            end_line += reserved;
	    diff -= reserved;
	  }
	}
        start_line -= diff;
#endif
#if 0
	/* do nothing - image will be centered in video out.
	   - problems with subtitles using unscaled OSD */
#endif
      }
    }

    /* stay inside frame and forget very small bars */
    if(start_line <= 8)
      start_line = 0;
    if(end_line >= (frame->height-6))
      end_line = frame->height;
 
    if(start_line < frame->height/12 || end_line > frame->height*11/12) {
      /* Small bars -> crop only detected borders */
     if(start_line || end_line < frame->height-1) {
       llprintf(LOG_TRACE, "Small bars -> <16:9 : start_line = %d end_line = %d (%s%d t%d)\n",
	      start_line, end_line,
	      this->height_limit_active ? "height limit " : "",
	      this->height_limit,
	      this->height_limit_active ? this->height_limit_timer : 0);
      }
    } else {
      /* Large bars -> crop to 16:9 */
      llprintf(LOG_TRACE, "Large bars -> 16:9  : start_line = %d end_line = %d (%s%d t%d)\n",
	    start_line, end_line,
	    this->height_limit_active ? "height limit " : "",
	    this->height_limit,
	    this->height_limit_active ? this->height_limit_timer : 0);
      if(start_line < frame->height / 8)
        start_line = frame->height / 8;
      if(end_line < frame->height * 7 / 8)
        end_line = frame->height * 7 / 8;
    }
  }

  if (this->overscan_compensate > 0) {
    int h = (frame->height * this->overscan_compensate) / (2*1000);
    if (start_line < h)
      h = start_line;
    if ((frame->height - end_line) < h)
      h = frame->height - end_line;
    start_line -= h;
    end_line += h;
  }

  /* adjust start and stop to even lines */
  (*crop_top)    = (start_line) & (~1);
  (*crop_bottom) = (end_line + 1) & (~1);
}

#ifdef MARK_FRAME
static void mark_frame_yv12(autocrop_post_plugin_t *this,
			    vo_frame_t *frame, int *crop_top, int *crop_bottom)
{
  int ypitch = frame->pitches[0];
  int upitch = frame->pitches[1];
  int vpitch = frame->pitches[2];
  uint8_t *ydata = frame->base[0];
  uint8_t *udata = frame->base[1];
  uint8_t *vdata = frame->base[2];

  /* draw markers to detected boundaries and expected boundaries */
  if(*crop_top > 4 && *crop_top < 200) {
    ydata = frame->base[0] + ((*crop_top)-2)*ypitch;
    udata = frame->base[1] + ((*crop_top)/2 -1)*upitch;
    memset(ydata, 0xff, frame->width/10);
    memset(ydata+ypitch, 0xff, frame->width/10);
    memset(udata, 0xff, frame->width/2/10);
    
    if(dbg_top < *crop_top) dbg_top = *crop_top;
    ydata = frame->base[0] + ((dbg_top - *crop_top))*ypitch;
    udata = frame->base[1] + ((dbg_top - *crop_top)/2)*upitch;
    memset(ydata, 0x80, frame->width/2);
    memset(ydata+ypitch, 0x80, frame->width/2);
    memset(udata, 0xff, frame->width/2/2);
  }
  if(*crop_bottom > 300) {
    if(*crop_bottom < frame->height - 2) {
      ydata = frame->base[0] + (*crop_bottom + 2)*ypitch;
      udata = frame->base[1] + ((*crop_bottom)/2)*upitch;
      memset(ydata, 0xff, frame->width);
      memset(ydata+ypitch, 0xff, frame->width);
      memset(udata, 0xff, frame->width/2);
    }
    if(dbg_bottom - *crop_top - 2 < frame->height) {
      ydata = frame->base[0] + (dbg_bottom - *crop_top - 2)*ypitch;
      udata = frame->base[1] + (dbg_bottom - *crop_top - 2)/2*upitch;
      memset(ydata, 0x80, frame->width/2);
      memset(ydata+ypitch, 0xff, frame->width/2);
      memset(udata, 0xff, frame->width/2/2);
    }
  }
  if(frame->height > 500) {
    /* TODO: use frame height instead of assuming 576 ... -> 72 */
    vdata = frame->base[2] + ((72-*crop_top)/2)*vpitch;
vdata = frame->base[2] + (72/2)*vpitch;
    memset(vdata, 0xff, frame->width/2);
    vdata = frame->base[2] + ((frame->height-72+(576-*crop_bottom))/2)*vpitch;
vdata = frame->base[2] + ((frame->height-72)/2)*vpitch;
    memset(vdata, 0xff, frame->width/2);
  }
}
#endif

/* 
 * crop frame by copying 
 */

static int crop_copy_yv12(vo_frame_t *frame, xine_stream_t *stream)
{
  post_video_port_t *port = (post_video_port_t *)frame->port;
  autocrop_post_plugin_t *this = (autocrop_post_plugin_t *)port->post;
  vo_frame_t *new_frame;
    
  int y, result;
  int yp = frame->pitches[0], yp2;
  int up = frame->pitches[1], up2;
  int vp = frame->pitches[2], vp2;
  uint8_t *ydata = frame->base[0], *ydata2;
  uint8_t *udata = frame->base[1], *udata2;
  uint8_t *vdata = frame->base[2], *vdata2;

  int   new_height;
  double new_ratio;

  /* top bar */
  ydata += this->start_line * yp;
  udata += (this->start_line/2) * up;
  vdata += (this->start_line/2) * vp;

  new_height = this->end_line - this->start_line;
  new_ratio  = 12.0/9.0 * ((double)frame->height / (double)new_height);

  new_frame = port->original_port->get_frame(port->original_port,
					     frame->width, new_height, 
					     new_ratio, frame->format, 
					     frame->flags | VO_BOTH_FIELDS);

  /* ??? */
  frame->ratio = new_frame->ratio;

  _x_post_frame_copy_down(frame, new_frame);

  yp2 = new_frame->pitches[0];
  up2 = new_frame->pitches[1];
  vp2 = new_frame->pitches[2];
  ydata2 = new_frame->base[0];
  udata2 = new_frame->base[1];
  vdata2 = new_frame->base[2];

  /*
    TODO:
    save channel logo mask (from top)
    - no changes in 3 sec -> next time crop it out ...
  */

  for(y=0; y < new_height/2; y++) {
    xine_fast_memcpy(ydata2, ydata, frame->width);
    ydata += yp;
    ydata2 += yp2;
    xine_fast_memcpy(ydata2, ydata, frame->width);
    ydata += yp;
    ydata2 += yp2;
    xine_fast_memcpy(udata2, udata, frame->width/2);
    udata += up;
    udata2 += up2;
    xine_fast_memcpy(vdata2, vdata, frame->width/2);
    vdata += vp;
    vdata2 += vp2;
  }

#ifdef MARK_FRAME
  mark_frame_yv12(this, new_frame, &this->start_line, &this->end_line);
#endif

  result = new_frame->draw(new_frame, stream);
  _x_post_frame_copy_up(frame, new_frame);
  new_frame->free(new_frame);
  
  return result;
}

static int crop_copy_yuy2(vo_frame_t *frame, xine_stream_t *stream)
{
  post_video_port_t *port = (post_video_port_t *)frame->port;
  autocrop_post_plugin_t *this = (autocrop_post_plugin_t *)port->post;
  vo_frame_t *new_frame;
    
  int y, result;
  int p = frame->pitches[0], p2;
  uint8_t *data = frame->base[0], *data2;

  int   new_height;
  double new_ratio;

  /* top bar */
  data += this->start_line * p;

  new_height = this->end_line - this->start_line;
  new_ratio = 12.0/9.0 * ((double)frame->height / (double)new_height);
  new_frame = port->original_port->get_frame(port->original_port,
					     frame->width, new_height, 
					     new_ratio, frame->format, 
					     frame->flags | VO_BOTH_FIELDS);
  /* ??? */
  frame->ratio = new_frame->ratio;

  _x_post_frame_copy_down(frame, new_frame);

  p2 = new_frame->pitches[0];
  data2 = new_frame->base[0];

  for(y=0; y < new_height; y++) {
    xine_fast_memcpy(data2, data, frame->width);
    data += p;
    data2 += p2;
  }

  result = new_frame->draw(new_frame, stream);
  _x_post_frame_copy_up(frame, new_frame);
  new_frame->free(new_frame);
  
  return result;
}


/*
 *    Frame handling
 */

static int autocrop_draw(vo_frame_t *frame, xine_stream_t *stream)
{
  post_video_port_t *port = (post_video_port_t *)frame->port;
  autocrop_post_plugin_t *this = (autocrop_post_plugin_t *)port->post;
  int result, start_line, end_line;

  if(!this->autodetect) {

    pthread_mutex_lock(&this->crop_lock);
    this->cropping_active = 1;
    this->start_line = frame->height/8;
    this->end_line   = frame->height*7/8;
    this->crop_total = frame->height/4;
    this->use_driver_crop = this->always_use_driver_crop || (frame->format != XINE_IMGFMT_YV12 && frame->format != XINE_IMGFMT_YUY2);
    pthread_mutex_unlock(&this->crop_lock);

    if (frame->bad_frame || this->use_driver_crop) {
      _x_post_frame_copy_down(frame, frame->next);
      result = frame->next->draw(frame->next, stream);
      _x_post_frame_copy_up(frame, frame->next);
    } else if(frame->format == XINE_IMGFMT_YV12)
      result = crop_copy_yv12(frame, stream);
    else /*if(frame->format == XINE_IMGFMT_YUY2)*/
      result = crop_copy_yuy2(frame, stream);

    return result;
  }

  int autodetect_rate = this->autodetect_rate;
  int cropping_active = this->cropping_active;

  /* use pts jumps to track stream changes (and seeks) */
  if(cropping_active && frame->pts > 0) {
    if(this->prev_pts>0) {
      int64_t dpts = frame->pts - this->prev_pts;
      if(dpts < INT64_C(-30*90000) || dpts > INT64_C(30*90000)) { /* 30 sec */
	if(this->height_limit_active) {
          this->height_limit_timer = this->subs_detect_lifetime / 2;
          llprintf(LOG_DEBUG, "short pts jump reseted height limit\n");
	}
      }
      if(dpts < INT64_C(-30*60*90000) || dpts > INT64_C(30*60*90000)) { /* 30 min */ 
        cropping_active = 0;
        llprintf(LOG_DEBUG, "long pts jump reseted cropping\n");
      }
    }
    this->prev_pts = frame->pts;
  }

  if (cropping_active) {
    start_line = this->stabilized_start_line;
    end_line = this->stabilized_end_line;
  } else {
    start_line = 0;
    end_line = frame->height;
  }

  /* Analyze frame for letterbox borders */
  if(!frame->bad_frame && (this->analyze_timer % autodetect_rate) == 0) {
    analyze_frame(frame, &start_line, &end_line);
    int detected_end_line = end_line;

    /* activate cropping if bars are large enough */
    if(!cropping_active && (start_line > 10 || end_line < (frame->height - 10))) {
      cropping_active = 1;
      this->stabilized_start_line = this->start_line = 0;
      this->stabilized_end_line = this->end_line = this->detected_end_line = this->prev_detected_end_line = frame->height;
      this->stabilize_timer = this->stabilize_time;
      this->prev_pts = -1;
      this->height_limit_active = 0;
      this->end_line_stabilize_timer = this->subs_detect_stabilize_time;
    }

    if(cropping_active && this->subs_detect) {
      /* no change unless different values for several frames */
      if (abs(this->detected_end_line - end_line) > 5) {
        this->end_line_stabilize_timer -= autodetect_rate;
        if (this->end_line_stabilize_timer <= 0) {
          this->detected_end_line = end_line;
          this->end_line_stabilize_timer = this->subs_detect_stabilize_time;
        }
      } else
        this->end_line_stabilize_timer = this->subs_detect_stabilize_time;

      if(this->height_limit_active) {
        this->height_limit_timer -= autodetect_rate;
        if (this->height_limit_timer <= 0) {
          this->height_limit_active = 0;
          llprintf(LOG_DEBUG, "height limit timer expired\n");
        }
      }
      /* apply height limit */
      if(this->height_limit_active && end_line < this->height_limit)
        end_line = this->height_limit;
    }

    /* no change unless different values for several frames */
    if(abs(start_line - this->stabilized_start_line) > 5 || abs(end_line - this->stabilized_end_line) > 5) {
      this->stabilize_timer -= autodetect_rate;
      if(this->stabilize_timer > 0) {
        start_line = this->stabilized_start_line;
        end_line = this->stabilized_end_line;
      }
      else {
        /* ignore very small bars */
        if(start_line <= 10 && end_line >= (frame->height - 10))
          cropping_active = 0;
        this->stabilize_timer = this->stabilize_time;
        llprintf(LOG_DEBUG, "stabilized start: %d -> %d, end %d -> %d\n",
              this->stabilized_start_line, start_line,
              this->stabilized_end_line, end_line);
      }
    } else {
      this->stabilize_timer = this->stabilize_time;
      start_line = this->stabilized_start_line;
      end_line = this->stabilized_end_line;
    }

    /* handle fixed subtitles inside bottom bar */
    if(cropping_active && this->subs_detect) {
      if(this->height_limit_active && abs(this->stabilized_start_line - start_line) > 5) {
        /* reset height limit if top bar changes */
        this->height_limit_active = 0;
        end_line = this->detected_end_line = detected_end_line;
        llprintf(LOG_DEBUG, "height limit reset, top bar moved from %d -> %d, bottom now %d\n", this->stabilized_start_line, start_line, end_line);

      } else if (this->detected_end_line > (this->prev_detected_end_line + 5)) {
        if(!this->height_limit_active || this->height_limit < this->detected_end_line) {
          /* start or increase height limit */
          if (this->height_limit_active)
            llprintf(LOG_DEBUG, "height limit %d -> %d, prev bottom %d\n", this->height_limit, this->detected_end_line, this->prev_detected_end_line);
          else
            llprintf(LOG_DEBUG, "activate height limit %d, prev bottom %d\n", this->detected_end_line, this->prev_detected_end_line);
          this->height_limit = this->detected_end_line;
          this->height_limit_timer = this->subs_detect_lifetime;
          this->height_limit_active = 1;

        } else if(this->height_limit_active && this->height_limit_timer < (this->subs_detect_lifetime / 4)) {
          /* keep height limit timer running */
          this->height_limit_timer = this->subs_detect_lifetime / 2;
          llprintf(LOG_DEBUG, "height_limit_timer increment bottom %d;%d -> %d\n", this->prev_detected_end_line, this->detected_end_line, detected_end_line);
        }
      }
      this->prev_detected_end_line = this->detected_end_line;
    }

    this->stabilized_start_line = start_line;
    this->stabilized_end_line = end_line;
  }

  /* update frame counter */
  ++this->analyze_timer;

  /* "soft start" */
  if(cropping_active && this->soft_start) {
    if(this->start_line != start_line) {
      int diff = this->start_line - start_line;
      if(diff < -this->soft_start_step)
        diff = -this->soft_start_step;
      else if(diff > this->soft_start_step)
        diff = this->soft_start_step;
      start_line = this->start_line - diff;
    }
    if(this->end_line != end_line) {
      int diff = this->end_line - end_line;
      if(diff < -this->soft_start_step)
        diff = -this->soft_start_step;
      else if(diff > this->soft_start_step)
        diff = this->soft_start_step;
      end_line = this->end_line - diff;
    }
  }

  this->prev_height = frame->height;
  this->prev_width = frame->width;

  if (cropping_active && (start_line != this->start_line || end_line != this->end_line)) {
    llprintf(LOG_DEBUG, "active start: %d -> %d, end %d -> %d\n",
          this->start_line, start_line,
          this->end_line, end_line);
  }

  if (this->cropping_active != cropping_active)
    llprintf(LOG_DEBUG, "draw: active %d -> %d\n", this->cropping_active, cropping_active);

  pthread_mutex_lock(&this->crop_lock);
  this->cropping_active = cropping_active;
  this->start_line = start_line;
  this->end_line = end_line;
  this->crop_total = start_line + frame->height - end_line;
  this->use_driver_crop = this->always_use_driver_crop || (frame->format != XINE_IMGFMT_YV12 && frame->format != XINE_IMGFMT_YUY2);
  pthread_mutex_unlock(&this->crop_lock);

  /*
   * do cropping 
   *  - using frame->crop_... does not seem to work with my xv and xine-lib-1.1.1. 
   *  - maybe cropping could be done by adjusting y/u/v data pointers 
   *    and height of frame ?
   *    -> no time-consuming copying 
   */
  if(frame->bad_frame || !cropping_active || this->use_driver_crop) {
    _x_post_frame_copy_down(frame, frame->next);
    result = frame->next->draw(frame->next, stream);
    _x_post_frame_copy_up(frame, frame->next);
  } else if(frame->format == XINE_IMGFMT_YV12)
    result = crop_copy_yv12(frame, stream);
  else /*if(frame->format == XINE_IMGFMT_YUY2)*/
    result = crop_copy_yuy2(frame, stream);

  return result;
}

static vo_frame_t *autocrop_get_frame(xine_video_port_t *port_gen, 
				       uint32_t width, uint32_t height, 
				       double ratio, int format, int flags)
{
  post_video_port_t      *port = (post_video_port_t *)port_gen;
  post_plugin_t          *this_gen = port->post;
  autocrop_post_plugin_t *this = (autocrop_post_plugin_t *)this_gen;

  int cropping_active = this->cropping_active;

  if (ratio <= 0.0) {
    if (height > 1)
      ratio = (double)width / (double)height;
  }

  /* Crop only SDTV 4:3 frames ... */
  int intercept = ((format == XINE_IMGFMT_YV12 || format == XINE_IMGFMT_YUY2 || this->has_driver_crop) &&
       FABS(ratio - 4.0/3.0) < 0.1 &&
       width  >= 240 && width  <= 768 &&
       height >= 288 && height <= 576);

  if(cropping_active && !intercept) {
    cropping_active = 0;
    llprintf(LOG_DEBUG, "get_frame: deactivate ratio: %lf width: %d height: %d\n", ratio, width, height);
  }

  /* reset when format changes */
  if (cropping_active && this->autodetect && (height != this->prev_height || width != this->prev_width)) {
    cropping_active = 0;
    llprintf(LOG_DEBUG, "get_frame: deactivate width %d -> %d height %d -> %d\n", this->prev_width, width, this->prev_height, height);
  }

  /* set new ratio when using driver crop */
  if (cropping_active && this->use_driver_crop) {
    if (this->autodetect) {
      int new_height = this->end_line - this->start_line;
      if (new_height > 1 && new_height != (int)height)
        ratio *= (double)height / (double)new_height;
    } else {
      ratio *= 4.0 / 3.0;
    }
  }

  _x_post_rewire(this_gen);
  vo_frame_t *frame = port->original_port->get_frame(port->original_port, width, height, ratio, format, flags);

  if (frame) {
    /* set cropping when using driver crop */
    if (cropping_active && this->use_driver_crop) {
      if (this->autodetect) {
        frame->crop_top = this->start_line;
        frame->crop_bottom = (height - this->end_line);
      } else {
        frame->crop_top = frame->crop_bottom = height / 8;
      }
    }

    /* intercept frame for analysis and crop-by-copy */
    if (intercept && format != XINE_IMGFMT_YV12 && format != XINE_IMGFMT_YUY2) {
#ifdef HAVE_PROC_PROVIDE_STANDARD_FRAME_DATA
      if (!frame->proc_provide_standard_frame_data) {
#endif
      llprintf(LOG_DEBUG, "get_frame: deactivate because missing provide_standard_frame_data feature\n");
      cropping_active = 0;
      intercept = 0;
#ifdef HAVE_PROC_PROVIDE_STANDARD_FRAME_DATA
    }
#endif
    }

    if (intercept) {
      _x_post_inc_usage(port);
      frame = _x_post_intercept_video_frame(frame, port);
    }
  }

  this->cropping_active = cropping_active;

  return frame;
}

static int autocrop_intercept_ovl(post_video_port_t *port)
{
  autocrop_post_plugin_t *this = (autocrop_post_plugin_t *)port->post;

  if (!this->cropping_active) 
    return 0;

  return 1;
}

static int32_t autocrop_overlay_add_event(video_overlay_manager_t *this_gen, void *event_gen)
{
  post_video_port_t      *port  = _x_post_ovl_manager_to_port(this_gen);
  autocrop_post_plugin_t *this  = (autocrop_post_plugin_t *)port->post;
  video_overlay_event_t  *event = (video_overlay_event_t *)event_gen;

  pthread_mutex_lock(&this->crop_lock);
  int cropping_active = this->cropping_active;
  int crop_total = this->crop_total;
  int use_driver_crop = this->use_driver_crop;
  int start_line = this->start_line;
  pthread_mutex_unlock(&this->crop_lock);

  if(cropping_active && crop_total>10) {
    if (event->event_type == OVERLAY_EVENT_SHOW
#ifdef VO_CAP_CUSTOM_EXTENT_OVERLAY
        /* Do not move overlay if video_out has independent video and OSD resolutions */
        &&   event->object.overlay
        && ( event->object.overlay->extent_width  <= 0 ||
             event->object.overlay->extent_height <= 0)
#endif
        ) {
      switch (event->object.object_type) {
      case 0:
	/* regular subtitle */
	/* Subtitle overlays must be coming somewhere inside xine engine */
        if (use_driver_crop) {
          if(this->has_driver_crop) {
            if(!event->object.overlay->unscaled || !this->has_unscaled_overlay) {
              event->object.overlay->y -= crop_total;
	  }
	} else {
	  /* object is moved crop_top amount in video_out */
            if(event->object.overlay->unscaled && this->has_unscaled_overlay) {
	    /* cancel incorrect move that will be done in video_out */
              event->object.overlay->y += start_line;
	  } else {
	    /* move crop_bottom pixels up */
              event->object.overlay->y -= (crop_total - start_line);
	  }
	}

	/* when using cropping overlays are moved in video_out */
	llprintf(LOG_INFO, "autocrop_overlay_add_event: subtitle event untouched\n");
        } else {
	/* when cropping here subtitles coming from inside of xine must be re-positioned */
          if(!event->object.overlay->unscaled || !this->has_unscaled_overlay) {
            event->object.overlay->y -= crop_total;
	  llprintf(LOG_INFO, "autocrop_overlay_add_event: subtitle event moved up\n");
	}
        }
	break;
      case 1:
	/* menu overlay */
	/* All overlays coming from VDR have this type */
	{
#ifdef DVDTEST
	  int dvd_menu = 0;
	  if (stream->input_plugin) {
	    if (stream->input_plugin->get_capabilities (stream->input_plugin) & 
		INPUT_CAP_SPULANG) {
	      *((int *)lang) = 0; /* channel */
	      if (stream->input_plugin->get_optional_data (stream->input_plugin, lang,
							   INPUT_OPTIONAL_DATA_SPULANG)
		  == INPUT_OPTIONAL_SUCCESS)
		if(!strcmp(lang, "menu"))
		  dvd_menu = 1; /* -> cropping off */
	      /* should turn on when not in menu ... -> where ? */
	    }
	  }
#endif
          if (use_driver_crop) {
            if(!event->object.overlay->unscaled || !this->has_unscaled_overlay) {
              event->object.overlay->y += start_line;//crop_total;
            }
	  }
	}
	break;
      default:;
      }
    }
  }

  return port->original_manager->add_event(port->original_manager, event_gen);
}


static void autocrop_video_close(xine_video_port_t *port_gen, xine_stream_t *stream)
{
  post_video_port_t *port = (post_video_port_t *)port_gen;
  autocrop_post_plugin_t *this = (autocrop_post_plugin_t *) port->post;

  if (this->cropping_active) {
    this->cropping_active = 0;
    llprintf(LOG_DEBUG, "deactivate because video close\n");
  }
  port->original_port->close(port->original_port, stream);
  port->stream = NULL;
  _x_post_dec_usage(port);
}


/*
 *    Parameter functions
 */

static xine_post_api_descr_t *autocrop_get_param_descr(void)
{
  return &autocrop_param_descr;
}

static int autocrop_set_parameters(xine_post_t *this_gen, void *param_gen)
{
  autocrop_post_plugin_t *this = (autocrop_post_plugin_t *)this_gen;
  autocrop_parameters_t *param = (autocrop_parameters_t *)param_gen;

  this->autodetect  = param->enable_autodetect;
  this->autodetect_rate = param->autodetect_rate;
  this->subs_detect = param->enable_subs_detect;
  this->subs_detect_lifetime = param->subs_detect_lifetime;
  this->subs_detect_stabilize_time = param->subs_detect_stabilize_time;
  this->soft_start  = param->soft_start;
  this->soft_start_step = param->soft_start_step;
  this->stabilize   = param->stabilize;
  this->stabilize_time = param->stabilize_time;
  this->always_use_driver_crop = param->use_driver_crop && this->has_driver_crop;
  this->logo_width = param->logo_width;
  this->use_avards_analysis = param->use_avards_analysis;
  this->overscan_compensate = param->overscan_compensate;
  this->bar_tone_tolerance = param->bar_tone_tolerance;
  this->debug_level = param->debug_level;

  llprintf(LOG_DEBUG, "autocrop_set_parameters: "
        "autodetect=%d  autodetect_rate=%d  logo_width=%d  "
        "subs_detect=%d  subs_detect_lifetime=%d  subs_detect_stabilize_time=%d  "
        "soft_start=%d  soft_start_step=%d  "
        "stabilize=%d  stabilize_time=%d  use_driver_crop=%d  "
        "use_avards_analysis=%d  overscan_compensate=%d  bar_tone_tolerance=%d\n",
        this->autodetect, this->autodetect_rate, this->logo_width,
        this->subs_detect, this->subs_detect_lifetime, this->subs_detect_stabilize_time,
	this->soft_start, this->soft_start_step,
        this->stabilize, this->stabilize_time, this->always_use_driver_crop,
        this->use_avards_analysis, this->overscan_compensate, this->bar_tone_tolerance);
  return 1;
}

static int autocrop_get_parameters(xine_post_t *this_gen, void *param_gen)
{
  autocrop_post_plugin_t *this = (autocrop_post_plugin_t *)this_gen;
  autocrop_parameters_t *param = (autocrop_parameters_t *)param_gen;
  
  param->enable_autodetect  = this->autodetect;
  param->autodetect_rate  = this->autodetect_rate;
  param->enable_subs_detect = this->subs_detect;
  param->subs_detect_lifetime = this->subs_detect_lifetime;
  param->subs_detect_stabilize_time = this->subs_detect_stabilize_time;
  param->soft_start         = this->soft_start;
  param->soft_start_step = this->soft_start_step;
  param->stabilize          = this->stabilize;
  param->stabilize_time = this->stabilize_time;
  param->use_driver_crop    = this->always_use_driver_crop;
  param->logo_width = this->logo_width;
  param->use_avards_analysis = this->use_avards_analysis;
  param->overscan_compensate = this->overscan_compensate;
  param->bar_tone_tolerance = this->bar_tone_tolerance;
  param->debug_level = this->debug_level;

  llprintf(LOG_DEBUG, "autocrop_get_parameters: "
        "autodetect=%d  autodetect_rate=%d  logo_width=%d  "
        "subs_detect=%d  subs_detect_lifetime=%d  subs_detect_stabilize_time=%d  "
        "soft_start=%d  soft_start_step=%d  "
        "stabilize=%d  stabilize_time=%d  use_driver_crop=%d  "
        "use_avards_analysis=%d  overscan_compensate=%d  bar_tone_tolerance=%d\n",
        this->autodetect, this->autodetect_rate, this->logo_width,
        this->subs_detect, this->subs_detect_lifetime, this->subs_detect_stabilize_time,
	this->soft_start, this->soft_start_step,
        this->stabilize, this->stabilize_time, this->always_use_driver_crop,
        this->use_avards_analysis, this->overscan_compensate, this->bar_tone_tolerance);

  return 1;
}

static char *autocrop_get_help(void) {
  return _("The autocrop plugin is meant to take 4:3 letterboxed frames and "
           "convert them to 16:9 by removing black bars on the top and bottom "
	   "of the frame.\n"
           "\n"
           "Parameters\n"
           "  enable_autodetect:          Enable automatic letterbox detection\n"
           "  autodetect_rate:            Rate of automatic letterbox detection\n"
           "  stabilize_time:             Number of frames with equal bars before cropping value changes\n"
           "  enable_subs_detect:         Enable automatic subtitle detection inside bottom bar\n"
           "  subs_detect_lifetime:       Lifetime of automatic subtitle detection\n"
           "  subs_detect_stabilize_time: Stabilize time of automatic subtitle detection\n"
           "  soft_start:                 Enable soft start of cropping\n"
           "  soft_start_step:            Soft start step width of cropping\n"
           "  stabilize:                  Stabilize cropping to 14:9, 16:9, (16:9+subs), 20:9, (20:9+subs)\n"
           "  use_driver_crop:            Always use video driver crop\n"
           "  logo_width:                 Width of logo (percentage of frame width) for automatic logo detection\n"
           "  use_avards_analysis:        Use AVARDS algorithm for frame analysis\n"
           "  overscan_compensate:        Compensation of output device overscan applied when cropping frames (%1000 of frame height)\n"
           "  bar_tone_tolerance:         Tolerance of bar color (avards only)"
           "  debug_level:                Output debug messages. 0=none, 1=info, 2=debug, 3=trace.\n"
           "\n"
         );
}


/*
 *    Open/close
 */

static void autocrop_dispose(post_plugin_t *this_gen)
{
  if (_x_post_dispose(this_gen)) {
    autocrop_post_plugin_t *this = (autocrop_post_plugin_t *) this_gen;
    pthread_mutex_destroy(&this->crop_lock);
    free(this->img);
    free(this);
  }
}

static post_plugin_t *autocrop_open_plugin(post_class_t *class_gen, 
					    int inputs,
					    xine_audio_port_t **audio_target,
					    xine_video_port_t **video_target)
{
  if (video_target && video_target[ 0 ]) {
    autocrop_post_plugin_t *this = calloc(1, sizeof(autocrop_post_plugin_t));
    post_in_t           *input;
    post_out_t          *output;
    post_video_port_t   *port;
    xine_post_in_t      *input_param;

    static xine_post_api_t post_api =
      { autocrop_set_parameters, autocrop_get_parameters, 
	autocrop_get_param_descr, autocrop_get_help };

    if (this) {
      _x_post_init(&this->post_plugin, 0, 1);

      port = _x_post_intercept_video_port(&this->post_plugin, 
					  video_target[ 0 ], 
					  &input, &output);

      input->xine_in.name   = "video in";
      output->xine_out.name = "video out";

      port->intercept_ovl          = autocrop_intercept_ovl;
      port->new_manager->add_event = autocrop_overlay_add_event;
      port->new_port.get_frame     = autocrop_get_frame;
      port->new_port.close         = autocrop_video_close;
      port->new_frame->draw        = autocrop_draw;

      this->post_plugin.xine_post.video_input[ 0 ] = &port->new_port;
      this->post_plugin.dispose = autocrop_dispose;

      input_param       = &this->parameter_input;
      input_param->name = "parameters";
      input_param->type = XINE_POST_DATA_PARAMETERS;
      input_param->data = &post_api;
#if XINE_VERSION_CODE >= 10102
      xine_list_push_back(this->post_plugin.input, input_param);
#else
      xine_list_append_content(this->post_plugin.input, input_param);
#endif
      this->autodetect  = 1;
      this->autodetect_rate = DEFAULT_AUTODETECT_RATE;
      this->stabilize_time = DEFAULT_STABILIZE_TIME;
      this->subs_detect = 1;
      this->subs_detect_lifetime = DEFAULT_SUBS_DETECT_LIFETIME;
      this->subs_detect_stabilize_time = DEFAULT_SUBS_DETECT_STABILIZE_TIME;
      this->soft_start  = 1;
      this->soft_start_step = DEFAULT_SOFT_START_STEP;
      this->stabilize   = 1;
      this->logo_width = DEFAULT_LOGO_WIDTH;
      this->use_avards_analysis = 0;
      this->overscan_compensate = DEFAULT_OVERSCAN_COMPENSATE;
      this->bar_tone_tolerance = DEFAULT_BAR_TONE_TOLERANCE;
      this->debug_level = 1;

      uint64_t caps = port->original_port->get_capabilities(port->original_port);
      this->has_driver_crop = caps & VO_CAP_CROP;
      this->has_unscaled_overlay = caps & VO_CAP_UNSCALED_OVERLAY;

      pthread_mutex_init(&this->crop_lock, NULL);

      return &this->post_plugin;
    }
  }

  return NULL;
}


/*
 *    Plugin class
 */

#if POST_PLUGIN_IFACE_VERSION < 10
static char *autocrop_get_identifier(post_class_t *class_gen)
{
  return "autocrop";
}

static char *autocrop_get_description(post_class_t *class_gen)
{
  return "Crop letterboxed 4:3 video to 16:9";
}

static void autocrop_class_dispose(post_class_t *class_gen)
{
  free(class_gen);
}
#endif

static void *autocrop_init_plugin(xine_t *xine, void *data)
{
  post_class_t *class = calloc(1, sizeof(post_class_t));
  
  if(class) {
    class->open_plugin     = autocrop_open_plugin;
#if POST_PLUGIN_IFACE_VERSION < 10
    class->get_identifier  = autocrop_get_identifier;
    class->get_description = autocrop_get_description;
    class->dispose         = autocrop_class_dispose;
#else
    class->identifier      = "autocrop";
    class->description     = N_("Crop letterboxed 4:3 video to 16:9");
    class->dispose         = default_post_class_dispose;
#endif
  }

  return class;
}


static post_info_t info = { XINE_POST_TYPE_VIDEO_FILTER };

const plugin_info_t xine_plugin_info[] __attribute__((visibility("default"))) =
{
  /* type, API, "name", version, special_info, init_function */  
  { PLUGIN_POST, POST_PLUGIN_IFACE_VERSION, "autocrop", XINE_VERSION_CODE, &info, &autocrop_init_plugin },
  { PLUGIN_NONE, 0, "", 0, NULL, NULL }
};
