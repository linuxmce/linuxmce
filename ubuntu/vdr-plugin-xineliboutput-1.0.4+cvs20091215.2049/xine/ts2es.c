/*
 * ts2es.c: demux MPEG-TS -> ES
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: ts2es.c,v 1.5 2009/08/25 10:09:20 phintuka Exp $
 *
 */

#include <stdlib.h>

#include <xine/buffer.h>

#include "../tools/ts.h"
#include "../tools/pes.h"

#include "ts2es.h"

#define LOG_MODULENAME "[demux_vdr] "
#define SysLogLevel    iSysLogLevel
#include "../logdefs.h"


struct ts2es_s {
  fifo_buffer_t *fifo;
  uint32_t       stream_type;
  uint32_t       xine_buf_type;

  buf_element_t *buf;
  int            pes_start;
  int            first_pusi_seen;
  int            video;
  int64_t        pts;
};


static void ts2es_parse_pes(ts2es_t *this)
{
  if (!DATA_IS_PES(this->buf->content)) {
    LOGMSG("ts2es: payload not PES ?");
    return;
  }

  /* parse PES header */
  uint    hdr_len = PES_HEADER_LEN(this->buf->content);
  uint8_t pes_pid = this->buf->content[3];
  uint    pes_len = (this->buf->content[4] << 8) | this->buf->content[5];

  /* parse PTS */
  this->buf->pts = pes_get_pts(this->buf->content, this->buf->size);
  if(this->buf->pts >= 0)
    this->pts = this->buf->pts;
  else
    this->pts = 0;

  /* strip PES header */
  this->buf->content += hdr_len;
  this->buf->size    -= hdr_len;

  /* parse substream header */

  if (pes_pid != PRIVATE_STREAM1)
    return;

  /* RAW AC3 audio ? -> do nothing */
  if (this->stream_type == STREAM_AUDIO_AC3) {
    this->xine_buf_type |= BUF_AUDIO_A52;
    this->buf->type = this->xine_buf_type;
    return;
  }

  /* AC3 syncword in beginning of PS1 payload ? */
  if (this->buf->content[0] == 0x0B &&
      this->buf->content[1] == 0x77) {
    /* --> RAW AC3 audio - do nothing */
    this->xine_buf_type |= BUF_AUDIO_A52;
    this->buf->type = this->xine_buf_type;
    return;
  }

  /* audio in PS1 */
  if (this->stream_type == ISO_13818_PES_PRIVATE) {

    if ((this->buf->content[0] & 0xf0) == 0x80) {
      /* AC3, strip substream header */
      this->buf->content += 4;
      this->buf->size    -= 4;
      this->xine_buf_type |= BUF_AUDIO_A52;
      this->buf->type = this->xine_buf_type;
      return;
    }

    if ((this->buf->content[0] & 0xf0) == 0xa0) {
      /* PCM, strip substream header */
      int pcm_offset;
      for (pcm_offset=0; ++pcm_offset < this->buf->size-1 ; ) {
        if (this->buf->content[pcm_offset] == 0x01 && 
            this->buf->content[pcm_offset+1] == 0x80 ) { /* START */
          pcm_offset += 2;
          break;
        }
      }
      this->buf->content += pcm_offset;
      this->buf->size    -= pcm_offset;

      this->xine_buf_type |= BUF_AUDIO_LPCM_BE;
      this->buf->type = this->xine_buf_type;
      return;
    }

    LOGMSG("ts2es: unhandled PS1 substream 0x%x", this->buf->content[0]);
    return;
  }

  /* DVB SPU */
  if (this->stream_type == STREAM_DVBSUB) {
    if (this->buf->content[0] != 0x20 ||
        this->buf->content[1] != 0x00)
      LOGMSG("ts2es: DVB SPU, invalid PES substream header");
    this->buf->decoder_info[2] = pes_len - hdr_len - 3 + 9;
    return;
  }
}

buf_element_t *ts2es_put(ts2es_t *this, uint8_t *data, fifo_buffer_t *src_fifo)
{
  buf_element_t *result = NULL;
  int            bytes  = ts_PAYLOAD_SIZE(data);
  int            pusi   = ts_PAYLOAD_START(data);

  if (ts_HAS_ERROR(data)) {
    LOGDBG("ts2es: transport error");
    return NULL;
  }
  if (!ts_HAS_PAYLOAD(data)) {
    LOGDBG("ts2es: no payload, size %d", bytes);
    return NULL;
  }

  /* handle new payload unit */
  if (pusi) {
    this->first_pusi_seen = 1;
    this->pes_start = 1;
    if (this->buf) {

      this->buf->decoder_flags |= BUF_FLAG_FRAME_END;
      this->buf->pts = this->pts;

      result = this->buf;
      this->buf = NULL;
    }
  }

  /* need new buffer ? */
  if (!this->buf) {
    /* discard data until first payload start indicator */
    if (!this->first_pusi_seen)
      return NULL;

    if (src_fifo && src_fifo != this->fifo) {
      if (!this->video)
        this->buf = this->fifo->buffer_pool_try_alloc(this->fifo);
      if (!this->buf)
        this->buf = src_fifo->buffer_pool_try_alloc(src_fifo);
      if (!this->buf)
        this->buf = this->fifo->buffer_pool_alloc(this->fifo);
    } else {
      this->buf = this->fifo->buffer_pool_alloc(this->fifo);
    }

    this->buf->type = this->xine_buf_type;
    this->buf->decoder_info[0] = 1;
  }

  /* strip ts header */
  data += TS_SIZE - bytes;

  /* copy payload */
  memcpy(this->buf->content + this->buf->size, data, bytes);
  this->buf->size += bytes;

  /* parse PES header */
  if (this->pes_start) {
    this->pes_start = 0;

    ts2es_parse_pes(this);
  }

  /* split large packets */
  if ((this->video && this->buf->size >= 2048+64-TS_SIZE) ||
      this->buf->size >= this->buf->max_size-TS_SIZE) {

    this->buf->pts = this->pts;

    result = this->buf;
    this->buf = NULL;
  }

  return result;
}

void ts2es_flush(ts2es_t *this)
{
  if (this->buf) {

    this->buf->decoder_flags |= BUF_FLAG_FRAME_END;
    this->buf->pts = this->pts;

    this->fifo->put (this->fifo, this->buf);
    this->buf = NULL;
  }
}

void ts2es_dispose(ts2es_t *data)
{
  if (data) {
    if (data->buf)
      data->buf->free_buffer(data->buf);
    free(data);
  }
}

ts2es_t *ts2es_init(fifo_buffer_t *dst_fifo, ts_stream_type stream_type, uint stream_index)
{
  ts2es_t *data = calloc(1, sizeof(ts2es_t));
  data->fifo = dst_fifo;

  data->stream_type = stream_type;

  switch(stream_type) {
    /* VIDEO (PES streams 0xe0...0xef) */
    case ISO_11172_VIDEO:
    case ISO_13818_VIDEO:
    case STREAM_VIDEO_MPEG:
      data->xine_buf_type = BUF_VIDEO_MPEG;
      break;
    case ISO_14496_PART2_VIDEO:
      data->xine_buf_type = BUF_VIDEO_MPEG4;
      break;
    case ISO_14496_PART10_VIDEO:
      data->xine_buf_type = BUF_VIDEO_H264;
      break;

    /* AUDIO (PES streams 0xc0...0xdf) */
    case  ISO_11172_AUDIO:
    case  ISO_13818_AUDIO:
      data->xine_buf_type = BUF_AUDIO_MPEG;
      break;
    case  ISO_13818_PART7_AUDIO:
    case  ISO_14496_PART3_AUDIO:
      data->xine_buf_type = BUF_AUDIO_AAC;
      break;

    /* AUDIO (PES stream 0xbd) */
    case ISO_13818_PES_PRIVATE:
      data->xine_buf_type = 0;
      /* detect from PES substream header */
      break;

    /* DVB SPU (PES stream 0xbd) */
    case STREAM_DVBSUB:
      data->xine_buf_type = BUF_SPU_DVB;
      break;

    /* RAW AC3 */
    case STREAM_AUDIO_AC3:
      data->xine_buf_type = BUF_AUDIO_A52;
      break;

    default:
      LOGMSG("ts2es: unknown stream type 0x%x", stream_type);
      break;
  }

  /* substream ID (audio/SPU) */
  data->xine_buf_type |= stream_index;

  if ((data->xine_buf_type & BUF_MAJOR_MASK) == BUF_VIDEO_BASE)
    data->video = 1;

  return data;
}

