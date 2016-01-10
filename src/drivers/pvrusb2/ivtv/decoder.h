/*
    saa7115 API header
    Copyright (C) 2003-2004  Kevin Thayer <nufan_wfk at yahoo.com>

    VBI additions:
    Copyright (C) 2004  Hans Verkuil <hverkuil@xs4all.nl>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef IVTV_DECODER_H_
#define IVTV_DECODER_H_

struct decoder_lcr {
    int raw;            /* 0 = sliced VBI, 1 = raw VBI */
    u8  lcr[24];        /* VBI settings for first (odd, high nibble) and second
                           (even, low nibble) fields, lines 0-23. Lines 0-5 are never used. */
};

struct decoder_state {
	int norm;
	int input;
	int enable;
	int bright;
	int contrast;
	int hue;
	int sat;
	int playback;
	int audio;
	int audio_input; /* some decoders may also handle audio input switching */
	struct decoder_lcr vbi_mode;
};

struct decoder_reg {
	u32 reg;
	u32 val;
};

struct decode_vbi_line {
        u32 is_even_field;
        u8 *p;
        u32 line;
        u32 type;
};

/* Possible audio bit rates for DECODER_SET_AUDIO */
#define DECODER_AUDIO_441_KHZ	0
#define DECODER_AUDIO_48_KHZ	1
#define DECODER_AUDIO_32_KHZ	2

/* Need to be able to set the audio bitrates */
#define DECODER_SET_AUDIO       _IOW('d', 100, int)
/* Digitizer Reset */
#define DECODER_RESET           _IO ('d', 101)
#define DECODER_SET_VBI         _IOW('d', 102, struct decoder_lcr)
#define DECODER_SET_SIZE        _IOW('d', 103, struct video_window)
#define	DECODER_GET_STATE       _IOR('d', 104, struct decoder_state)
#define	DECODER_SET_REG         _IOR('d', 105, struct decoder_reg)
#define	DECODER_GET_REG         _IOWR('d', 106, struct decoder_reg)
#define DECODER_GET_WSS         _IOR('d', 107, int *)
#define DECODER_GET_CC_ODD      _IOR('d', 108, int *)
#define DECODER_GET_CC_EVEN     _IOR('d', 109, int *)
#define DECODER_SET_AUDIO_INPUT _IOW('d', 110, int *)
#define DECODER_GET_VBI         _IOR('d', 111, struct decoder_lcr)
#define DECODER_DECODE_VBI_LINE _IOWR('d', 112, struct decode_vbi_line)
#define DECODER_LOG_STATUS      _IO ('d', 113)
#define	DECODER_SET_STATE       _IOW('d', 114, struct decoder_state)

/* range of 200 and up is reserved for decoder-specific commands. */

#endif /* IVTV_DECODER_H_ */
