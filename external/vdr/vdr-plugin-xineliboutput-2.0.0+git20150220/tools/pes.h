/*
 * pes.h: PES header definitions
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: pes.h,v 1.13 2009-07-01 09:56:26 phintuka Exp $
 *
 */

#ifndef _XINELIBOUTPUT_PES_H_
#define _XINELIBOUTPUT_PES_H_

#ifdef __cplusplus
extern "C" {
#endif


/*
 * Constants
 */

#define PES_CHUNK_SIZE 2048

#define MAX_SCR ((int64_t)0x1ffffffffLL)

#define NO_PTS  (INT64_C(-1))

/* PES PIDs */
#define PRIVATE_STREAM1   0xBD
#define PADDING_STREAM    0xBE
#define PRIVATE_STREAM2   0xBF
#define AUDIO_STREAM_S    0xC0      /* 1100 0000 */
#define AUDIO_STREAM_E    0xDF      /* 1101 1111 */
#define VIDEO_STREAM_S    0xE0      /* 1110 0000 */
#define VIDEO_STREAM_E    0xEF      /* 1110 1111 */

#define AUDIO_STREAM_MASK 0x1F  /* 0001 1111 */
#define VIDEO_STREAM_MASK 0x0F  /* 0000 1111 */
#define AUDIO_STREAM      0xC0  /* 1100 0000 */
#define VIDEO_STREAM      0xE0  /* 1110 0000 */

#define ECM_STREAM        0xF0
#define EMM_STREAM        0xF1
#define DSM_CC_STREAM     0xF2
#define ISO13522_STREAM   0xF3
#define PROG_STREAM_DIR   0xFF

#define IS_VIDEO_PACKET(data)      (VIDEO_STREAM    == ((data)[3] & ~VIDEO_STREAM_MASK))
#define IS_MPEG_AUDIO_PACKET(data) (AUDIO_STREAM    == ((data)[3] & ~AUDIO_STREAM_MASK))
#define IS_PS1_PACKET(data)        (PRIVATE_STREAM1 == (data)[3])
#define IS_PADDING_PACKET(data)    (PADDING_STREAM  == (data)[3])
#define IS_AUDIO_PACKET(data)      (IS_MPEG_AUDIO_PACKET(data) || IS_PS1_PACKET(data))

#define PES_HAS_PTS(data)     ((data)[7] & 0x80)
#define PES_HAS_DTS(data)     ((data)[7] & 0x40)

#define DATA_IS_PES(data) (!(data)[0] && !(data)[1] && (data)[2] == 1)

#define PES_HEADER_LEN(data) (8 + (data)[8] + 1)


/*
 * timestamps
 */

static inline int     pts_to_ms(int64_t pts) { return (int)(pts/INT64_C(90)); }
static inline int64_t ms_to_pts(int ms)      { return ((int64_t)(ms)) * INT64_C(90); }

int64_t pes_get_pts(const uint8_t *buf, int len);
int64_t pes_get_dts(const uint8_t *buf, int len);
void    pes_change_pts(uint8_t *buf, int len, int64_t new_pts);
int     pes_strip_pts_dts(uint8_t *buf, int len);

/*
 * payload
 */

struct video_size_s;

int     pes_is_frame_h264(const uint8_t *buf, int len);
uint8_t pes_get_picture_type(const uint8_t *buf, int len);
int     pes_get_video_size(const uint8_t *buf, int len, struct video_size_s *size, int h264);

static inline int pes_is_mpeg1(const uint8_t *header)
{
  if (IS_VIDEO_PACKET(header) || IS_AUDIO_PACKET(header))
    return ((header[6] & 0xC0) != 0x80);
  if (header[3] == 0xBA) 
    return ((header[4] & 0x40) == 0); /* mpeg1 */
  return 0;
}

/*
 * Extract PES packet length
 */

static inline int pes_packet_len(const uint8_t *data, const int len)
{
  if (IS_VIDEO_PACKET(data) || IS_AUDIO_PACKET(data)) {
    return 6 + (data[4] << 8 | data[5]);
  } else if (data[3] == PADDING_STREAM) {
    return 6 + (data[4] << 8 | data[5]);
  } else if (data[3] == 0xBA) {
    if ((data[4] & 0x40) == 0)  /* mpeg1 */
      return 12;
    else  /* mpeg 2 */
      return 14 + (data[0xD] & 0x07);
  } else if (data[3] <= 0xB9) {
    return -3;
  }
  return -(6 + (data[4] << 8 | data[5]));
}


#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* _XINELIBOUTPUT_PES_H_ */
