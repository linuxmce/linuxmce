/*
 * pes.h: PES header definitions
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: pes.c,v 1.2.2.1 2008/09/26 19:08:25 phintuka Exp $
 *
 */

#include "mpeg.h"
#include "h264.h"

#include "pes.h"


int64_t pes_get_pts(const uint8_t *buf, int len)
{
  /* assume mpeg2 pes header ... */
  if (IS_VIDEO_PACKET(buf) || IS_AUDIO_PACKET(buf)) {
      
    if ((buf[6] & 0xC0) != 0x80)
      return INT64_C(-1);
    if ((buf[6] & 0x30) != 0)
      return INT64_C(-1);
      
    if ((len > 13) && (buf[7] & 0x80)) { /* pts avail */
      int64_t pts;
      pts  = ((int64_t)(buf[ 9] & 0x0E)) << 29 ;
      pts |= ((int64_t) buf[10])         << 22 ;
      pts |= ((int64_t)(buf[11] & 0xFE)) << 14 ;
      pts |= ((int64_t) buf[12])         <<  7 ;
      pts |= ((int64_t)(buf[13] & 0xFE)) >>  1 ;
      return pts;
    }
  }
  return INT64_C(-1);
}

int64_t pes_get_dts(const uint8_t *buf, int len)
{
  if (IS_VIDEO_PACKET(buf) || IS_AUDIO_PACKET(buf)) {
      
    if ((buf[6] & 0xC0) != 0x80)
      return INT64_C(-1);
    if ((buf[6] & 0x30) != 0)
      return INT64_C(-1);

    if (len > 18 && (buf[7] & 0x40)) { /* dts avail */
      int64_t dts;
      dts  = ((int64_t)( buf[14] & 0x0E)) << 29 ;
      dts |=  (int64_t)( buf[15]         << 22 );
      dts |=  (int64_t)((buf[16] & 0xFE) << 14 );
      dts |=  (int64_t)( buf[17]         <<  7 );
      dts |=  (int64_t)((buf[18] & 0xFE) >>  1 );
      return dts;
    }
  }
  return INT64_C(-1);
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

  if (!buf[0] && !buf[1] && buf[2] == 0x01 && buf[3] == 0x09)
    return 1;
  return 0;
}

uint8_t pes_get_picture_type(const uint8_t *buf, int len)
{
  int i = 8;         /* the minimum length of the video packet header */
  i += buf[i] + 1;   /* possible additional header bytes */

  buf += i;
  len -= i;

  if (!buf[0] && !buf[1] && buf[2]) {
    if (buf[3] == 0x09)
      return h264_get_picture_type(buf, len);
    else
      return mpeg2_get_picture_type(buf, len);
  }
  return NO_PICTURE;
}

int pes_get_video_size(const uint8_t *buf, int len, video_size_t *size, int h264)
{
  int i = 8;

  i += buf[i] + 1;   /* possible additional header bytes */

  buf += i;
  len -= i;

  if (h264 || (!buf[0] && !buf[1] && buf[2] == 0x01 && buf[3] == 0x09))
    return h264_get_video_size(buf, len, size);
  else
    return mpeg2_get_video_size(buf, len, size);

  return 0;
}

