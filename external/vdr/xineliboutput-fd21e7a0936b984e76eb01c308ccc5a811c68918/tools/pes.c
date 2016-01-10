/*
 * pes.h: PES header definitions
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: pes.c,v 1.11 2012-08-30 08:36:39 phintuka Exp $
 *
 */

#include <inttypes.h>
#include <string.h>

#include "../logdefs.h"

#include "mpeg.h"
#include "h264.h"

#include "pes.h"

static int64_t parse_timestamp(const uint8_t *buf)
{
  int64_t ts;
  ts  = ((int64_t)( buf[0] & 0x0E)) << 29 ;
  ts |=  (int64_t)( buf[1]         << 22 );
  ts |=  (int64_t)((buf[2] & 0xFE) << 14 );
  ts |=  (int64_t)( buf[3]         <<  7 );
  ts |=  (int64_t)((buf[4] & 0xFE) >>  1 );
  return ts;
}

int64_t pes_get_pts(const uint8_t *buf, int len)
{
  /* assume mpeg2 pes header ... */
  if (IS_VIDEO_PACKET(buf) || IS_AUDIO_PACKET(buf)) {

    if ((buf[6] & 0xC0) != 0x80)
      return NO_PTS;
    if ((buf[6] & 0x30) != 0)
      return NO_PTS;

    if ((len > 13) && (buf[7] & 0x80)) { /* pts avail */
      return parse_timestamp(buf + 9);
    }
  }
  return NO_PTS;
}

int64_t pes_get_dts(const uint8_t *buf, int len)
{
  if (IS_VIDEO_PACKET(buf) || IS_AUDIO_PACKET(buf)) {

    if ((buf[6] & 0xC0) != 0x80)
      return NO_PTS;
    if ((buf[6] & 0x30) != 0)
      return NO_PTS;

    if (len > 18 && (buf[7] & 0x40)) { /* dts avail */
      return parse_timestamp(buf + 14);
    }
  }
  return NO_PTS;
}

void pes_change_pts(uint8_t *buf, int len, int64_t new_pts)
{
  /* assume mpeg2 pes header ... Assume header already HAS pts */
  if (IS_VIDEO_PACKET(buf) || IS_AUDIO_PACKET(buf)) {

    if ((buf[6] & 0xC0) != 0x80)
      return;
    if ((buf[6] & 0x30) != 0)
      return;

    if ((len > 13) && (buf[7] & 0x80)) { /* pts avail */
      buf[ 9] = ((new_pts >> 29) & 0x0E) | (buf[ 9] & 0xf1);
      buf[10] = ((new_pts >> 22) & 0xFF);
      buf[11] = ((new_pts >> 14) & 0xFE) | (buf[11] & 0x01);
      buf[12] = ((new_pts >> 7 ) & 0xFF);
      buf[13] = ((new_pts << 1 ) & 0xFE) | (buf[13] & 0x01);
    }
  }
}

int pes_strip_pts_dts(uint8_t *buf, int size)
{
  if(size > 13 && buf[7] & 0x80) { /* pts avail */
    int n = 5;
    int pes_len = (buf[4] << 8) | buf[5];
    if ((buf[6] & 0xC0) != 0x80)
      return size;
    if ((buf[6] & 0x30) != 0) /* scrambling control */
      return size;
    /* dts too ? */
    if(size > 18 && buf[7] & 0x40)
      n += 5;
    pes_len -= n;     /* update packet len */
    buf[4]   = pes_len >> 8;   /* packet len (hi) */
    buf[5]   = pes_len & 0xff; /* packet len (lo) */
    buf[7]  &= 0x3f;  /* clear pts and dts flags */
    buf[8]  -= n;     /* update header len */
    memmove(buf+4+n, buf+9+n, size-9-n);
    return size - n;
  }
  return size;
}

int pes_is_frame_h264(const uint8_t *buf, int len)
{
  if (len < 9 || len < 9 + buf[8])
    return 0;
  if ( (buf[6] & 0xC0) != 0x80)  /* MPEG 2 PES */
    return 0;

  buf += 9 + buf[8];

  if (IS_NAL_AUD(buf))
    return 1;
  return 0;
}

uint8_t pes_get_picture_type(const uint8_t *buf, int len)
{
  int i = PES_HEADER_LEN(buf);

  buf += i;
  len -= i;

  if (buf[0] == 0x00 && buf[1] == 0x00 && buf[2] == 0x01) {
    if (buf[3] == NAL_AUD)
      return h264_get_picture_type(buf, len);
    return mpeg2_get_picture_type(buf, len);
  }

  return NO_PICTURE;
}

int pes_get_video_size(const uint8_t *buf, int len, video_size_t *size, int h264)
{
  int i = PES_HEADER_LEN(buf);

  buf += i;
  len -= i;

  if (h264 || IS_NAL_AUD(buf))
    return h264_get_video_size(buf, len, size);

  return mpeg2_get_video_size(buf, len, size);
}

