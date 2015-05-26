/*
 * Copyright (C) 2000-2007 the xine project
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
 * $Id: xine_post_swscale.c,v 1.16 2011-10-30 16:59:02 phintuka Exp $
 *
 * Simple (faster) resize for avisynth
 *     Copyright (C) 2002 Tom Barry
 *
 * Very simple 2 tap linear interpolation.  
 * It is unfiltered which means it will not soften much.
 *
 * WarpedResize will do a non-linear stretch/squeeze in both the horizontal 
 * and vertical dimensions. This can be useful when you want to change the 
 * aspect ratio of a video clip and have it mostly distorted at the
 * top, bottom, and side edges. 
 *
 *
 * Ported to linux/xine by Petri Hintukainen <phintuka@users.sourceforge.net>
 *    - Added x86_64 support
 *    - Added PIC support (do not clobber ebx in x86, access only local variables from asm)
 *    - Fixed yv12 stretched warp tables generation
 */

#include <xine/xine_internal.h>
#include <xine/post.h>
#include <float.h>  /* DBL_MIN */

/*
 * tools
 */

#ifndef ALIGN
#  define ALIGN(b,p) ((void*)((((unsigned long)(p)) + (b)-1) & (~((b)-1))))
#endif
#ifndef MIN
#  define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif
#ifndef MAX
#  define MAX(a,b) ((a) > (b) ? (a) : (b))
#endif
#ifndef FABS
#  define FABS(x) ((x) < 0.0 ? -(x) : (x))
#endif

static int doublecmp(double a, double b) {
  if      (FABS(a-b) <= 2*DBL_MIN) return 0; // equal
  else if (a > b) return 1;
  else            return -1;
}


/*#define DBG(x...)*/
#define DBG(x...) do { if(this->config.debug) fprintf(stderr, "post_warp: " x); } while (0)

/*#define STREAMING_STORE_TMP*/
/*#define STREAMING_STORE*/
/*#define PREFETCH*/
/* streaming store and prefetch seems to be slower ...
 * Tested with P3 (128M L2) and C2D (4M L2).
 * Maybe access pattern is enough simple for HW prefetchers.
 */

/*#define VANILLA*/

/*
 * This function accepts a position from 0 to 1 and warps it, to 0 through 1 based
 * upon the wFact var. The warp equations are designed to:
 * 
 * * Always be rising but yield results from 0 to 1
 *
 * * Have a first derivative that doesn't go to 0 or infinity, at least close
 *   to the center of the screen
 *
 * * Have a curvature (absolute val of 2nd derivative) that is small in the
 *   center and smoothly rises towards the edges. We would like the curvature
 *   to be everywhere = 0 when the warp factor = 1
 */
static double WarpFactor(double position, double wFact)
{
  double x;
  double z;
  double w;
  x = 2 * (position - .5);
  if (1) /*(wFact < 1.0)*/
    /* For warp factor < 1 the warp is calculated as (1-w) * x^3 + w *x, centered
     *
     * The warp is calculated as z = (1 - w) * x^3 + w * x, centered
     * around .5 and ranging from 0 to 1. After some tinkering this seems
     * to give decent values and derivatives at the right places.
     */
    w = 2.0 - wFact; /* reverse parm for compat with initial release */
  
  if (x < 0.0) {
    z = -(1 - w) * x*x*x - w * x; /* -1 < x < 0, wFact < 1 */
    return .5 - .5 * z;                     
  } else {
    z = (1 - w) * x*x*x + w * x;  /* -1 < x < 0, wFact < 1 */
    return .5 + .5 * z;           /* amts to same formula as above for now */
  }
}

/*
 * YV12
 *
 * For each horizontal output pair of pixels there is are 2 qword masks followed by 2 int
 * offsets. The 2 masks are the weights to be used for the luma and chroma, respectively.
 * Each mask contains LeftWeight1, RightWeight1, LeftWeight2, RightWeight2. So a pair of pixels
 * will later be processed each pass through the horizontal resize loop.  I think with my
 * current math the Horizontal Luma and Chroma contains the same values but since I may have screwed it
 * up I'll leave it this way for now. Vertical chroma is different.
 *
 * Note - try just using the luma calcs for both, seem to be the same.
 *
 * The weights are scaled 0-256 and the left and right weights will sum to 256 for each pixel.
 */
static void init_tables_yv12(int newwidth, int newheight, int oldwidth, int oldheight,
			     int Interlaced, double hWarp, double vWarp,
			     uint32_t *hControl,   uint32_t *vOffsets,   uint32_t *vWeights,
			     uint32_t *hControlUV, uint32_t *vOffsetsUV, uint32_t *vWeightsUV)
{
  int i;
  int j;
  int k;
  int wY1;
  int wY2;

  /* First set up horizontal table, use for both luma & chroma since 
   * it seems to have the same equation.
   * We will geneerate these values in pairs, mostly because that's the way
   * I wrote it for YUY2 above.
   */

  for(i=0; i < newwidth; i+=2) {
    /* first make even pixel control */
    if (doublecmp(hWarp, 1.0) == 0)  /* if no warp factor */
      j = i * 256 * (oldwidth-1) / (newwidth-1);
    else           /* stretch and warp somehow */
      j = (int) (256 * WarpFactor(i / (newwidth-1.0), hWarp) * (oldwidth-1));
    
    k = j>>8;
    wY2 = j - (k << 8);  /* luma weight of right pixel */
    wY1 = 256 - wY2;     /* luma weight of left pixel  */

    if (k > oldwidth - 2) {
      hControl[i*3+4] = oldwidth - 1;  /* point to last byte */
      hControl[i*3] =   0x00000100;    /* use 100% of rightmost Y */
    } else {
      hControl[i*3+4] = k;             /* pixel offset */
      hControl[i*3] = wY2 << 16 | wY1; /* luma weights */
    }

    /* now make odd pixel control */
    if (doublecmp(hWarp, 1.0) == 0)   /* if no warp factor */
      j = (i+1) * 256 * (oldwidth-1) / (newwidth-1);
    else        /* stretch and warp somehow */
      j = (int) (256 * WarpFactor((i+1) / (newwidth-1.0), hWarp) * (oldwidth-1));

    k = j>>8;
    wY2 = j - (k << 8); /* luma weight of right pixel */
    wY1 = 256 - wY2;    /* luma weight of left pixel  */

    if (k > oldwidth - 2) {
      hControl[i*3+5] = oldwidth - 1; /* point to last byte  */
      hControl[i*3+1] = 0x00000100;   /* use 100% of rightmost Y */
    } else {
      hControl[i*3+5] = k;               /* pixel offset */
      hControl[i*3+1] = wY2 << 16 | wY1; /* luma weights */
    }
  }

  hControl[newwidth*3+4] =  2 * (oldwidth-1); /* give it something to prefetch at end */
  hControl[newwidth*3+5] =  2 * (oldwidth-1); /*  "  */
#ifndef VANILLA
  // UV
  for(i=0; i < newwidth/2; i+=2) {
    /* first make even pixel control */
    if (doublecmp(hWarp, 1.0) == 0)  /* if no warp factor */
      j = i * 256 * (oldwidth/2-1) / (newwidth/2-1);
    else           /* stretch and warp somehow */
      j = (int) (256 * WarpFactor(i / (newwidth/2-1.0), hWarp) * (oldwidth/2-1));
    
    k = j>>8;
    wY2 = j - (k << 8);  /* luma weight of right pixel */
    wY1 = 256 - wY2;     /* luma weight of left pixel  */

    if (k > oldwidth/2 - 2) {
      hControlUV[i*3+4] = oldwidth/2 - 1;  /* point to last byte */
      hControlUV[i*3] =   0x00000100;    /* use 100% of rightmost Y */
    } else {
      hControlUV[i*3+4] = k;             /* pixel offset */
      hControlUV[i*3] = wY2 << 16 | wY1; /* luma weights */
    }

    /* now make odd pixel control */
    if (doublecmp(hWarp, 1.0) == 0)   /* if no warp factor */
      j = (i+1) * 256 * (oldwidth/2-1) / (newwidth/2-1);
    else        /* stretch and warp somehow */
      j = (int) (256 * WarpFactor((i+1) / (newwidth/2-1.0), hWarp) * (oldwidth/2-1));

    k = j>>8;
    wY2 = j - (k << 8); /* luma weight of right pixel */
    wY1 = 256 - wY2;    /* luma weight of left pixel  */

    if (k > oldwidth/2 - 2) {
      hControlUV[i*3+5] = oldwidth/2 - 1; /* point to last byte  */
      hControlUV[i*3+1] = 0x00000100;   /* use 100% of rightmost Y */
    } else {
      hControlUV[i*3+5] = k;               /* pixel offset */
      hControlUV[i*3+1] = wY2 << 16 | wY1; /* luma weights */
    }
  }

  hControlUV[newwidth/2*3+4] =  (oldwidth/2-1); /* give it something to prefetch at end */
  hControlUV[newwidth/2*3+5] =  (oldwidth/2-1); /*  "  */
#endif

  /* Next set up vertical tables. The offsets are measured in lines and will be mult */
  /* by the source pitch later . */

  /* For YV12 we need separate Luma and chroma tables */

  /* First Luma Table */
  for(i=0; i< newheight; ++i) {
    if (doublecmp(vWarp, 1.0) == 0)  /* if no warp factor */
      j = i * 256 * (oldheight-1) / (newheight-1);
    else           /* stretch and warp somehow */
      j = (int) (256 * WarpFactor(i / (newheight-1.0), vWarp) * (oldheight-1));
    
    if (Interlaced) {  /* do hard way? */
      if (i%2) {       /* is odd output line? */
	if (j < 256) {     /* before 1st odd input line */
	  vOffsets[i] = 1; /* all from line 1 */
	  vWeights[i] = 0; /* weight to give to 2nd line */
	} else {
	  k = (((j-256) >> 9) << 1) + 1; /* next lowest odd line */
	  vOffsets[i] = k;
	  wY2 = j - (k << 8); 
	  vWeights[i] = wY2 >> 1; /* weight to give to 2nd line */
	}
      } else {         /* is even output line */
	k = (j >> 9) << 1;        /* next lower even line */
	vOffsets[i] = k;
	wY2 = j - (k << 8); 
	vWeights[i] = wY2 >> 1;   /* weight to give to 2nd line */
      }
    } else {           /* simple way, do as progressive */
      k = j >> 8;
      vOffsets[i] = k;
      wY2 = j - (k << 8); 
      vWeights[i] = wY2;   /* weight to give to 2nd line */
    }
  }

  /* Vertical table for chroma */
  for(i=0; i< newheight/2; ++i) {
    if (doublecmp(vWarp, 1.0) == 0)  /* if no warp factor */
#ifdef VANILLA
      j = (int) ( (i+.25) * 256 * (oldheight-1) / (newheight-1.0) - 64 );
#else
      j = (int) ( (i+.25) * 256 * (oldheight/2-1) / (newheight/2-1.0) - 64 );
#endif
    else           /* stretch and warp somehow */
#ifdef VANILLA
      j = (int) (256 * WarpFactor( (i+.25) / (newheight-1.0), vWarp) * (oldheight-1.0) );
#else
      j = (int) (256 * WarpFactor( (i+.25) / (newheight/2 - 1.0), vWarp) * (oldheight/2 - 1.0) );
#endif
#ifndef VANILLA
    if(j<0) j=0;
#endif
    if (Interlaced) { /* do hard way? */
      if (i%2) {                /* is odd output line? */
	if (j < 256) {            /* before 1st odd input line */
	  vOffsetsUV[i] = 1;         /* all from line 1 */
	  vWeightsUV[i] = 0;         /* weight to give to 2nd line */
	} else {
	  k = (((j-256) >> 9) << 1) + 1; /* next lowest odd line */
	  vOffsetsUV[i] = k;
	  wY2 = j - (k << 8); 
	  vWeightsUV[i] = wY2 >> 1;  /* weight to give to 2nd line */
	}
      } else {                  /* is even output line */
#ifdef VANILLA
	k = (j >> 9) << 1;         /* next lower even line */
	vOffsetsUV[i] = k;
	wY2 = j - (k << 8); 
	vWeightsUV[i] = wY2 >> 1;  /* weight to give to 2nd line */
#else
	k = (j / 512) << 1;         /* next lower even line */
	vOffsetsUV[i] = k;
	wY2 = j - (k << 8); 
	vWeightsUV[i] = wY2 >> 1;  /* weight to give to 2nd line */
#endif
      }
    } else {              /* simple way, do as progressive */
#ifdef VANILLA
      k = j >> 8;
#else
      k = j / 256; /* j >> 8;  does not work right if  -256 < j < 0 */
#endif
      vOffsetsUV[i] = k;
      wY2 = j - (k << 8);
      vWeightsUV[i] = wY2;      /* weight to give to 2nd line */
    }
  }
}

/*
 * YUY2
 *
 * For each horizontal output pair of pixels there is are 2 qword masks followed by 2 int
 * offsets. The 2 masks are the weights to be used for the luma and chroma, respectively.
 * Each mask contains LeftWeight1, RightWeight1, LeftWeight2, RightWeight2. So a pair of pixels
 * will later be processed each pass through the horizontal resize loop.
 *
 * The weights are scaled 0-256 and the left and right weights will sum to 256 for each pixel.
 */
static void init_tables_yuy2(int newwidth, int newheight, int oldwidth, int oldheight,
			     int Interlaced, double hWarp, double vWarp,
			     uint32_t *hControl, uint32_t *vOffsets, uint32_t *vWeights )
{
  int i;
  int j;
  int k;
  int wY1;
  int wY2;
  int wUV1;
  int wUV2;
  
  /* First set up horizontal table */
  for(i=0; i < newwidth; i+=2) {
    /* first make even pixel control */
    if (doublecmp(hWarp, 1.0) == 0)          /* if no warp factor */
      j = i * 256 * (oldwidth-1) / (newwidth-1);
    else                   /* stretch and warp somehow */
      j = (int) (256 * WarpFactor(i / (newwidth-1.0), hWarp) * (oldwidth-1));
    
    k = j>>8;
    wY2 = j - (k << 8);	   /* luma weight of right pixel */
    wY1 = 256 - wY2;       /* luma weight of left pixel  */
    wUV2 = (k%2)  ?  128 + (wY2 >> 1)  :  wY2 >> 1;
    wUV1 = 256 - wUV2;
    
    if (k > oldwidth - 2) {
      hControl[i*3+4] = oldwidth - 1;      /* point to last byte      */
      hControl[i*3]   = 0x00000100;        /* use 100% of rightmost Y */
      hControl[i*3+2] = 0x00000100;        /* use 100% of rightmost U */
    } else {
      hControl[i*3+4] = k;                 /* pixel offset   */
      hControl[i*3]   = wY2 << 16 | wY1;   /* luma weights   */
      hControl[i*3+2] = wUV2 << 16 | wUV1; /* chroma weights */
    }
    
    /* now make odd pixel control */
    if (doublecmp(hWarp, 1.0) == 0)  /* if no warp factor */
      j = (i+1) * 256 * (oldwidth-1) / (newwidth-1);
    else                                   /* stretch and warp somehow */
      j = (int) (256 * WarpFactor((i+1) / (newwidth-1.0), hWarp) * (oldwidth-1));
    
    k = j>>8;
    wY2 = j - (k << 8); /* luma weight of right pixel */
    wY1 = 256 - wY2;    /* luma weight of left pixel  */
    wUV2 = (k%2)  ?  128 + (wY2 >> 1)  :  wY2 >> 1;
    wUV1 = 256 - wUV2;
    
    if (k > oldwidth - 2) {
      hControl[i*3+5] = oldwidth - 1;    /* point to last byte      */
      hControl[i*3+1] = 0x00000100;      /* use 100% of rightmost Y */
      hControl[i*3+3] = 0x00000100;      /* use 100% of rightmost V */
    } else {
      hControl[i*3+5] = k;               /* pixel offset */
      hControl[i*3+1] = wY2 << 16 | wY1; /* luma weights */
      /*			hControl[i*3+3] = wUV2 << 16 | wUV1; // chroma weights */
      /* horiz chroma weights should be same as for even pixel - trbarry 09/16/2002 */
      hControl[i*3+3] = hControl[i*3+2]; /* chroma weights */
    }
  }
  
  hControl[newwidth*3+4] =  2 * (oldwidth-1); /* give it something to prefetch at end */
  hControl[newwidth*3+5] =  2 * (oldwidth-1);
  
  /* Next set up vertical table. The offsets are measured in lines and will be mult */
  /* by the source pitch later */
  for(i=0; i< newheight; ++i) {
    if (doublecmp(vWarp, 1.0) == 0)                   /* if no warp factor */
      j = i * 256 * (oldheight-1) / (newheight-1);
    else                            /* stretch and warp somehow */
      j = (int) (256 * WarpFactor(i / (newheight-1.0), vWarp) * (oldheight-1));

    if (Interlaced) {           /* do hard way? */
      if (i%2) {                  /* is odd output line? */
	if (j < 256) {            /* before 1st odd input line */
	  vOffsets[i] = 1;        /* all from line 1 */
	  vWeights[i] = 0;        /* weight to give to 2nd line */
	} else  {
	  k = (((j-256) >> 9) << 1) + 1; /* next lowest odd line */
	  vOffsets[i] = k;
	  wY2 = j - (k << 8); 
	  vWeights[i] = wY2 >> 1; /* weight to give to 2nd line */
	}
      } else {                    /* is even output line */
	k = (j >> 9) << 1;        /* next lower even line */
	vOffsets[i] = k;
	wY2 = j - (k << 8); 
	vWeights[i] = wY2 >> 1;   /* weight to give to 2nd line */
      }
    } else {                    /* simple way, do as progressive */
      k = j >> 8;
      vOffsets[i] = k;
      wY2 = j - (k << 8);  
      vWeights[i] = wY2;          /* weight to give to 2nd line */
    }
  }
}

/* Register allocation */
/* index/counter registers (REGA, REGC) are loaded from 32bit vars/arrays ! */
#define     REGEA "eax"
#define     REGEB "ebx"
#if defined(__x86_64__)
#  define   REGA  "rax"
#  define   REGB  "rbx"
#  define   REGC  "ecx"
#  define   REGD  "rdx"
#  define   REGDI "rdi"
#  define   REGSI "rsi"
#elif defined(__i386__) 
#  define   REGA  "eax"
#  define   REGB  "ebx"
#  define   REGC  "ecx"
#  define   REGD  "edx"
#  define   REGDI "edi"
#  define   REGSI "esi"
#endif

/* variables accessed from assembler code */
#define _FPround1       "%0"
#define _vWeight1       "%1" 
#define _vWeight2       "%2" 
#define _YMask          "%3"
#define _src_row_size   "%4"
#define _EndOffset      "%5"
#define _pControl       "%6"
#define _row_size       "%7"
#define _vWorkYW        "%8"
#define _dstp           "%9"
#define _vWorkUVW       "%10"
#define _FPround2       "%11"
#define _srcp1          "%12"
#define _srcp2          "%13"
#if !defined(__x86_64__)
#define _oldbx          "%14"
#define _SSEMMXenabledW "%15"
#define _SSE2enabledW   "%16"
#endif

/* Labels */
#define vMaybeSSEMMX      "1"
#define LessThan8         "2"
#define LessThan4         "3"
#define AllDone           "4"
#define LastOne           "5"
#define vLoopSSE2_Fetch   "6"
#define vLoopSSE2         "7"
#define vLoopSSEMMX_Fetch "8"
#define vLoopSSEMMX       "9"
#define vLoopMMX         "10"
#define MoreSpareChange  "11"
#define DoHorizontal     "12"
#define hLoopMMX         "13"
#define hLoopMMXSSE      "14"


/* structure for mmx constants */
typedef union {
  uint64_t uq[1];  /* Unsigned Quadword */
  uint32_t ud[2];  /* Unsigned Doubleword */
} ATTR_ALIGN(16) mmx_t;

/* structure for sse2 constants */
typedef union {
  uint64_t uq[2];  /* Unsigned Quadword */
  uint32_t ud[4];  /* Unsigned Doubleword */
} ATTR_ALIGN(16) sse2_t;


static int do_warp_yuy2(uint8_t *dst, const uint8_t *src,
			const int dst_pitch, const int src_pitch,
			const int dst_width, const int dst_height,
			const int src_width, const int src_height,
			const int Interlaced, const uint32_t * const hControl, 
			const uint32_t * const vOffsets, const uint32_t * const vWeights,
			uint32_t *vWorkY, uint32_t *vWorkUV,
			int dst_start)
{
#if defined(__i386__) || defined(__x86_64__)
  sse2_t YMask    = {uq:{UINT64_C(0x00ff00ff00ff00ff),UINT64_C(0x00ff00ff00ff00ff)}}; /* keeps only luma */
  sse2_t FPround1 = {uq:{UINT64_C(0x0080008000800080),UINT64_C(0x0080008000800080)}}; /* round words      */
  sse2_t FPround2 = {uq:{UINT64_C(0x0000008000000080),UINT64_C(0x0000008000000080)}}; /* round dwords     */
  sse2_t vWeight1;
  sse2_t vWeight2;

  const uint32_t *pControl = &hControl[0];
  const uint32_t *vWorkYW = vWorkY;
  const uint32_t *vWorkUVW = vWorkUV;
  const uint8_t *srcp = src;
  const uint8_t *srcp1;
  const uint8_t *srcp2;
  uint8_t *dstp = dst + dst_pitch*dst_start;

  const uint32_t src_row_size = src_width * 2;
  const uint32_t row_size = dst_width * 2;
  const uint32_t EndOffset = src_row_size / 2;

#if !defined(__x86_64__)
  const int accel = xine_mm_accel();
  const uint32_t SSE2enabledW   = !!(accel & MM_ACCEL_X86_SSE2);   /* in local storage for asm */
  const uint32_t SSEMMXenabledW = !!(accel & MM_ACCEL_X86_MMXEXT); /* in local storage for asm */
  long int oldbx;
#endif
  int y;

  for (y = dst_start; y < dst_height; y++) {

    if((int)vOffsets[y] >= src_height) {
      /* slice completed */
      /*DBG("do_warp_yuy2: max input height reached: need line %d, height %d\n -> Returning next output line: %d\n",
	vOffsets[y], src_height, y);*/
      return y;
    }

    vWeight1.ud[0] = vWeight1.ud[1] = vWeight1.ud[2] = vWeight1.ud[3] = 
      (256-vWeights[y]) << 16 | (256-vWeights[y]);
    vWeight2.ud[0] = vWeight2.ud[1] = vWeight2.ud[2] = vWeight2.ud[3] = 
      vWeights[y] << 16 | vWeights[y];

    srcp1 = srcp + vOffsets[y] * src_pitch;
    if (Interlaced)
      srcp2 = (y < dst_height-2)  ?  srcp1 + 2 * src_pitch  :  srcp1;
    else
      srcp2 = (y < dst_height-1)  ?  srcp1 + src_pitch      :  srcp1;

    __asm__ __volatile__ (
#if !defined(__x86_64__)
            /* store ebx (PIC) */
            "mov %%"REGB", "_oldbx"          \n\t"
#endif
	    "movl   "_src_row_size", %%"REGC"  \n\t"
	    "shrl   $3,          %%"REGC"      \n\t" /* 8 bytes a time             */
	    "mov    "_srcp1",    %%"REGSI"     \n\t" /* top of 2 src lines to get  */
	    "mov    "_srcp2",    %%"REGD"      \n\t" /* next "                     */
	    "mov    "_vWorkYW",  %%"REGDI"     \n\t" /* luma work destination line */
	    "mov    "_vWorkUVW", %%"REGB"      \n\t" /* luma work destination line */
	    "xor    %%"REGA",    %%"REGA"      \n\t"
#if !defined(__x86_64__)
	    /* Let's check here to see if we are on a P4 or higher and can use SSE2 instructions.
	     * This first loop is not the performance bottleneck anyway but it is trivial to tune
	     * using SSE2 if we have proper alignment.
	     */
	    "testl  $1, "_SSE2enabledW"  \n\t"  /* is SSE2 supported?*/
	    "jz     "vMaybeSSEMMX"f      \n\t"  /* n, can't do anyway*/
#endif
	    "cmpl   $2, %%"REGC"         \n\t"  /* we have at least 16 bytes, 2 qwords? */
	    "jl     "vMaybeSSEMMX"f      \n\t"  /* n, don't bother*/
	    
	    "shrl   $1, %%"REGC"         \n\t"  /* do 16 bytes at a time instead*/
	    "decl   %%"REGC"             \n"    /* jigger loop ct */
	    
	    ".align 16                   \n\t"

	    "movdqu "_FPround1", %%xmm0  \n\t"
	    "movdqu "_vWeight1", %%xmm5  \n\t"
	    "movdqu "_vWeight2", %%xmm6  \n\t"
	    "movdqu "_YMask",    %%xmm7  \n"

	    ""vLoopSSE2_Fetch":          \n\t"
#ifdef PREFETCH
	    "  prefetcht0 16(%%"REGSI", %%"REGA", 2) \n\t"
	    "  prefetcht0 16(%%"REGD",  %%"REGA", 2) \n"
#endif	    
	    ""vLoopSSE2":  \n\t"
	    "  movdqu   (%%"REGSI", %%"REGA", 2), %%xmm1 \n\t" /* top of 2 lines to interpolate */
	    "  movdqu   (%%"REGD",  %%"REGA", 2), %%xmm2 \n\t" /* 2nd of 2 lines */

	    "  movdqa   %%xmm1, %%xmm3  \n\t"  /* get chroma  bytes  */
	    "  pand     %%xmm7, %%xmm1  \n\t"  /* keep only luma     */
	    "  psrlw        $8, %%xmm3  \n\t"  /* right just chroma  */
	    "  pmullw   %%xmm5, %%xmm1  \n\t"  /* mult by weighting factor */
	    "  pmullw   %%xmm5, %%xmm3  \n\t"  /* mult by weighting factor */

	    "  movdqa   %%xmm2, %%xmm4  \n\t"  /* get chroma bytes  */
	    "  pand     %%xmm7, %%xmm2  \n\t"  /* keep only luma    */
	    "  psrlw        $8, %%xmm4  \n\t"  /* right just chroma */
	    "  pmullw   %%xmm6, %%xmm2  \n\t"  /* mult by weighting factor */
	    "  pmullw   %%xmm6, %%xmm4  \n\t"  /* mult by weighting factor */
	    
	    "  paddw    %%xmm2, %%xmm1  \n\t"  /* combine lumas     */
	    "  paddusw  %%xmm0, %%xmm1  \n\t"  /* round             */
	    "  psrlw        $8, %%xmm1  \n\t"  /* right adjust luma */
#ifdef STREAMING_STORE_TMP
	    "  movntdq  %%xmm1, (%%"REGDI", %%"REGA", 2) \n\t" /* save lumas in our work area */
#else
	    "  movdqu   %%xmm1, (%%"REGDI", %%"REGA", 2) \n\t" /* save lumas in our work area */
#endif
	    "  paddw    %%xmm4, %%xmm3  \n\t"  /* combine chromas */
	    "  paddusw  %%xmm0, %%xmm3  \n\t"  /* round */
	    "  psrlw        $8, %%xmm3  \n\t"  /* right adjust chroma */
	    "  packuswb %%xmm3, %%xmm3  \n\t"  /* pack UV's into low dword */
	    "  movdq2q  %%xmm3, %%mm1   \n\t"  /* save in our work area    */
#ifdef STREAMING_STORE_TMP
	    "  movntq    %%mm1, (%%"REGB", %%"REGA") \n\t"  /* save in our work area */
#else
	    "  movq      %%mm1, (%%"REGB", %%"REGA") \n\t"  /* save in our work area */
#endif
	    "  lea   8(%%"REGA"), %%"REGA"  \n\t"
	    "  decl  %%"REGC"               \n\t"
	    
	    "  jg    "vLoopSSE2_Fetch"b     \n\t"  /* if not on last one loop, prefetch */
	    "  jz    "vLoopSSE2"b           \n\t"  /* or just loop, or not */

	    /* done with our SSE2 fortified loop but we may need to pick up the spare change */
#ifdef STREAMING_STORE_TMP
	    "  sfence    \n\t"
#endif
	    "  movl  "_src_row_size", %%"REGC" \n\t"  /* get count again   */
	    "  andl  $15, %%"REGC"          \n\t"  /* just need mod 16  */

	    "  movq  "_YMask",    %%mm7     \n\t"  /* useful luma mask constant - lazy dupl init */
	    "  movq  "_vWeight1", %%mm5     \n\t"
	    "  movq  "_vWeight2", %%mm6     \n\t"
	    "  movq  "_FPround1", %%mm0     \n\t"  /* useful rounding constant  */

	    "  shrl  $3, %%"REGC"     \n\t"  /* 8 bytes at a time, any?  */
	    "  jz   "MoreSpareChange"f \n"    /* n, did them all  */

	    /* Let's check here to see if we are on a P2 or Athlon and can use SSEMMX instructions.
	     * This first loop is not the performance bottleneck anyway but it is trivial to tune
	     * using SSE if we have proper alignment.
	     */
	    ""vMaybeSSEMMX":    \n\t"

	    "  movq  "_YMask",    %%mm7    \n\t"  /* useful luma mask constant - lazy dupl init */
	    "  movq  "_vWeight1", %%mm5    \n\t"  
	    "  movq  "_vWeight2", %%mm6    \n\t"  
	    "  movq  "_FPround1", %%mm0    \n\t"  /* useful rounding constant  */
#if !defined(__x86_64__)
	    "  testl $1, "_SSEMMXenabledW" \n\t"  /* MMXEXTsupported? */
	    "  jz    "vLoopMMX"f           \n\t"  /* n, can't do anyway */
#endif
	    "  decl  %%"REGC"              \n"    /* jigger loop ctr */

	    ".align 16             \n"
	    ""vLoopSSEMMX_Fetch":  \n\t"
#ifdef PREFETCH
	    "  prefetcht0 8(%%"REGSI", %%"REGA", 2)  \n\t"
	    "  prefetcht0 8(%%"REGD",  %%"REGA", 2)  \n"
#endif
	    ""vLoopSSEMMX":   \n\t"
	    "  movq    (%%"REGSI", %%"REGA", 2), %%mm1  \n\t"   /* top of 2 lines to interpolate */
	    "  movq    (%%"REGD",  %%"REGA", 2), %%mm2  \n\t"   /* 2nd of 2 lines    */

	    "  movq    %%mm1, %%mm3  \n\t"   /* copy top bytes */
	    "  pand    %%mm7, %%mm1  \n\t"   /* keep only luma */
	    "  pxor    %%mm1, %%mm3  \n\t"   /* keep only chroma */
	    "  psrlw      $8, %%mm3  \n\t"   /* right just chroma */
	    "  pmullw  %%mm5, %%mm1  \n\t"   /* mult by weighting factor */
	    "  pmullw  %%mm5, %%mm3  \n\t"   /* mult by weighting factor */
			  
	    "  movq    %%mm2, %%mm4  \n\t"   /* copy 2nd bytes */
	    "  pand    %%mm7, %%mm2  \n\t"   /* keep only luma */
	    "  pxor    %%mm2, %%mm4  \n\t"   /* keep only chroma */
	    "  psrlw      $8, %%mm4  \n\t"   /* right just chroma */
	    "  pmullw  %%mm6, %%mm2  \n\t"   /* mult by weighting factor */
	    "  pmullw  %%mm6, %%mm4  \n\t"   /* mult by weighting factor */
	    
	    "  paddw   %%mm2, %%mm1  \n\t"   /* combine lumas     */
	    "  paddusw %%mm0, %%mm1  \n\t"   /* round             */
	    "  psrlw      $8, %%mm1  \n\t"   /* right adjust luma */
#ifdef STREAMING_STORE_TMP
	    "  movntq  %%mm1, (%%"REGDI", %%"REGA", 2) \n\t"  /* save lumas in our work area */
#else
	    "  movq    %%mm1, (%%"REGDI", %%"REGA", 2) \n\t"  /* save lumas in our work area */
#endif	    
	    "  paddw    %%mm4, %%mm3  \n\t"  /* combine chromas  */
	    "  paddusw  %%mm0, %%mm3  \n\t"  /* round            */
	    "  psrlw       $8, %%mm3  \n\t"  /* right adjust chroma  */
	    "  packuswb %%mm3, %%mm3  \n\t"  /* pack UV's into low dword */
	    "  movd     %%mm3, (%%"REGB", %%"REGA") \n\t"  /* save in our work area    */
	    
	    "  lea   4(%%"REGA"), %%"REGA" \n\t"
	    "  decl  %%"REGC"              \n\t"
	    "  jg    "vLoopSSEMMX_Fetch"b  \n\t"  /* if not on last one loop, prefetch  */
	    "  jz    "vLoopSSEMMX"b        \n\t"  /* or just loop, or not  */
#ifdef STREAMING_STORE_TMP
	    "  sfence                      \n\t"
#endif
	    "  jmp    "MoreSpareChange"f   \n"    /* all done with vertical  */
	    
	    ".align 16     \n"
	    ""vLoopMMX":   \n\t"

	    "  movq (%%"REGSI", %%"REGA", 2), %%mm1  \n\t" /* top of 2 lines to interpolate */
	    "  movq (%%"REGD",  %%"REGA", 2), %%mm2  \n\t" /* 2nd of 2 lines */

	    "  movq     %%mm1, %%mm3  \n\t"  /* copy top bytes    */
	    "  pand     %%mm7, %%mm1  \n\t"  /* keep only luma    */
	    "  pxor     %%mm1, %%mm3  \n\t"  /* keep only chroma  */
	    "  psrlw       $8, %%mm3  \n\t"  /* right just chroma */
	    "  pmullw   %%mm5, %%mm1  \n\t"  /* mult by weighting factor */
	    "  pmullw   %%mm5, %%mm3  \n\t"  /* mult by weighting factor */
	    
	    "  movq     %%mm2, %%mm4  \n\t"  /* copy 2nd bytes    */
	    "  pand     %%mm7, %%mm2  \n\t"  /* keep only luma    */
	    "  pxor     %%mm2, %%mm4  \n\t"  /* keep only chroma  */
	    "  psrlw       $8, %%mm4  \n\t"  /* right just chroma */
	    "  pmullw   %%mm6, %%mm2  \n\t"  /* mult by weighting factor */
	    "  pmullw   %%mm6, %%mm4  \n\t"  /* mult by weighting factor */
	    
	    "  paddw    %%mm2, %%mm1  \n\t"  /* combine lumas     */
	    "  paddusw  %%mm0, %%mm1  \n\t"  /* round             */
	    "  psrlw       $8, %%mm1  \n\t"  /* right adjust luma */
	    "  movq     %%mm1, (%%"REGDI", %%"REGA", 2) \n\t"  /* save lumas in our work area */
	    
	    "  paddw    %%mm4, %%mm3  \n\t"  /* combine chromas     */
	    "  paddusw  %%mm0, %%mm3  \n\t"  /* round               */
	    "  psrlw       $8, %%mm3  \n\t"  /* right adjust chroma */
	    "  packuswb %%mm3, %%mm3  \n\t"  /* pack UV's into low dword */
	    "  movd     %%mm3, (%%"REGB", %%"REGA")  \n\t"  /* save in our work area */

	    "  lea      4(%%"REGA"), %%"REGA"  \n\t"
	    "  loop     "vLoopMMX"b      \n"

	    /* Add a little code here to check if we have 2 more pixels to do and, if so, make one
	     * more pass thru vLoopMMX. We were processing in multiples of 4 pixels and alway have
	     * an even number so there will never be more than 2 left. trbarry 7/29/2002
	     */
	    ""MoreSpareChange":    \n\t"

	    "  cmpl  "_EndOffset", %%"REGEA"  \n\t"  /* did we get them all */
	    "  jnl   "DoHorizontal"f \n\t"  /* yes, else have 2 left */
	    "  movl  $1, %%"REGC"    \n\t"  /* jigger loop ct */
	    "  sub   $2, %%"REGA"    \n\t"  /* back up 2 pixels (4 bytes, but eax carried as 1/2) */
	    "  jmp   "vLoopMMX"b     \n"

	    /*  We've taken care of the vertical scaling, now do horizontal  */
	    ""DoHorizontal":      \n\t"

	    "  movq  "_YMask",    %%mm7     \n\t"  /* useful 0U0U..  mask constant  */
	    "  movq  "_FPround2", %%mm6     \n\t"  /* useful rounding constant, dwords  */
	    "  mov   "_pControl", %%"REGSI" \n\t"  /* @ horiz control bytes  */	
	    "  movl  "_row_size", %%"REGC"  \n\t"
	    "  shrl  $2,          %%"REGC"  \n\t"  /* bytes a time, 2 pixels  */
	    "  mov   "_vWorkYW",  %%"REGD"  \n\t"  /* our luma data, as 0Y0Y 0Y0Y..  */
	    "  mov   "_dstp",     %%"REGDI" \n\t"  /* the destination line  */
	    "  mov   "_vWorkUVW", %%"REGB"  \n"    /* chroma data, as UVUV UVUV...  */

	    ".align 16  \n"
	    ""hLoopMMX":    \n\t"

	    /* x86_64: must use movl (accessing table of uint32's) */
	    "  movl      16(%%"REGSI"), %%"REGEA"        \n\t"  /* get data offset in pixels, 1st pixel pair */
	    "  movd      (%%"REGD", %%"REGA", 2), %%mm0  \n\t"  /* copy luma pair */
	    "  shr       $1, %%"REGA"                    \n\t"  /* div offset by 2 */
	    "  movd      (%%"REGB", %%"REGA", 2), %%mm1  \n\t"  /* copy UV pair VUVU */
	    "  psllw     $8, %%mm1                       \n\t"  /* shift out V, keep 0000U0U0 */
	    
	    /*  we need to use both even and odd croma from same location - trb 9/2002 */
	    "  punpckldq (%%"REGB", %%"REGA", 2), %%mm1  \r\n"  /* copy UV pair VUVU  */
	    "  psrlw     $8, %%mm1                       \r\n"  /* shift out U0, keep 0V0V 0U0U   */
	    "  movl      20(%%"REGSI"), %%"REGEA"        \r\n"  /* get data offset in pixels, 2nd pixel pair  */
	    "  punpckldq (%%"REGD", %%"REGA", 2), %%mm0  \r\n"  /* copy luma pair  */
	    
	    "  pmaddwd    (%%"REGSI"), %%mm0  \r\n"  /* mult and sum lumas by ctl weights  */
	    "  paddusw    %%mm6, %%mm0        \r\n"  /* round  */
	    "  psrlw      $8, %%mm0           \r\n"  /* right just 2 luma pixel value 000Y,000Y  */
	    
	    "  pmaddwd    8(%%"REGSI"), %%mm1 \r\n"  /* mult and sum chromas by ctl weights */
	    "  paddusw    %%mm6, %%mm1        \r\n"  /* round */
	    "  pslld      $8, %%mm1           \r\n"  /* shift into low bytes of different words */
	    "  pand       %%mm7, %%mm1        \r\n"  /* keep only 2 chroma values 0V00,0U00  */
	    "  por        %%mm1, %%mm0        \r\n"  /* combine luma and chroma, 0V0Y,0U0Y  */
	    "  packuswb   %%mm0, %%mm0        \r\n"  /* pack all into low dword, xxxxVYUY  */
	    "  movd       %%mm0, (%%"REGDI")  \n\t"  /* done with 2 pixels */

	    "  lea     24(%%"REGSI"), %%"REGSI"  \n\t"  /* bump to next control bytest */
	    "  lea      4(%%"REGDI"), %%"REGDI"  \n\t"  /* bump to next output pixel addr */
	    
	    "  loop   "hLoopMMX"b             \n\t"  /* loop for more */

	    "emms              \n\t"
	    /* done with one line */

#if !defined(__x86_64__)
	    "mov "_oldbx", %%"REGB" \n\t"
#endif
	    ::
	    "m" /*0*/(FPround1), 
	    "m" /*1*/(vWeight1), 
	    "m" /*2*/(vWeight2), 
	    "m" /*3*/(YMask),
	    "m" /*4*/(src_row_size),
	    "m" /*5*/(EndOffset),
	    "m" /*6*/(pControl),
	    "m" /*7*/(row_size),
	    "m" /*8*/(vWorkYW),
	    "m" /*9*/(dstp),
	    "m" /*10*/(vWorkUVW),
	    "m" /*11*/(FPround2),
	    "m" /*12*/(srcp1),
	    "m" /*13*/(srcp2)
#if !defined(__x86_64__)
	    ,
	    "m" /*14*/(oldbx),
	    "m" /*15*/(SSEMMXenabledW),
	    "m" /*16*/(SSE2enabledW)
	    : REGA, /*REGB,*/ REGC, REGD, REGSI, REGDI
#else
	    : REGA, REGB, REGC, REGD, REGSI, REGDI
#endif
	    );

    dstp += dst_pitch;
  }
#endif
  return 0;
}

static int do_warp_yv12(uint8_t *dst, const uint8_t * const src,
			const int dst_pitch, const int src_pitch,
			const int dst_width, const int dst_height,			 
			const int src_width, const int src_height,
			const int Interlaced, const uint32_t * const hControl, 
			const uint32_t * vOffsets, const uint32_t * vWeights,
			uint32_t *vWorkY, int dst_start)
{
#if defined(__i386__) || defined(__x86_64__)
  sse2_t FPround1 = {uq:{UINT64_C(0x0080008000800080),UINT64_C(0x0080008000800080)}}; /* round words     */
  sse2_t FPround2 = {uq:{UINT64_C(0x0000008000000080),UINT64_C(0x0000008000000080)}}; /* round dwords    */
  sse2_t vWeight1;
  sse2_t vWeight2;

  const uint32_t *pControl = &hControl[0];
  const uint32_t *vWorkYW = vWorkY;
  const uint8_t *srcp = src;
  const uint8_t *srcp1;
  const uint8_t *srcp2;
  uint8_t *dstp = dst + dst_pitch*dst_start;

  const uint32_t src_row_size = src_width;
  const uint32_t row_size = dst_width;

#if !defined(__x86_64__)
  const int accel = xine_mm_accel();
  const uint32_t SSE2enabledW   = !!(accel & MM_ACCEL_X86_SSE2);   /* in local storage for asm */
  const uint32_t SSEMMXenabledW = !!(accel & MM_ACCEL_X86_MMXEXT); /* in local storage for asm */
  long int oldbx;
#endif
  int y;

  /* Operation in sliced mode:
   *  - continue until required next source line is out of slice
   *  - return next output line
   *  - at next call, continue from next souce line
   */

  for (y = dst_start; y < dst_height; y++) {
    if((int)vOffsets[y] >= src_height) {
      /* slice completed */
      /*DBG("do_warp_yv12: max input height reached: need line %d, height %d\n -> Returning next output line: %d , start was %d\n",
	(int)vOffsets[y], (int)src_height, (int)y, (int)dst_start);*/
      return y;
    }

    vWeight1.ud[0] = vWeight1.ud[1] = vWeight1.ud[2] = vWeight1.ud[3] =
      (256-vWeights[y]) << 16 | (256-vWeights[y]);
    vWeight2.ud[0] = vWeight2.ud[1] = vWeight2.ud[2] = vWeight2.ud[3] =
      vWeights[y] << 16 | vWeights[y];

    srcp1 = srcp + vOffsets[y] * src_pitch;

    if (Interlaced)
      srcp2 = (y < dst_height-2)  ?  srcp1 + 2 * src_pitch  :  srcp1;
    else
      srcp2 = (y < dst_height-1)  ?  srcp1 + src_pitch  :  srcp1;

    __asm__  __volatile__(
             "movl "_src_row_size", %%"REGC" \n\t"
	     "shr  $3,         %%"REGC"   \n\t"  /* 8 bytes a time */
	     "mov  "_srcp1",   %%"REGSI"  \n\t"  /* top of 2 src lines to get */
	     "mov  "_srcp2",   %%"REGD"   \n\t"  /* next "  */ 
	     "mov  "_vWorkYW", %%"REGDI"  \n\t"  /* luma work destination line */
	     "xor  %%"REGA",   %%"REGA"   \n\t"
#if !defined(__x86_64__)
	     /* Let's check here to see if we are on a P4 or higher and can use SSE2 instructions.
	      * This first loop is not the performance bottleneck anyway but it is trivial to tune
	      * using SSE2 if we have proper alignment.
	      */
	     "testl $1, "_SSE2enabledW"   \n\t"  /* is SSE2 supported? */
	     "jz    "vMaybeSSEMMX"f       \n\t"  /* n, can't do anyway */
#endif
	     "cmpl  $2, %%"REGC"          \n\t"  /* we have at least 16 byts, 2 qwords? */
	     "jl    "vMaybeSSEMMX"f       \n\t"  /* n, don't bother */
	     
	     "mov   %%"REGSI", %%"REGB"   \n\t"
	     "or    %%"REGD",  %%"REGB"   \n\t"
	     "test  $15,       %%"REGB"   \n\t"  /* both src rows 16 byte aligned? */
	     "jnz   "vMaybeSSEMMX"f       \n\t"  /* n, don't use sse2 */
			 
	     "shr   $1, %%"REGC"          \n\t"  /* do 16 bytes at a time instead */
	     "dec   %%"REGC"              \n\t"  /* jigger loop ct */
			 
	     "movdqu "_FPround1", %%xmm0  \n\t"
	     "movdqu "_vWeight1", %%xmm5  \n\t"
	     "movdqu "_vWeight2", %%xmm6  \n\t"
	     "pxor        %%xmm7, %%xmm7  \n"

	     ".align 16                   \n"
	     ""vLoopSSE2_Fetch":          \n\t"
#ifdef PREFETCH
	     "  prefetcht0 16(%%"REGSI", %%"REGA", 2) \n\t"
	     "  prefetcht0 16(%%"REGD",  %%"REGA", 2) \n"
#endif
	     ""vLoopSSE2":  \n\t"
	     /* we're already checked pointers to be on dqword aligned */
	     "  movdqa  (%%"REGSI", %%"REGA"), %%xmm1 \n\t" /* top of 2 lines to interpolate */
	     "  movdqa  (%%"REGD",  %%"REGA"), %%xmm3 \n\t" /* 2nd of 2 lines */
	     "  movdqa    %%xmm1, %%xmm2  \n\t"
	     "  movdqa    %%xmm3, %%xmm4  \n\t"

	     "  punpcklbw %%xmm7, %%xmm1  \n\t"  /* make words */
	     "  punpckhbw %%xmm7, %%xmm2  \n\t"  /*     "     */
	     "  punpcklbw %%xmm7, %%xmm3  \n\t"  /*     "     */
	     "  punpckhbw %%xmm7, %%xmm4  \n\t"  /*     "     */

	     "  pmullw    %%xmm5, %%xmm1  \n\t"  /* mult by top weighting factor */
	     "  pmullw    %%xmm5, %%xmm2  \n\t"  /*    "    */
	     "  pmullw    %%xmm6, %%xmm3  \n\t"  /* mult by bot weighting factor */
	     "  pmullw    %%xmm6, %%xmm4  \n\t"  /*    "    */

	     "  paddw     %%xmm3, %%xmm1  \n\t"  /* combine lumas low */
	     "  paddw     %%xmm4, %%xmm2  \n\t"  /* combine lumas high */

	     "  paddusw   %%xmm0, %%xmm1  \n\t"  /* round */
	     "  paddusw   %%xmm0, %%xmm2  \n\t"  /* round */
			
	     "  psrlw     $8, %%xmm1      \n\t"  /* right adjust luma */
	     "  psrlw     $8, %%xmm2      \n\t"  /* right adjust luma */

	     "  packuswb  %%xmm2, %%xmm1  \n\t"  /* pack words to our 16 byte answer */
#ifdef STREAMING_STORE_TMP
	     "  movntdq   %%xmm1, (%%"REGDI", %%"REGA") \n\t" /* save lumas in our work area */
#else
	     "  movdqu    %%xmm1, (%%"REGDI", %%"REGA") \n\t" /* save lumas in our work area */
#endif			 
	     "  lea   16(%%"REGA"), %%"REGA" \n\t"
	     "  decl  %%"REGC"            \n\t"

	     "  jg    "vLoopSSE2_Fetch"b  \n\t"  /* if not on last one loop, prefetch  */
	     "  jz    "vLoopSSE2"b        \n\t"  /* or just loop, or not  */

	     /* done with our SSE2 fortified loop but we may need to pick up the spare change */
#ifdef STREAMING_STORE_TMP
	     "  sfence                  \n\t"
#endif
	     "  movl  "_src_row_size", %%"REGC" \n\t"  /* get count again   */
	     "  andl  $15, %%"REGC"       \n\t"  /* just need mod 16  */
	     "  movq "_vWeight1", %%mm5   \n\t"
	     "  movq "_vWeight2", %%mm6   \n\t"
	     "  movq "_FPround1", %%mm0   \n\t"  /* useful rounding constant  */

	     "  shrl  $3, %%"REGC"        \n\t"  /* 8 bytes at a time, any?  */
	     "  jz   "MoreSpareChange"f   \n"    /* n, did them all  */

	     /* Let's check here to see if we are on a P2 or Athlon and can use SSEMMX instructions.
	      * This first loop is not the performance bottleneck anyway but it is trivial to tune
	      * using SSE if we have proper alignment.
	      */
	     ""vMaybeSSEMMX":             \n\t"

	     "  movq "_vWeight1", %%mm5   \n\t"  
	     "  movq "_vWeight2", %%mm6   \n\t"  
	     "  movq "_FPround1", %%mm0   \n\t"  /* useful rounding constant  */
	     "  pxor       %%mm7, %%mm7   \n\t"
#if !defined(__x86_64__)
	     "  testl $1, "_SSEMMXenabledW" \n\t"/* MMXEXTsupported? */
	     "  jz    "vLoopMMX"f         \n\t"  /* n, can't do anyway */
#endif
	     "  decl  %%"REGC"      \n"  /* jigger loop ctr */
			 
	     ".align 16             \n"
	     ""vLoopSSEMMX_Fetch":  \n\t"
#ifdef PREFETCH
	     "  prefetcht0 8(%%"REGSI", %%"REGA")  \n\t"
	     "  prefetcht0 8(%%"REGD",  %%"REGA")  \n"
#endif
	     ""vLoopSSEMMX":   \n\t"

	     "  movq    (%%"REGSI", %%"REGA"), %%mm1  \n\t"   /* top of 2 lines to interpolate */
	     "  movq    (%%"REGD",  %%"REGA"), %%mm3  \n\t"   /* 2nd of 2 lines    */

	     "  movq      %%mm1, %%mm2  \n\t"
	     "  movq      %%mm3, %%mm4  \n\t"

	     "  punpcklbw %%mm7, %%mm1  \n\t"  /* make words */
	     "  punpckhbw %%mm7, %%mm2  \n\t"  /*     "     */
	     "  punpcklbw %%mm7, %%mm3  \n\t"  /*     "     */
	     "  punpckhbw %%mm7, %%mm4  \n\t"  /*     "     */

	     "  pmullw    %%mm5, %%mm1  \n\t"  /* mult by top weighting factor */
	     "  pmullw    %%mm5, %%mm2  \n\t"  /*    "    */
	     "  pmullw    %%mm6, %%mm3  \n\t"  /* mult by bot weighting factor */
	     "  pmullw    %%mm6, %%mm4  \n\t"  /*    "    */

	     "  paddw     %%mm3, %%mm1  \n\t"  /* combine lumas low */
	     "  paddw     %%mm4, %%mm2  \n\t"  /* combine lumas high */

	     "  paddusw   %%mm0, %%mm1  \n\t"  /* round */
	     "  paddusw   %%mm0, %%mm2  \n\t"  /* round */
			
	     "  psrlw     $8, %%mm1     \n\t"  /* right adjust luma */
	     "  psrlw     $8, %%mm2     \n\t"  /* right adjust luma */

	     "  packuswb  %%mm2, %%mm1  \n\t"  /* pack words to our 8 byte answer */
#ifdef STREAMING_STORE_TMP
	     "  movntq    %%mm1, (%%"REGDI", %%"REGA") \n\t" /* save lumas in our work area */
#else
	     "  movq      %%mm1, (%%"REGDI", %%"REGA") \n\t" /* save lumas in our work area */
#endif
	     "  lea   8(%%"REGA"), %%"REGA" \n\t"
	     "  decl  %%"REGC"              \n\t"

	     "  jg    "vLoopSSEMMX_Fetch"b  \n\t"  /* if not on last one loop, prefetch  */
	     "  jz    "vLoopSSEMMX"b        \n\t"  /* or just loop, or not  */
#ifdef STREAMING_STORE_TMP
	     "  sfence                      \n\t"
#endif
	     "  jmp    "MoreSpareChange"f   \n"    /* all done with vertical  */

	     ".align 16        \n"
	     ""vLoopMMX":      \n\t"

	     "  movq    (%%"REGSI", %%"REGA"), %%mm1  \n\t"  /* top of 2 lines to interpolate */
	     "  movq    (%%"REGD",  %%"REGA"), %%mm3  \n\t"  /* 2nd of 2 lines    */

	     "  movq      %%mm1, %%mm2  \n\t"
	     "  movq      %%mm3, %%mm4  \n\t"

	     "  punpcklbw %%mm7, %%mm1  \n\t"  /* make words */
	     "  punpckhbw %%mm7, %%mm2  \n\t"  /*     "     */
	     "  punpcklbw %%mm7, %%mm3  \n\t"  /*     "     */
	     "  punpckhbw %%mm7, %%mm4  \n\t"  /*     "     */
	     
	     "  pmullw    %%mm5, %%mm1  \n\t"  /* mult by top weighting factor */
	     "  pmullw    %%mm5, %%mm2  \n\t"  /*    "    */
	     "  pmullw    %%mm6, %%mm3  \n\t"  /* mult by bot weighting factor */
	     "  pmullw    %%mm6, %%mm4  \n\t"  /*    "    */

	     "  paddw     %%mm3, %%mm1  \n\t"  /* combine lumas low */
	     "  paddw     %%mm4, %%mm2  \n\t"  /* combine lumas high */

	     "  paddusw   %%mm0, %%mm1  \n\t"  /* round */
	     "  paddusw   %%mm0, %%mm2  \n\t"  /* round */
			
	     "  psrlw     $8, %%mm1     \n\t"  /* right adjust luma */
	     "  psrlw     $8, %%mm2     \n\t"  /* right adjust luma */

	     "  packuswb  %%mm2, %%mm1  \n\t"  /* pack words to our 8 byte answer */
	     "  movq      %%mm1, (%%"REGDI", %%"REGA") \n\t" /* save lumas in our work area */
			 
	     "  lea   8(%%"REGA"), %%"REGA" \n\t"
	     "  loop  "vLoopMMX"b  \n"

	     /* Add a little code here to check if we have more pixels to do and, if so, make one
	      * more pass thru vLoopMMX. We were processing in multiples of 8 pixels and alway have
	      * an even number so there will never be more than 7 left.
	      */
	     ""MoreSpareChange":     \n\t"

	     "  cmpl "_src_row_size", %%"REGEA"  \n\t"  /* did we get them all */
	     "  jnl  "DoHorizontal"f  \n\t"  /* yes, else have 2 left */
	     "  movl $1, %%"REGC"     \n\t"  /* jigger loop ct */
	     "  movl "_src_row_size", %%"REGEA"  \n\t"
	     "  sub  $8, %%"REGA"     \n\t"  /* back up to last 8 pixels */
	     "  jmp  "vLoopMMX"b      \n"

	     /*  We've taken care of the vertical scaling, now do horizontal  */
	     ""DoHorizontal":        \n\t"
	     "  pxor        %%mm7, %%mm7     \n\t"
	     "  movq  "_FPround2", %%mm6     \n\t"  /* useful rounding constant, dwords  */
	     "  mov   "_pControl", %%"REGSI" \n\t"  /* @ horiz control bytes  */	
	     "  movl  "_row_size", %%"REGC"  \n\t"
	     "  shrl  $2, %%"REGC"          \n\t"  /* 4 bytes a time, 4 pixels  */
	     "  mov   "_vWorkYW",  %%"REGD"  \n\t"  /* our luma data, as 0Y0Y 0Y0Y..  */
	     "  mov   "_dstp",     %%"REGDI" \n\t"  /* the destination line  */
#if !defined(__x86_64__)
	     "  testl $1, "_SSEMMXenabledW" \n\t"  /* MMXEXTsupported? */
	     "  jz    "hLoopMMX"f           \n\t"  /* n, can't do anyway */
#endif
	     /* With SSE support we will make 8 pixels (from 8 pairs) at a time */
	     "  shrl  $1, %%"REGC"  \n\t"  /* 8 bytes a time instead of 4  */
	     "  jz    "LessThan8"f  \n"

	     ".align 16          \n"
	     ""hLoopMMXSSE":    \n\t"


	     /* handle first 2 pixels */
	     /* phi: must use movl here (x86_64, reading from table of uint_32's) */
	     "  movl   16(%%"REGSI"), %%"REGEA"  \n\t"  /* get data offset in pixels, 1st pixel pair */
	     "  movl   20(%%"REGSI"), %%"REGEB"  \r\n"  /* get data offset in pixels, 2nd pixel pair  */

	     "  movd      (%%"REGD", %%"REGA"), %%mm0  \n\t"  /* copy luma pair 0000xxYY */
	     "  punpcklwd (%%"REGD", %%"REGB"), %%mm0  \r\n"  /* 2nd luma pair, now xxxxYYYY  */
	     "  punpcklbw %%mm7, %%mm0      \n\t"  /* make words out of bytes, 0Y0Y0Y0Y */
	     "  movl      16+24(%%"REGSI"), %%"REGEA"  \n\t"  /* get data offset in pixels, 3st pixel pair */
	     "  movl      20+24(%%"REGSI"), %%"REGEB"  \r\n"  /* get data offset in pixels, 4nd pixel pair  */
	     "  pmaddwd   (%%"REGSI"), %%mm0  \n\t"  /* mult and sum lumas by ctl weights */
	     "  paddusw    %%mm6, %%mm0       \n\t"  /* round */
	     "  psrlw         $8, %%mm0       \n\t"  /* right just 4 luma pixel value 0Y0Y0Y0Y */

	     /* handle 3rd and 4th pixel pairs */
	     "  movd      (%%"REGD", %%"REGA"), %%mm1  \n\t"  /* copy luma pair 0000xxYY */
	     "  punpcklwd (%%"REGD", %%"REGB"), %%mm1  \r\n"  /* 2nd luma pair, now xxxxYYYY  */
	     "  punpcklbw %%mm7, %%mm1        \n\t"  /* make words out of bytes, 0Y0Y0Y0Y */
	     "  movl      16+48(%%"REGSI"), %%"REGEA"  \n\t"  /* get data offset in pixels, 5st pixel pair */
	     "  movl      20+48(%%"REGSI"), %%"REGEB"  \r\n"  /* get data offset in pixels, 6nd pixel pair  */
	     "  pmaddwd 24(%%"REGSI"), %%mm1  \n\t"  /* mult and sum lumas by ctl weights */
	     "  paddusw    %%mm6, %%mm1       \n\t"  /* round */
	     "  psrlw         $8, %%mm1       \n\t"  /* right just 4 luma pixel value 0Y0Y0Y0Y */

	     /* handle 5th and 6th pixel pairs */
	     "  movd      (%%"REGD", %%"REGA"), %%mm2  \n\t"  /* copy luma pair 0000xxYY */
	     "  punpcklwd (%%"REGD", %%"REGB"), %%mm2  \r\n"  /* 2nd luma pair, now xxxxYYYY  */
	     "  punpcklbw %%mm7, %%mm2        \n\t"  /* make words out of bytes, 0Y0Y0Y0Y */
	     "  movl      16+72(%%"REGSI"), %%"REGEA"  \n\t"  /* get data offset in pixels, 7st pixel pair */
	     "  movl      20+72(%%"REGSI"), %%"REGEB"  \r\n"  /* get data offset in pixels, 8nd pixel pair  */
	     "  pmaddwd 48(%%"REGSI"), %%mm2  \n\t"  /* mult and sum lumas by ctl weights */
	     "  paddusw    %%mm6, %%mm2       \n\t"  /* round */
	     "  psrlw         $8, %%mm2       \n\t"  /* right just 4 luma pixel value 0Y0Y0Y0Y */

	     /* handle 7th and 8th pixel pairs */
	     "  movd      (%%"REGD", %%"REGA"), %%mm3  \n\t"  /* copy luma pair 0000xxYY */
	     "  punpcklwd (%%"REGD", %%"REGB"), %%mm3  \r\n"  /* 2nd luma pair, now xxxxYYYY  */
	     "  punpcklbw %%mm7, %%mm3        \n\t"  /* make words out of bytes, 0Y0Y0Y0Y */
	     "  pmaddwd 72(%%"REGSI"), %%mm3  \n\t"  /* mult and sum lumas by ctl weights */
	     "  paddusw    %%mm6, %%mm3       \n\t"  /* round */
	     "  psrlw         $8, %%mm3       \n\t"  /* right just 4 luma pixel value 0Y0Y0Y0Y */

	     /* combine, store, and loop */
	     "  packuswb %%mm1, %%mm0         \n\t"  /* pack into qword, 0Y0Y0Y0Y */
	     "  packuswb %%mm3, %%mm2         \n\t"  /* pack into qword, 0Y0Y0Y0Y */
	     "  packuswb %%mm2, %%mm0         \n\t"  /* and again into  YYYYYYYY */			
#ifdef STREAMING_STORE
	     "  movntq   %%mm0, (%%"REGDI")   \n\t"  /* done with 4 pixels */
#else
	     "  movq     %%mm0, (%%"REGDI")   \n\t"  /* done with 4 pixels */
#endif

	     "  lea  96(%%"REGSI"), %%"REGSI" \n\t"
	     "  lea   8(%%"REGDI"), %%"REGDI" \n\t"
	     "  decl  %%"REGC"                \n\t"
	     "  jg    "hLoopMMXSSE"b    \n\t"   /* loop for more  */
#ifdef STREAMING_STORE
	     "  sfence                  \n"
#endif
	     ""LessThan8":    \n\t"
	     "  movl "_row_size", %%"REGC"  \n\t"
	     "  andl          $7, %%"REGC"  \n\t"  /* we have done all but maybe this */
	     "  shrl          $2, %%"REGC"  \n\t"  /* now do only 4 bytes at a time */
	     "  jz            "LessThan4"f  \n"

	     ".align 16   \n"
	     ""hLoopMMX":    \n\t"

	     /* handle first 2 pixels */
	     "  movl   16(%%"REGSI"), %%"REGEA"  \n\t"  /* get data offset in pixels, 1st pixel pair */
	     "  movl   20(%%"REGSI"), %%"REGEB"  \r\n"  /* get data offset in pixels, 2nd pixel pair  */
	     "  movd      (%%"REGD", %%"REGA"), %%mm0  \n\t"  /* copy luma pair 0000xxYY */
	     "  punpcklwd (%%"REGD", %%"REGB"), %%mm0  \r\n"  /* 2nd luma pair, now xxxxYYYY  */
	     "  punpcklbw %%mm7, %%mm0      \n\t"  /* make words out of bytes, 0Y0Y0Y0Y */
	     "  movl      16+24(%%"REGSI"), %%"REGEA"  \n\t"  /* get data offset in pixels, 3st pixel pair */
	     "  movl      20+24(%%"REGSI"), %%"REGEB"  \r\n"  /* get data offset in pixels, 4nd pixel pair  */
	     "  pmaddwd   (%%"REGSI"), %%mm0  \n\t"  /* mult and sum lumas by ctl weights */
	     "  paddusw    %%mm6, %%mm0       \n\t"  /* round */
	     "  psrlw         $8, %%mm0       \n\t"  /* right just 4 luma pixel value 0Y0Y0Y0Y */

	     /* handle 3rd and 4th pixel pairs */
	     "  movd      (%%"REGD", %%"REGA"), %%mm1  \n\t"  /* copy luma pair 0000xxYY */
	     "  punpckldq (%%"REGD", %%"REGB"), %%mm1  \r\n"  /* 2nd luma pair, now xxxxYYYY  */
	     "  punpcklbw %%mm7, %%mm1        \n\t"  /* make words out of bytes, 0Y0Y0Y0Y */
	     "  pmaddwd 24(%%"REGSI"), %%mm1  \n\t"  /* mult and sum lumas by ctl weights */
	     "  paddusw    %%mm6, %%mm1       \n\t"  /* round */
	     "  psrlw         $8, %%mm1       \n\t"  /* right just 4 luma pixel value 0Y0Y0Y0Y */

	     /* combine, store, and loop */
	     "  packuswb %%mm1, %%mm0         \n\t"  /* pack into qword, 0Y0Y0Y0Y */
	     "  packuswb %%mm7, %%mm0         \n\t"  /* and again into  0000YYYY */			
	     "  movd     %%mm0, (%%"REGDI")   \n\t"  /* done with 4 pixels */
	     "  lea  48(%%"REGSI"), %%"REGSI" \n\t"
	     "  lea   4(%%"REGDI"), %%"REGDI" \n\t"

	     "  loop   "hLoopMMX"b            \n"    /* loop for more */
		 
	     /* test to see if we have a mod 4 size row, if not then more spare change */
	     ""LessThan4":    \n\t"
	     "  movl "_row_size", %%"REGC"    \n\t"
	     "  andl          $3, %%"REGC"    \n\t"  /* remainder side mod 4 */
	     "  cmpl          $2, %%"REGC"    \n\t"  
	     "  jl            "LastOne"f      \n\t"  /* none, none */

	     /* handle 2 more pixels */
	     "  movl      16(%%"REGSI"), %%"REGEA"  \n\t"  /* get data offset in pixels, 1st pixel pair */
	     "  movl      20(%%"REGSI"), %%"REGEB"  \r\n"  /* get data offset in pixels, 2nd pixel pair  */
	     "  movd      (%%"REGD", %%"REGA"), %%mm0  \n\t"  /* copy luma pair 0000xxYY */
	     "  punpcklwd (%%"REGD", %%"REGB"), %%mm0  \r\n"  /* 2nd luma pair, now xxxxYYYY  */
	     "  punpcklbw %%mm7, %%mm0        \n\t"  /* make words out of bytes, 0Y0Y0Y0Y */

	     "  pmaddwd   (%%"REGSI"), %%mm0  \n\t"  /* mult and sum lumas by ctl weights */
	     "  paddusw   %%mm6, %%mm0        \n\t"  /* round */
	     "  psrlw        $8, %%mm0        \n\t"  /* right just 4 luma pixel value 0Y0Y0Y0Y */
	     "  packuswb  %%mm7, %%mm0        \n\t"  /* pack into qword, 00000Y0Y */
	     "  packuswb  %%mm7, %%mm0        \n\t"  /* and again into  000000YY */			
	     "  movd      %%mm0, (%%"REGDI")  \n\t"  /* store, we are guarrenteed room in buffer (8 byte mult) */
	     "  subl         $2, %%"REGC"     \n\t"  
	     
	     "  lea  24(%%"REGSI"), %%"REGSI" \n\t" /* bump to next control bytes */
	     "  lea   2(%%"REGDI"), %%"REGDI" \n" /* bump to next output pixel addr */

	     /* maybe one last pixel */
	     ""LastOne":   \n\t"
	     "  cmpl   $0, %%"REGC"   \r\n"  /* still more ? */
	     "  jz     "AllDone"f     \r\n"  /* n, done */
	     "  movl   16(%%"REGSI"), %%"REGEA"     \n\t"  /* get data offset in pixels, 1st pixel pair */
	     "  movd   (%%"REGD", %%"REGA"), %%mm0  \n\t"  /* copy luma pair 0000xxYY */
	     "  punpcklbw %%mm7, %%mm0        \n\t"  /* make words out of bytes, 0Y0Y0Y0Y */

	     "  pmaddwd   (%%"REGSI"), %%mm0  \n\t"  /* mult and sum lumas by ctl weights */
	     "  paddusw    %%mm6, %%mm0       \n\t"  /* round */
	     "  psrlw         $8, %%mm0       \n\t"  /* right just 4 luma pixel value 0Y0Y0Y0Y */
	     "  movd       %%mm0, %%"REGEA"   \n\t"
	     "  movb        %%al, (%%"REGDI") \n"    /* store last one */
			 
	     ""AllDone":  \n\t"
	     "  emms      \n\t"
#if !defined(__x86_64__)
	     "mov "_oldbx", %%"REGB" \n\t"
#endif
	     ::
	     "m" /*0*/(FPround1),
	     "m" /*1*/(vWeight1),
	     "m" /*2*/(vWeight2),
	     "m" /*3*/(y/*YMask[0]*/),
	     "m" /*4*/(src_row_size),
	     "m" /*5*/(y/*EndOffset*/),
	     "m" /*6*/(pControl),
	     "m" /*7*/(row_size),
	     "m" /*8*/(vWorkYW),
	     "m" /*9*/(dstp),
	     "m" /*10*/(y/*vWorkUVW*/),
	     "m" /*11*/(FPround2),
	     "m" /*12*/(srcp1),
	     "m" /*13*/(srcp2)
#if !defined(__x86_64__)
	     ,
	     "m" /*14*/(oldbx),
	     "m" /*15*/(SSEMMXenabledW),
	     "m" /*16*/(SSE2enabledW)
	     : REGA, /*REGB,*/ REGC, REGD, REGSI, REGDI
#else
	     : REGA, REGB, REGC, REGD, REGSI, REGDI
#endif
	     );

    dstp += dst_pitch;
  }
#endif
  return 0; 
}

/*
 * xine plugin
 */

#define PLUGIN_ID     "warp"
#define PLUGIN_DESCR  "(non-)linear software scaling post plugin";
#define PLUGIN_T      warp_plugin_t
/*#define POST_THREADS*/
/*#define POST_SLICES*/
#include "xine/post_util.h"

/* plugin instance functions */
static void        warp_dispose(post_plugin_t *this_gen);

/* parameter functions */
static xine_post_api_descr_t *warp_get_param_descr(void);
static int                    warp_set_parameters(xine_post_t *this_gen, void *param_gen);
static int                    warp_get_parameters(xine_post_t *this_gen, void *param_gen);
static char                  *warp_get_help(void);


typedef struct warp_parameters_s {
  int    output_width;
  int    output_height;
  double output_aspect;
  int    no_downscaling;
  int    debug;
} warp_parameters_t;

START_PARAM_DESCR(warp_parameters_t)
PARAM_ITEM(POST_PARAM_TYPE_INT,    output_width,  NULL, 640, 1920, 0,
  "output video width")
PARAM_ITEM(POST_PARAM_TYPE_INT,    output_height, NULL, 480, 1080, 0,
  "output video height")
PARAM_ITEM(POST_PARAM_TYPE_DOUBLE, output_aspect, NULL,   1,    3, 0,
  "output video aspect ratio")
PARAM_ITEM(POST_PARAM_TYPE_BOOL,   no_downscaling,NULL,   0,    1, 0,
  "disable downscaling")
PARAM_ITEM(POST_PARAM_TYPE_BOOL,   debug,         NULL,   0,    1, 0,
  "debug mode")
END_PARAM_DESCR(warp_param_descr)


typedef struct {
  post_plugin_t  post;

  xine_post_in_t parameter_input;

  /* User config  (changes to actual config are delayed) */
  warp_parameters_t config;

  /* Current config */
  int    enable;
  int    output_width;
  int    output_height;
  double output_aspect;
  double factor_x;
  double factor_y;

  /* Last seen input frame */
  int    input_width;
  int    input_height;
  int    input_format;
  int    input_interlaced;
  double input_aspect;

  /* working buffers */
  uint32_t *vWorkY;
  uint32_t *vWorkUV;

  /* scaling tables */
  uint32_t *hControl;
  uint32_t *hControlUV;
  uint32_t *vOffsets;
  uint32_t *vOffsetsUV;
  uint32_t *vWeights;
  uint32_t *vWeightsUV;

  /* memory for work areas and scaling tables */
  void *pMem;

} warp_plugin_t;

/*
 *
 */

static void init_tables(warp_plugin_t *this)
{
#define BP(x) ((uint8_t*)(x))
  /* allocate memory for scaling tables and workspace */
  free(this->pMem);
  this->pMem = malloc(this->input_width*3 + this->output_width*sizeof(uint32_t)*3*2 +
		      this->output_height*sizeof(uint32_t)*4 + 2*9*128);

  /* - aligned for P4 cache line */
  this->vWorkY   = (uint32_t*)ALIGN(128, this->pMem);
  this->vWorkUV  = (uint32_t*)ALIGN(128, BP(this->vWorkY)   + this->input_width*2 + 128);
  this->hControl = (uint32_t*)ALIGN(128, BP(this->vWorkUV)  + this->input_width   + 128);
  this->vOffsets = (uint32_t*)ALIGN(128, BP(this->hControl) + this->output_width  * sizeof(uint32_t) * 3 + 128);
  this->vWeights = (uint32_t*)ALIGN(128, BP(this->vOffsets) + this->output_height * sizeof(uint32_t) + 128);

  if (this->input_format == XINE_IMGFMT_YV12) {
    this->vOffsetsUV = (uint32_t*)ALIGN(128, BP(this->vWeights)   + this->output_height * sizeof(uint32_t) + 128);
    this->vWeightsUV = (uint32_t*)ALIGN(128, BP(this->vOffsetsUV) + this->output_height * sizeof(uint32_t) + 128);
    this->hControlUV = (uint32_t*)ALIGN(128, BP(this->vWeightsUV) + this->output_height * sizeof(uint32_t) + 128);

    DBG("init_yv12: %dx%d->%dx%d hWarp %1.3lf vWarp %1.3lf\n",
        this->input_width,  this->input_height,
        this->output_width, this->output_height,
        this->factor_x, this->factor_y);

    init_tables_yv12(this->output_width, this->output_height,
		     this->input_width,  this->input_height,
		     this->input_interlaced, this->factor_x, this->factor_y, 
		     this->hControl,   this->vOffsets,   this->vWeights,
		     this->hControlUV, this->vOffsetsUV, this->vWeightsUV );

  } else if (this->input_format == XINE_IMGFMT_YUY2) {

    DBG("init_yuy2: %dx%d->%dx%d hWarp %1.3lf vWarp %1.3lf\n",
        this->input_width,  this->input_height,
        this->output_width, this->output_height,
        this->factor_x, this->factor_y);

    init_tables_yuy2(this->output_width, this->output_height,
		     this->input_width,  this->input_height,
		     this->input_interlaced, this->factor_x, this->factor_y, 
		     this->hControl, this->vOffsets, this->vWeights );
  }
}

static void calculate_factors(warp_plugin_t *this)
{
  /* try to guess amount to stretch/shrink */
  double adiff = this->input_aspect - this->output_aspect;
  this->factor_x = 1.0;
  this->factor_y = 1.0;

  if (adiff > 0.1) {

    if (adiff > 0.1 + ((16.0-12.0)/9.0)) {
      /* >16:9 -> >4:3 */
      DBG("aspect ratio diff %1.3lf > 0 : too large !\n", adiff);
      this->factor_x = 0.95;
      this->factor_y = 1.15;
      this->output_aspect += (adiff - 4.0/9.0);
      DBG("  changing target ratio to %3.1lf\n", this->output_aspect);
    } else {
      /* 16:9 ... 12:9 -> 4:3 */
      DBG("aspect ratio diff %1.3lf > 0 : 16.9...12:9 -> 4:3\n", adiff);
      this->factor_x = 1.0 - 0.05 * adiff * 9.0/4.0;
      this->factor_y = 1.0 + 0.15 * adiff * 9.0/4.0;
    }

  } else if (adiff < -0.1) {

    if(adiff < -0.1-((16.0-12.0)/9.0)) {
      /* <4:3 -> <16:9 */
      DBG("aspect ratio diff %1.3lf > 0 : too large !\n", adiff);
      this->factor_x = 1.05;
      this->factor_y = 0.85;
      this->output_aspect += (adiff + 4.0/9.0);
      DBG("  changing target ratio to %3.1lf\n", this->output_aspect);
    } else {
      /* 4:3...16:9 -> 16:9 */
      DBG("aspect ratio diff %1.3lf < 0 : 4:3...16:9 -> 16:9\n", adiff);
      this->factor_x = 1.0 + 0.05 * adiff * 9.0/4.0;
      this->factor_y = 1.0 - 0.15 * adiff * 9.0/4.0;
    }

  } else {
    DBG("aspect ratio matches, no warp\n");
    this->factor_x = 1.0;
    this->factor_y = 1.0;
  }

  DBG("factor_x = %1.3lf factor_y = %1.3lf  output ratio = %1.3lf\n", 
      this->factor_x, this->factor_y, this->output_aspect);
}

/*
 *
 */

void *warp_init_plugin(xine_t *xine, void *data)
{
#if !defined(__x86_64__)
  /* Need at least MMX */
  if (!(xine_mm_accel() & MM_ACCEL_X86_MMX)) {
    fprintf(stderr, "warp_init_plugin: ERROR: at least MMX required\n");
    return NULL;
  }
#endif

  return init_plugin(xine, data);
}

static post_plugin_t *open_plugin(post_class_t *class_gen, int inputs,
					 xine_audio_port_t **audio_target,
					 xine_video_port_t **video_target)
{
  warp_plugin_t     *this     = calloc(1, sizeof(warp_plugin_t));
  post_plugin_t     *this_gen = (post_plugin_t *) this;
  post_in_t         *input;
  post_out_t        *output;
  xine_post_in_t    *input_param;
  post_video_port_t *port;

  static xine_post_api_t post_api =
    { warp_set_parameters,  warp_get_parameters, warp_get_param_descr, warp_get_help };
  
  if (!this || !video_target || !video_target[0]) {
    free(this);
    return NULL;
  }
  
  _x_post_init(this_gen, 0, 1);
  
  port = _x_post_intercept_video_port(this_gen, video_target[0], &input, &output);
  port->intercept_frame = intercept_frame_yuy;
  port->new_frame->draw = post_draw;
  input->xine_in.name   = "video";
  output->xine_out.name = "video (scaled)";
  this_gen->xine_post.video_input[0] = &port->new_port;

  this_gen->dispose = warp_dispose;

  input_param       = &this->parameter_input;
  input_param->name = "parameters";
  input_param->type = XINE_POST_DATA_PARAMETERS;
  input_param->data = &post_api;
  xine_list_push_back(this_gen->input, input_param);

  this->config.output_aspect  = 0.0; /* -> do not change aspect ratio */
  this->config.output_width   = 0;   /* -> do not change width */
  this->config.output_height  = 0;   /* -> do not change height */
  this->config.no_downscaling = 0;

  this->input_width  = 0; /* not known yet, triggers initialization later */
  this->input_height = 0;

  return this_gen;
}

static void warp_dispose(post_plugin_t *this_gen)
{
  if (_x_post_dispose(this_gen)) {
    warp_plugin_t *this = (warp_plugin_t *) this_gen;

    DBG("dispose\n");

    free(this->pMem);
    free(this);
  }
}

static vo_frame_t *got_frame(vo_frame_t *frame)
{
  post_video_port_t *port = (post_video_port_t *)frame->port;
  warp_plugin_t     *this = (warp_plugin_t *)port->post;
  double             adiff = this->input_aspect - frame->ratio;

  if (this->input_width != frame->width || this->input_height != frame->height ||
      this->input_format != frame->format || FABS(adiff)>0.1 ||
      this->input_interlaced != !!(frame->flags & VO_INTERLACED_FLAG)) {

    DBG("detected frame format change: %dx%d -> %dx%d, interlaced %d->%d, aspect %1.3lf->%1.3lf, %s->%s\n",
	this->input_width, this->input_height, frame->width, frame->height,
	this->input_interlaced, !!(frame->flags & VO_INTERLACED_FLAG),
	this->input_aspect, frame->ratio, 
	this->input_format==XINE_IMGFMT_YV12 ? "yv12":"yuy2", 
	frame->format==XINE_IMGFMT_YV12 ? "yv12":"yuy2" );

    /* free tables and buffers */
    free(this->pMem);
    this->pMem = NULL;

    /* remember frame properties to detect changes in video format */
    this->input_width  = frame->width;
    this->input_height = frame->height;
    this->input_format = frame->format;
    this->input_aspect = frame->ratio;
    this->input_interlaced = !!(frame->flags & VO_INTERLACED_FLAG);

    /* re-configure target size and aspect ratio */
    if (doublecmp(this->config.output_aspect, 0.0) == 0)
      this->output_aspect = frame->ratio;
    else
      this->output_aspect = this->config.output_aspect;
    if (!this->config.no_downscaling) {
      this->output_width  = this->config.output_width  ?: frame->width;
      this->output_height = this->config.output_height ?: frame->height;
    } else {
      this->output_width  = MAX(this->config.output_width,  frame->width);
      this->output_height = MAX(this->config.output_height, frame->height);
    }

    /* calculate warp function factors */
    calculate_factors(this);

    adiff = this->input_aspect - this->output_aspect;
    if(this->output_width  == frame->width &&
       this->output_height == frame->height &&
       adiff < 0.1  && 
       adiff > -0.1 ) {
      this->enable = 0;
      DBG("--> nothing to do, disabling processing for now\n");
      return NULL;
    }

    this->enable = 1;

    init_tables(this);
  }

  if (!this->enable)
    return NULL;

  return port->original_port->get_frame(port->original_port,
					this->output_width, this->output_height, 
					this->output_aspect, frame->format,
					frame->flags | VO_BOTH_FIELDS);
}

static void draw_internal(vo_frame_t *frame, vo_frame_t *new_frame)
{
  post_video_port_t *port = (post_video_port_t *)frame->port;
  warp_plugin_t *this = (warp_plugin_t *)port->post;
  int proc_height = frame->height;

  if (frame->format == XINE_IMGFMT_YV12) {

    do_warp_yv12(new_frame->base[0], frame->base[0], 
		 new_frame->pitches[0], frame->pitches[0],
		 this->output_width, this->output_height,
		 frame->width, proc_height,
		 this->input_interlaced,
		 this->hControl, this->vOffsets, this->vWeights,
		 this->vWorkY,
		 0);
    proc_height /= 2;
    do_warp_yv12(new_frame->base[1], frame->base[1], 
		 new_frame->pitches[1], frame->pitches[1],
		 this->output_width/2, this->output_height/2,
		 frame->width/2, proc_height, 
		 this->input_interlaced,
		 this->hControlUV, this->vOffsetsUV, this->vWeightsUV,
		 this->vWorkUV,
		 0);
    do_warp_yv12(new_frame->base[2], frame->base[2], 
		 new_frame->pitches[2], frame->pitches[2],
		 this->output_width/2, this->output_height/2,
		 frame->width/2, proc_height, 
		 this->input_interlaced,
		 this->hControlUV, this->vOffsetsUV, this->vWeightsUV,
		 this->vWorkUV,
		 0);

  } else if (frame->format == XINE_IMGFMT_YUY2) {
    do_warp_yuy2(new_frame->base[0], frame->base[0], 
		 new_frame->pitches[0], frame->pitches[0],
		 this->output_width, this->output_height,
		 frame->width, proc_height, 
		 this->input_interlaced,
		 this->hControl, this->vOffsets, this->vWeights,
		 this->vWorkY, this->vWorkUV, 
		 0);
  }
}

/*
 * parameter functions
 */

static xine_post_api_descr_t *warp_get_param_descr(void)
{
  return &warp_param_descr;
}

static int warp_set_parameters(xine_post_t *this_gen, void *param_gen)
{
  warp_plugin_t *this = (warp_plugin_t *)this_gen;
  warp_parameters_t *params = (warp_parameters_t *)param_gen;

  memcpy(&this->config, params, sizeof(warp_parameters_t));  
  this->input_width = this->input_height = 0;

  if(this->config.output_aspect > 999)
    this->config.output_aspect /= 1000.0;

  DBG("warp_set_parameters: "
      "output_width=%d, output_height=%d, output_aspect=%4.3lf, no_downscaling=%d, debug=%d\n",
      this->config.output_width, this->config.output_height, this->config.output_aspect,
      this->config.no_downscaling, this->config.debug);

  return 1;
}

static int warp_get_parameters(xine_post_t *this_gen, void *param_gen)
{
  warp_plugin_t *this = (warp_plugin_t *)this_gen;
  warp_parameters_t *params = (warp_parameters_t *)param_gen;
  
  DBG("warp_get_parameters\n");
  memcpy(params, &this->config, sizeof(warp_parameters_t));

  return 1;
}

static char *warp_get_help(void) {
  return _(
	   "The warp plugin scales video to another resolution. "
           "It supports non-linear stretching to change video aspect ratio. "
	   "\n"
           "Parameters\n"
	   "  output_width:       Scale video to width\n"
	   "                      (0 -> do not change video width)\n"
	   "  output_height:      Scale video to height\n"
	   "                      (0 -> do not change video height)\n"
	   "  output_aspect:      Adjust aspect ratio using non-linear scaling\n"
	   "                      (0 -> do not change video aspect ratio)\n"
	   "  no_downscaling:     Do not downscale video\n"
           "  debug:              Output debug messages\n"
           "\n"
         );
}


/*
 * plugin info
 */

static post_info_t info = { XINE_POST_TYPE_VIDEO_FILTER };

const plugin_info_t xine_plugin_info[] __attribute__((visibility("default"))) =
{
  /* type, API, "name", version, special_info, init_function */
  { PLUGIN_POST, POST_PLUGIN_IFACE_VERSION, "warp",    XINE_VERSION_CODE, &info, &warp_init_plugin },
  { PLUGIN_POST, POST_PLUGIN_IFACE_VERSION, "swscale", XINE_VERSION_CODE, &info, &warp_init_plugin },
  { PLUGIN_NONE, 0, "", 0, NULL, NULL }
};
