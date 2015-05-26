/*
 * vo_props.h: Extended video-out capabilities and properties
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: vo_props.h,v 1.5 2010-05-30 20:31:18 phintuka Exp $
 *
 */

#ifndef XINELIBOUTPUT_VO_PROPS_H_
#define XINELIBOUTPUT_VO_PROPS_H_

/*
 * Extended vo capabilities
 */

/* output can scale OSD */
#ifdef VO_CAP_CUSTOM_EXTENT_OVERLAY
  /* xine-lib 1.2 */
# define VO_XCAP_OSDSCALING  VO_CAP_CUSTOM_EXTENT_OVERLAY
#else
# define VO_XCAP_OSDSCALING  0x01000000
#endif

/* Output can blend ARGB surfaces */
#ifdef VO_CAP_ARGB_LAYER_OVERLAY
# define VO_XCAP_ARGB_LAYER_OVERLAY  VO_CAP_ARGB_LAYER_OVERLAY
#else
# define VO_XCAP_ARGB_LAYER_OVERLAY  0x02000000
#endif

#ifdef VO_CAP_VIDEO_WINDOW_OVERLAY
# define VO_XCAP_VIDEO_WINDOW_OVERLAY VO_CAP_VIDEO_WINDOW_OVERLAY
#else
# define VO_XCAP_VIDEO_WINDOW_OVERLAY 0x04000000
#endif

/*
 * Extended vo properties
 */

/* enable/disable OSD scaling */
#define VO_PROP_OSD_SCALING 0x1001


/*
 * VDR OSD tagging and extra data
 */

/* VDR OSD , stored in overlay hili_rgb_clut member */
#define VDR_OSD_MAGIC       -9999

/* VDR OSD extra data, stored in overlay hili clut data */
typedef struct {
  /* extent of reference coordinate system */
  uint16_t extent_width;
  uint16_t extent_height;
  /* overlay layer */
  uint32_t layer;
  /* scaling: 0 - disable , 1...N - quality */
  uint8_t scaling;
} vdr_osd_extradata_t;

#endif /* XINELIBOUTPUT_VO_PROPS_H_ */
