/*
 * vo_frameoutput.h:
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: vo_frameoutput.h,v 1.1 2012-03-07 08:27:40 phintuka Exp $
 *
 */

#ifndef _XINELIBOUTPUT_VO_FRAMEOUTPUT_H
#define _XINELIBOUTPUT_VO_FRAMEOUTPUT_H

vo_driver_t *vo_frameoutput_init(void *handle, void (*cb)(void*, vo_frame_t*));

#endif /* _XINELIBOUTPUT_VO_FRAMEOUTPUT_H */
