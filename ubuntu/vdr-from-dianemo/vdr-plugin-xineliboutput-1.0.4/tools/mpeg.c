/*
 * mpeg.c:
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: mpeg.c,v 1.1 2008/02/04 22:42:13 phintuka Exp $
 *
 */

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
  for (i = 0; i < len-5; i++) {
    if (buf[i] == 0 && buf[i + 1] == 0 && buf[i + 2] == 1) {
      switch (buf[i + 3]) {
        case SC_PICTURE:
	  return (buf[i + 5] >> 3) & 0x07;
      }
    }
  }
  return NO_PICTURE;
}

int mpeg2_get_video_size(const uint8_t *buf, int len, video_size_t *size)
{
  int i;
  for (i = 0; i < len-6; i++) {
    if (buf[i] == 0 && buf[i + 1] == 0 && buf[i + 2] == 1) {
      if (buf[i + 3] == SC_SEQUENCE) {
	int d = (buf[i+4] << 16) | (buf[i+5] << 8) | buf[i+6];
	size->width  = (d >> 12);
	size->height = (d & 0xfff);
	return 1;
      }
    }
  }
  return 0;
}

