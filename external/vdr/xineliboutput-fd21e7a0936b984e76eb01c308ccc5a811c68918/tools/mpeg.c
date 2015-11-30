/*
 * mpeg.c:
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: mpeg.c,v 1.5 2012-12-31 16:42:45 phintuka Exp $
 *
 */

#include <inttypes.h>
#include <string.h>

#include "mpeg.h"


const char * const picture_type_str[] = {
  "(none)",
  "I-Frame",
  "B-Frame",
  "P-Frame"
};

int mpeg2_get_picture_type(const uint8_t *buf, int len)
{
  int i;
  for (i = 0; i < len-5; i++)
    if (IS_SC_PICTURE(buf + i))
      return (buf[i + 5] >> 3) & 0x07;

  return NO_PICTURE;
}

int mpeg2_is_sequence_header(const uint8_t *buf, int len)
{
  int i;
  for (i = 0; i < len-6; i++) {
    if (IS_SC_SEQUENCE(buf + i)) {
      return 1;
    }
  }
  return 0;
}

int mpeg2_get_video_size(const uint8_t *buf, int len, video_size_t *size)
{
  int i;
  for (i = 0; i < len-6; i++) {
    if (IS_SC_SEQUENCE(buf + i)) {
	static const mpeg_rational_t mpeg2_aspect[16] = {
	  {0,1}, {1,1}, {4,3}, {16,9}, {221,100},
	  {0,1}, {0,1}, {0,1}, { 0,1}, {  0,1},
	  {0,1}, {0,1}, {0,1}, { 0,1}, {  0,1},
	  {0,1},
	};

	int d = (buf[i+4] << 16) | (buf[i+5] << 8) | buf[i+6];
	int a = buf[i+7] >> 4;

	size->width  = (d >> 12);
	size->height = (d & 0xfff);

	memcpy(&size->pixel_aspect, &mpeg2_aspect[a], sizeof(mpeg_rational_t));
	size->pixel_aspect.num *= size->height;
	size->pixel_aspect.den *= size->width;

	return 1;
      }
  }
  return 0;
}

