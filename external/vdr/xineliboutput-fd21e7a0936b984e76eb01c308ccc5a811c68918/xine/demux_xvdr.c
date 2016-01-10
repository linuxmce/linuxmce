/*
 * Copyright (C) 2000-2006 the xine project
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
 *
 * demultiplexer for xineliboutput (xvdr)
 */

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include <xine/xine_internal.h>
#include <xine/xineutils.h>
#include <xine/demux.h>

#include "../xine_input_vdr_mrl.h"
#include "../tools/mpeg.h"
#include "../tools/h264.h"
#include "../tools/pes.h"
#include "../tools/ts.h"

#include "ts2es.h"
#include "demux_xvdr_tsdata.h"
#include "xvdr_metronom.h"

/*
 * features
 */

#define VDR_SUBTITLES
#define TEST_DVB_SPU

/*
 * logging
 */

static const char log_module_demux_xvdr[] = "[demux_vdr] ";
#define LOG_MODULENAME log_module_demux_xvdr
#define SysLogLevel    iSysLogLevel

#include "../logdefs.h"

#define LOGSPU(x...)

/*
 * constants
 */

#define WRAP_THRESHOLD     360000  /* 4 sec */
#define PTS_AUDIO 0
#define PTS_VIDEO 1


/* redefine abs as macro to handle 64-bit diffs.
   i guess llabs may not be available everywhere */
#define abs(x) ( ((x)<0) ? -(x) : (x) )

typedef struct demux_xvdr_s {
  demux_plugin_t        demux_plugin;

  xine_stream_t        *stream;
  fifo_buffer_t        *audio_fifo;
  fifo_buffer_t        *video_fifo;

  input_plugin_t       *input;

  ts_data_t            *ts_data;        /* MPEG-TS stuff */

  int64_t               last_pts[2];
  int64_t               last_vpts;
  int                   status;
  uint32_t              video_type;
  uint32_t              audio_type;
  uint32_t              subtitle_type;

  /* current buf_element */
  int64_t               pts;
  int64_t               dts;
  uint32_t              packet_len;
  uint8_t               stream_id;

  uint8_t               send_newpts          : 1;
  uint8_t               buf_flag_seek        : 1;
  uint8_t               ffmpeg_mpeg2_decoder : 1;
  uint8_t               coreavc_h264_decoder : 1;
  uint8_t               bih_posted           : 1;
} demux_xvdr_t ;

typedef struct {

  demux_class_t     demux_class;

  /* class-wide, global variables here */

  xine_t           *xine;
  config_values_t  *config;
} demux_xvdr_class_t;


static const char * get_decoder_name(xine_t *xine, int buf_type)
{
  uint32_t base = buf_type & 0xFF000000;
  uint32_t type = (buf_type >> 16) & 0xFF;
  plugin_node_t *node = NULL;

  if (base == BUF_VIDEO_BASE)
    node = xine->plugin_catalog->video_decoder_map[type][0];
  else if (base == BUF_AUDIO_BASE)
    node = xine->plugin_catalog->audio_decoder_map[type][0];

  if (node) {
    plugin_info_t *info = node->info;
    if (info)
      return info->id;
  }

  return "<none>";
}

#define LOG_DECODER(buf_type, name)                              \
  LOGDBG("Using %-10s decoder \"%s\"",                           \
         name, get_decoder_name(this->stream->xine, buf_type));

static void detect_video_decoders(demux_xvdr_t *this)
{
  const char *name;

  name = get_decoder_name(this->stream->xine, BUF_VIDEO_MPEG);
  if (!strcmp(name, "ffmpegvideo"))
    this->ffmpeg_mpeg2_decoder = 1;
  LOGDBG("Using MPEG video decoder \"%s\"%s",
         name, this->ffmpeg_mpeg2_decoder ? " (FFmpeg)" : "");

  name = get_decoder_name(this->stream->xine, BUF_VIDEO_H264);
  if (!strcmp(name, "dshowserver"))
    this->coreavc_h264_decoder = 1;
  LOGDBG("Using H.264      decoder \"%s\"%s",
         name, this->coreavc_h264_decoder ? " (dshowserver (CoreAVC))" : "");

  LOG_DECODER(BUF_VIDEO_VC1,     "VC-1");
}

static void detect_audio_decoders(demux_xvdr_t *this)
{
  LOG_DECODER(BUF_AUDIO_MPEG,    "MPEG audio");
  LOG_DECODER(BUF_AUDIO_A52,     "AC-3");
  LOG_DECODER(BUF_AUDIO_AAC,     "AAC");
  LOG_DECODER(BUF_AUDIO_DTS,     "DTS");
  LOG_DECODER(BUF_AUDIO_LPCM_BE, "LPCM");
#ifdef BUF_AUDIO_AAC_EAC3
  LOG_DECODER(BUF_AUDIO_AAC_EAC3, "E-AC-3");
#endif
#ifdef BUF_AUDIO_AAC_LATM
  LOG_DECODER(BUF_AUDIO_AAC_LATM, "AAC LATM");
#endif
}

static void demux_xvdr_parse_ts(demux_xvdr_t *this, buf_element_t *buf);
static void demux_xvdr_parse_pes(demux_xvdr_t *this, buf_element_t *buf);

static int32_t parse_video_stream(demux_xvdr_t *this, uint8_t *p, buf_element_t *buf);
static int32_t parse_audio_stream(demux_xvdr_t *this, uint8_t *p, buf_element_t *buf);
static int32_t parse_private_stream_1(demux_xvdr_t *this, uint8_t *p, buf_element_t *buf);

static void pts_wrap_workaround(demux_xvdr_t *this, buf_element_t *buf, int video)
{
  /* PTS wrap workaround */
  if (buf->pts > 0) {

#ifdef LOG_PES_AV_DIFF
    static int64_t vpts = 0, apts = 0;
    if (video) vpts = buf->pts;
    else apts = buf->pts;
    if (vpts > 0 && apts > 0)
      LOGMSG("pts diff [%d] %d", video, (int)(vpts - apts));
#endif

    if (video) {

      /* VIDEO wrap in middle of GOP */
      if (this->last_vpts < 14400 &&
          this->last_vpts > 0     &&
          buf->pts > (INT64_C(0x1ffffffff) - 14400) &&
          !this->send_newpts) {
        LOGMSG("VIDEO pts wrap in middle of GOP, ignoring video pts %" PRId64, buf->pts);
        buf->pts = INT64_C(0);
        return;
      }

      this->last_vpts = buf->pts;
      return;
    }

    /* VIDEO wrap before AUDIO wrap */
    else if (buf->pts        > INT64_C( 0x40400000 ) &&
             this->last_vpts < INT64_C( 0x40000000 ) &&
             this->last_vpts > INT64_C( 0 )) {
      LOGMSG("VIDEO pts wrap before AUDIO, ignoring audio pts %" PRId64, buf->pts);
      buf->pts = INT64_C(0);
    }
  }
}

static void check_newpts(demux_xvdr_t *this, buf_element_t *buf, int video )
{
  if (buf->pts <= 0)
    return;

  if (video) {
    int still_mode  = (int)this->stream->metronom->get_option(this->stream->metronom, XVDR_METRONOM_STILL_MODE);
    int trick_speed = (int)this->stream->metronom->get_option(this->stream->metronom, XVDR_METRONOM_TRICK_SPEED);
    if (still_mode > 0 || trick_speed > 0) {
      LOGMSG("Skipping new pts %"PRId64" (still=%d trickspeed=%d)", buf->pts, still_mode, trick_speed);
      return;
    }
  }

  pts_wrap_workaround(this, buf, video);

  if (buf->pts) {
    int64_t diff = buf->pts - this->last_pts[video];

    if (this->send_newpts || (this->last_pts[video] && abs(diff)>WRAP_THRESHOLD)) {

      LOGVERBOSE("New PTS: %"PRId64" (%s)", buf->pts, video ? "VIDEO" : "AUDIO");

      if (this->buf_flag_seek) {
        _x_demux_control_newpts(this->stream, buf->pts, BUF_FLAG_SEEK);
        this->buf_flag_seek = 0;
      } else {
        _x_demux_control_newpts(this->stream, buf->pts, 0);
      }
      this->send_newpts = 0;
    }

    this->last_pts[video] = buf->pts;
    this->last_pts[1-video] = buf->pts;
  }
}

static void put_control_buf(fifo_buffer_t *buffer, fifo_buffer_t *pool, int cmd)
{
  buf_element_t *buf = pool->buffer_pool_try_alloc(pool);
  if(buf) {
    buf->type = cmd;
    buffer->put(buffer, buf);
  }
}

/*
 * post_sequence_end()
 *
 * Add MPEG2 or H.264 sequence end code to fifo buffer
 */
static void post_sequence_end(fifo_buffer_t *fifo, uint32_t video_type)
{
  buf_element_t *buf = fifo->buffer_pool_try_alloc(fifo);
  if (buf) {
    buf->type = video_type;
    buf->size = 4;
    buf->decoder_flags = BUF_FLAG_FRAME_END;
    buf->content[0] = 0x00;
    buf->content[1] = 0x00;
    buf->content[2] = 0x01;
    buf->content[3] = (video_type == BUF_VIDEO_H264) ? NAL_END_SEQ : SC_SEQUENCE_END;
    fifo->put(fifo, buf);
  }
}

/*
 * post_frame_end()
 *
 * Signal end of video frame to decoder.
 *
 * This function is used with:
 *  - FFmpeg mpeg2 decoder
 *  - FFmpeg and CoreAVC H.264 decoders
 *  - NOT with libmpeg2 mpeg decoder
 */
static void post_frame_end(demux_xvdr_t *this, buf_element_t *vid_buf)
{
  buf_element_t *cbuf = this->video_fifo->buffer_pool_try_alloc (this->video_fifo) ?:
                        this->audio_fifo->buffer_pool_try_alloc (this->audio_fifo);

  if (!cbuf) {
    LOGMSG("post_frame_end(): buffer_pool_try_alloc() failed, retrying");
    xine_usec_sleep (10*1000);
    cbuf = this->video_fifo->buffer_pool_try_alloc (this->video_fifo);
    if (!cbuf) {
      LOGERR("post_frame_end(): get_buf_element() failed !");
      return;
    }
  }

  cbuf->type          = this->video_type;
  cbuf->decoder_flags = BUF_FLAG_FRAME_END;

  if (!this->bih_posted) {
    video_size_t size = {0};
    if (pes_get_video_size(vid_buf->content, vid_buf->size, &size, this->video_type == BUF_VIDEO_H264)) {

      /* reset decoder buffer */
      cbuf->decoder_flags |= BUF_FLAG_FRAME_START;

      /* Fill xine_bmiheader for CoreAVC / H.264 */

      if (this->video_type == BUF_VIDEO_H264 && this->coreavc_h264_decoder) {
        xine_bmiheader *bmi = (xine_bmiheader*) cbuf->content;

        cbuf->decoder_flags |= BUF_FLAG_HEADER;
        cbuf->decoder_flags |= BUF_FLAG_STDHEADER;   /* CoreAVC: buffer contains bmiheader */
        cbuf->size           = sizeof(xine_bmiheader);

        memset (bmi, 0, sizeof(xine_bmiheader));

        bmi->biSize   = sizeof(xine_bmiheader);
        bmi->biWidth  = size.width;
        bmi->biHeight = size.height;

        bmi->biPlanes        = 1;
        bmi->biBitCount      = 24;
        bmi->biCompression   = 0x34363248;
        bmi->biSizeImage     = 0;
        bmi->biXPelsPerMeter = size.pixel_aspect.num;
        bmi->biYPelsPerMeter = size.pixel_aspect.den;
        bmi->biClrUsed       = 0;
        bmi->biClrImportant  = 0;
      }

      /* Set aspect ratio for ffmpeg mpeg2 / CoreAVC H.264 decoder
       * (not for FFmpeg H.264 or libmpeg2 mpeg2 decoders)
       */

      if (size.pixel_aspect.num &&
          (this->video_type != BUF_VIDEO_H264 || this->coreavc_h264_decoder)) {
        cbuf->decoder_flags |= BUF_FLAG_HEADER;
        cbuf->decoder_flags |= BUF_FLAG_ASPECT;
        /* pixel ratio -> frame ratio */
        if (size.pixel_aspect.num > size.height) {
          cbuf->decoder_info[1] = size.pixel_aspect.num / size.height;
          cbuf->decoder_info[2] = size.pixel_aspect.den / size.width;
        } else {
          cbuf->decoder_info[1] = size.pixel_aspect.num * size.width;
          cbuf->decoder_info[2] = size.pixel_aspect.den * size.height;
        }
      }

      LOGDBG("post_frame_end: video width %d, height %d, pixel aspect %d:%d",
             size.width, size.height, size.pixel_aspect.num, size.pixel_aspect.den);

      this->bih_posted = 1;
    }
  }

  this->video_fifo->put (this->video_fifo, cbuf);
}

static void track_audio_stream_change(demux_xvdr_t *this, buf_element_t *buf)
{
#if !defined(BUF_CONTROL_RESET_TRACK_MAP)
#  warning xine-lib is older than 1.1.2. Multiple audio streams are not supported.
#else
  if (this->audio_type != buf->type) {
    LOGDBG("audio stream changed: %08x -> %08x", this->audio_type, buf->type);

    ts_data_reset_audio(this->ts_data, this->audio_fifo, buf->type & 0xff);

    put_control_buf(this->audio_fifo,
                    this->audio_fifo,
                    BUF_CONTROL_RESET_TRACK_MAP);

    if (this->audio_type) {
      buf_element_t *b = this->audio_fifo->buffer_pool_try_alloc(this->audio_fifo);
      if (b) {
        b->type = BUF_CONTROL_START;
        b->decoder_flags = BUF_FLAG_GAPLESS_SW;
        this->audio_fifo->put(this->audio_fifo, b);
      }
    }

    this->audio_type = buf->type;
  }
#endif
}

static void demux_xvdr_fwd_buf(demux_xvdr_t *this, buf_element_t *buf)
{
  /* demuxed video --> video_fifo */
  if ((buf->type & BUF_MAJOR_MASK) == BUF_VIDEO_BASE) {
    this->video_type = buf->type;
    check_newpts (this, buf, PTS_VIDEO);
    this->video_fifo->put (this->video_fifo, buf);
    return;
  }

  /* demuxed audio --> audio_fifo */
  if ((buf->type & BUF_MAJOR_MASK) == BUF_AUDIO_BASE) {
    if (this->audio_fifo) {
      check_newpts (this, buf, PTS_AUDIO);
      track_audio_stream_change (this, buf);
      this->audio_fifo->put (this->audio_fifo, buf);
    } else {
      buf->free_buffer (buf);
    }
    return;
  }

  /* decoder flush --> video_fifo */
  if (buf->type == BUF_CONTROL_FLUSH_DECODER) {
    /* append sequence end code */
    post_sequence_end (this->video_fifo, this->video_type);
    this->video_fifo->put (this->video_fifo, buf);
    return;
  }

  /* control buffer --> both fifos */
  if ((buf->type & BUF_MAJOR_MASK) == BUF_CONTROL_BASE) {
    if (this->audio_fifo) {
      /* duplicate goes to audio fifo */
      buf_element_t *cbuf = this->audio_fifo->buffer_pool_alloc (this->audio_fifo);
      cbuf->type = buf->type;
      cbuf->decoder_flags = buf->decoder_flags;
      memcpy (cbuf->decoder_info,     buf->decoder_info,     sizeof(cbuf->decoder_info));
      memcpy (cbuf->decoder_info_ptr, buf->decoder_info_ptr, sizeof(cbuf->decoder_info_ptr));

      this->audio_fifo->put (this->audio_fifo, cbuf);
    }
    this->video_fifo->put (this->video_fifo, buf);
    return;
  }

  LOGMSG("Unhandled buffer type %08x", buf->type);
  buf->free_buffer (buf);
}

static void demux_xvdr_parse_pack (demux_xvdr_t *this)
{
  buf_element_t *buf = NULL;
  uint8_t       *p;

  buf = this->input->read_block (this->input, this->video_fifo, 8128);

  if (!buf) {
    if (errno == EINTR) {
      LOGVERBOSE("input->read_block() was interrupted");
      ts_data_flush(this->ts_data);
    } else if (errno != EAGAIN) {
      LOGDBG("DEMUX_FINISHED (input returns NULL with error)");
      this->status = DEMUX_FINISHED;
      ts_data_dispose(&this->ts_data);
    }
    return;
  }

  /* If this is not a block for the demuxer, pass it
   * straight through. */
  if (buf->type != BUF_DEMUX_BLOCK) {
    ts_data_flush (this->ts_data);
    demux_xvdr_fwd_buf (this, buf);
    return;
  }

  p = buf->content; /* len = this->blocksize; */
  buf->decoder_flags = 0;

  if (DATA_IS_TS(p)) {
    demux_xvdr_parse_ts (this, buf);
    return;
  }
  if (DATA_IS_PES(p)) {
    demux_xvdr_parse_pes (this, buf);
    return;
  }

  LOGMSG("Header %02x %02x %02x (should be 0x000001 or 0x47)", p[0], p[1], p[2]);
  buf->free_buffer (buf);
  return;
}

static void demux_xvdr_parse_pes (demux_xvdr_t *this, buf_element_t *buf)
{
  uint8_t       *p = buf->content;
  int32_t        result;

  if (IS_PADDING_PACKET(p)) {
    buf->free_buffer (buf);
    return;
  }

  this->stream_id  = p[3];

  if (this->ts_data) {
    ts_data_flush(this->ts_data);
    ts_data_dispose(&this->ts_data);
  }

  if (IS_VIDEO_PACKET(p)) {
    result = parse_video_stream(this, p, buf);
  } else if (IS_MPEG_AUDIO_PACKET(p)) {
    result = parse_audio_stream(this, p, buf);
  } else if (IS_PS1_PACKET(p)) {
    result = parse_private_stream_1(this, p, buf);
  } else {
    LOGMSG("Unrecognised PES stream 0x%02x", this->stream_id);
    buf->free_buffer (buf);
    return;
  }

  if (result < 0) {
    return;
  }

  LOGMSG("error! freeing buffer.");
  buf->free_buffer (buf);
}

/*
 * demux_xvdr_parse_ts()
 *
 * MPEG-TS demuxing
 */
static void demux_xvdr_parse_ts (demux_xvdr_t *this, buf_element_t *buf)
{
  if (!this->ts_data)
    this->ts_data = calloc(1, sizeof(ts_data_t));

  ts_data_t     *ts_data  = this->ts_data;
  fifo_buffer_t *src_fifo = buf->source;

  while (buf->size >= TS_SIZE) {

    unsigned int ts_pid = ts_PID(buf->content);

    /* parse PAT */
    if (ts_pid == 0) {
      pat_data_t pat;

      if (ts_parse_pat(&pat, buf->content)) {
        if (ts_data->pmt_pid        != pat.pmt_pid[0] ||
            ts_data->program_number != pat.program_number[0]) {

          LOGVERBOSE("PAT: program changed, flushing demuxer");

          ts_data_flush(ts_data);

          ts_data->pmt_pid        = pat.pmt_pid[0];
          ts_data->program_number = pat.program_number[0];
        } else {
          LOGVERBOSE("Got PAT, PMT pid = %d, program = %d", ts_data->pmt_pid, ts_data->program_number);
        }
      }
    }

    /* parse PMT */
    else if (ts_pid == ts_data->pmt_pid) {

      if (ts_parse_pmt(&ts_data->pmt, ts_data->program_number, buf->content)) {

        /* PMT changed, reset ts->es converters */
        LOGDBG("PMT changed, resetting demuxer");
        ts_data_ts2es_init(&ts_data, this->stream->video_fifo, this->stream->audio_fifo);

        this->video_type = (ts_data->pmt.video_type == ISO_14496_PART10_VIDEO) ?
                           BUF_VIDEO_H264 : BUF_VIDEO_MPEG;

        /* Inform UI of channels changes */
        xine_event_t event;
        event.type = XINE_EVENT_UI_CHANNELS_CHANGED;
        event.data_length = 0;
        xine_event_send(this->stream, &event);
      }
    }

    /* demux video */
    else if (ts_pid == ts_data->pmt.video_pid) {
      if (ts_data->video) {
        buf_element_t *vbuf     = ts2es_put(ts_data->video, buf->content, src_fifo);
        if (vbuf) {
          check_newpts( this, vbuf, PTS_VIDEO );

          this->stream->video_fifo->put(this->stream->video_fifo, vbuf);
        }
      }
    }

    /* demux audio */
    else {
      int i/*, done = 0*/;
      for (i=0; i < ts_data->pmt.audio_tracks_count; i++)
        if (ts_pid == ts_data->pmt.audio_tracks[i].pid) {
          if (ts_data->audio[i]) {
            buf_element_t *abuf = ts2es_put(ts_data->audio[i], buf->content, src_fifo);
            if (abuf) {
              check_newpts( this, abuf, PTS_AUDIO );
              track_audio_stream_change (this, abuf);

              this->stream->audio_fifo->put(this->stream->audio_fifo, abuf);
            }
          }
          /*done = 1;*/
          break;
        }
#if 0
      /* demux subtitles */
      if (!done)
      for (i=0; i < ts_data->pmt.spu_tracks_count; i++)
        if (ts_pid == ts_data->pmt.spu_tracks[i].pid) {
          if (ts_data->spu[i]) {
            buf_element_t *sbuf = ts2es_put(ts_data->spu[i], buf->content, NULL);
            if (sbuf)
              this->stream->video_fifo->put(this->stream->video_fifo, sbuf);
          }
          done = 1;
          break;
        }

      if (!done)
        LOGMSG("Got unknown TS packet, pid = %d", ts_pid);
#endif
    }

    buf->content += TS_SIZE;
    buf->size    -= TS_SIZE;
  }

  buf->free_buffer(buf);
}

/* FIXME: Extension data is not parsed, and is also not skipped. */

static int32_t parse_pes_for_pts(demux_xvdr_t *this, uint8_t *p, buf_element_t *buf)
{
  int32_t header_len;

  this->packet_len = p[4] << 8 | p[5];

  if ((p[6] & 0xC0) != 0x80 /* mpeg1 */) {
    header_len = 6;
    p   += 6; /* packet_len -= 6; */

    while ((p[0] & 0x80) == 0x80) {
      p++;
      header_len++;
      this->packet_len--;
    }

    if ((p[0] & 0xc0) == 0x40) {
      /* STD_buffer_scale, STD_buffer_size */
      p += 2;
      header_len += 2;
      this->packet_len -= 2;
    }

    this->pts = 0;
    this->dts = 0;

    if ((p[0] & 0xf0) == 0x20) {
      this->pts  = (int64_t)(p[ 0] & 0x0E) << 29 ;
      this->pts |=  p[ 1]         << 22 ;
      this->pts |= (p[ 2] & 0xFE) << 14 ;
      this->pts |=  p[ 3]         <<  7 ;
      this->pts |= (p[ 4] & 0xFE) >>  1 ;
      p   += 5;
      header_len+= 5;
      this->packet_len -=5;
      return header_len;
    } else if ((p[0] & 0xf0) == 0x30) {
      this->pts  = (int64_t)(p[ 0] & 0x0E) << 29 ;
      this->pts |=  p[ 1]         << 22 ;
      this->pts |= (p[ 2] & 0xFE) << 14 ;
      this->pts |=  p[ 3]         <<  7 ;
      this->pts |= (p[ 4] & 0xFE) >>  1 ;

      this->dts  = (int64_t)(p[ 5] & 0x0E) << 29 ;
      this->dts |=  p[ 6]         << 22 ;
      this->dts |= (p[ 7] & 0xFE) << 14 ;
      this->dts |=  p[ 8]         <<  7 ;
      this->dts |= (p[ 9] & 0xFE) >>  1 ;

      p   += 10;
      header_len += 10;
      this->packet_len -= 10;
      return header_len;
    } else {
      p++;
      header_len++;
      this->packet_len--;
      return header_len;
    }

  } else { /* mpeg 2 */


    if ((p[6] & 0xC0) != 0x80) {
      LOGMSG("warning: PES header reserved 10 bits not found");
      buf->free_buffer(buf);
      return -1;
    }


    /* check PES scrambling_control */
    if ((p[6] & 0x30) != 0) {
      LOGMSG("encrypted PES ?");
      buf->free_buffer(buf);
      return -1;
    }

    if (p[7] & 0x80) { /* pts avail */

      this->pts  = (int64_t)(p[ 9] & 0x0E) << 29 ;
      this->pts |=  p[10]         << 22 ;
      this->pts |= (p[11] & 0xFE) << 14 ;
      this->pts |=  p[12]         <<  7 ;
      this->pts |= (p[13] & 0xFE) >>  1 ;

    } else
      this->pts = 0;

    if (p[7] & 0x40) { /* dts avail */

      this->dts  = (int64_t)(p[14] & 0x0E) << 29 ;
      this->dts |=  p[15]         << 22 ;
      this->dts |= (p[16] & 0xFE) << 14 ;
      this->dts |=  p[17]         <<  7 ;
      this->dts |= (p[18] & 0xFE) >>  1 ;

    } else
      this->dts = 0;


    header_len = p[8];

    this->packet_len -= header_len + 3;
    return header_len + 9;
  }
  return 0;
}

#if defined(TEST_DVB_SPU)
/*
 * parse_dvb_spu()
 *
 * DVB subtitle stream demuxing
 */
static int32_t parse_dvb_spu(demux_xvdr_t *this, uint8_t *p, buf_element_t *buf, int substream_header_len)
{
  uint spu_id = p[0] & 0x1f;
  _x_select_spu_channel(this->stream, spu_id);

# ifdef VDR_SUBTITLES
  if (substream_header_len == 1) {
    p--;
    this->packet_len++;
  }
# endif /* VDR_SUBTITLES */

  /* Skip substream header */
  p += substream_header_len;
  buf->content = p;
  buf->size    = this->packet_len - substream_header_len;

  /* Special buffer when payload packet changes */
  if (this->pts > 0) {
    buf_element_t *cbuf = this->video_fifo->buffer_pool_alloc(this->video_fifo);
    int page_id         = (*(p+4) << 8) | *(p+5);

    spu_dvb_descriptor_t *spu_descriptor = (spu_dvb_descriptor_t *) cbuf->content;
    memset(spu_descriptor, 0, sizeof(spu_dvb_descriptor_t));
    spu_descriptor->comp_page_id = page_id;

    cbuf->type = BUF_SPU_DVB + spu_id;
    cbuf->size = 0;
    cbuf->decoder_flags   = BUF_FLAG_SPECIAL;
    cbuf->decoder_info[1] = BUF_SPECIAL_SPU_DVB_DESCRIPTOR;
    cbuf->decoder_info[2] = sizeof(spu_dvb_descriptor_t);
    cbuf->decoder_info_ptr[2] = spu_descriptor;

    this->video_fifo->put (this->video_fifo, cbuf);
  }

  buf->type      = BUF_SPU_DVB + spu_id;
  buf->pts       = this->pts;
  buf->decoder_info[2] = this->pts > 0 ? 0xffff : 0; /* hack - size unknown here (?) */

  this->video_fifo->put (this->video_fifo, buf);

  return -1;
}

int detect_dvb_spu(demux_xvdr_t *this, uint8_t *p, buf_element_t *buf)
{
  LOGSPU("%s%02x %02x %02x %02x   %02x %02x %02x %02x",
         this->pts>0?"* ":"  ",p[0], p[1], p[2], p[3],
         p[4], p[5], p[6], p[7]);

  /* If PES packet has PTS, it starts new subtitle (ES) packet. */
  if (this->pts > 0) {
    /* Reset SPU type */
    this->subtitle_type = 0;
  }

# ifdef VDR_SUBTITLES
  /* Compatibility mode for old subtitles plugin */
  if (this->subtitle_type != BUF_SPU_DVD) {
    if ((buf->content[7] & 0x01) && (p[-3] & 0x81) == 0x01 && p[-2] == 0x81) {
      LOGDBG("DVB SPU: Old vdr-subtitles compability mode");
      return parse_dvb_spu(this, p, buf, 1);
    }
  }
# endif /* VDR_SUBTITLES */

  /* Start of subtitle packet. Guess substream type */
  if (this->pts > 0) {
    if (p[4] == 0x20 && p[5] == 0x00 && (p[6] == 0x0f || p[4] == 0x0f)) {
      this->subtitle_type = BUF_SPU_DVB;
      LOGSPU(" -> DVB SPU");
    } else if (p[2] || (p[3] & 0xfe)) {
      this->subtitle_type = BUF_SPU_DVD;
      LOGSPU(" -> DVD SPU");
    } else {
      this->subtitle_type = BUF_SPU_DVD;
      LOGMSG(" -> DV? SPU -> DVD");
    }
  }

  /* DVD SPU ? */
  if (this->subtitle_type == BUF_SPU_DVD)
    return this->packet_len;

  /* DVB SPU */
  return parse_dvb_spu(this, p, buf, 4);
}

#endif /* TEST_DVB_SPU */

static int32_t parse_private_stream_1(demux_xvdr_t *this, uint8_t *p, buf_element_t *buf)
{
  int track, spu_id;
  int32_t result;

  result = parse_pes_for_pts(this, p, buf);
  if (result < 0) return -1;

  p += result;

  /* SPU */
  if ((p[0] & 0xE0) == 0x20) {
    spu_id = (p[0] & 0x1f);

    if (this->pts <= 0 && !this->subtitle_type) {
      /* need whole ES packet (after seek etc.) */
      buf->free_buffer(buf);
      return -1;
    }

#ifdef TEST_DVB_SPU
    if (detect_dvb_spu(this, p, buf) < 0)
      return -1;
#endif /* TEST_DVB_SPU */
    this->subtitle_type = BUF_SPU_DVD;

    /* DVD SPU */
    buf->content   = p+1;
    buf->size      = this->packet_len-1;

    buf->type      = BUF_SPU_DVD + spu_id;
    buf->decoder_flags |= BUF_FLAG_SPECIAL;
    buf->decoder_info[1] = BUF_SPECIAL_SPU_DVD_SUBTYPE;
    buf->decoder_info[2] = SPU_DVD_SUBTYPE_PACKAGE;
    buf->pts       = this->pts;

    this->video_fifo->put (this->video_fifo, buf);

    return -1;
  }

  if ((p[0]&0xF0) == 0x80) {

    track = p[0] & 0x0F; /* hack : ac3 track */

    buf->decoder_info[1] = p[1];             /* Number of frame headers */
    buf->decoder_info[2] = p[2] << 8 | p[3]; /* First access unit pointer */

    buf->content   = p+4;
    buf->size      = this->packet_len-4;
    if (track & 0x8) {
      buf->type      = BUF_AUDIO_DTS + (track & 0x07); /* DVDs only have 8 tracks */
    } else {
      buf->type      = BUF_AUDIO_A52 + track;
    }
    buf->pts       = this->pts;

    if (this->audio_fifo) {
      check_newpts( this, buf, PTS_AUDIO );
      track_audio_stream_change (this, buf);
      this->audio_fifo->put (this->audio_fifo, buf);
      return -1;
    } else {
      buf->free_buffer(buf);
      return -1;
    }

  } else if ((p[0]&0xf0) == 0xa0) {

    track = p[0] & 0x0F;

    /* send lpcm config byte */
    buf->decoder_flags |= BUF_FLAG_SPECIAL;
    buf->decoder_info[1] = BUF_SPECIAL_LPCM_CONFIG;
    buf->decoder_info[2] = p[5];

    buf->content   = p + 7;
    buf->size      = this->packet_len - 7;
    buf->type      = BUF_AUDIO_LPCM_BE + track;
    buf->pts       = this->pts;

    if (this->audio_fifo) {
      check_newpts( this, buf, PTS_AUDIO );
      track_audio_stream_change (this, buf);
      this->audio_fifo->put (this->audio_fifo, buf);
      return -1;
    } else {
      buf->free_buffer(buf);
      return -1;
    }
  }

  buf->free_buffer(buf);
  return -1;
}

/*
 * detect_h264()
 *
 * Detect video codec (MPEG2 or H.264)
 */
static int detect_h264(uint8_t *data)
{
  /* H.264 detection */
  if (data[0] == 0 && data[1] == 0 && data[2] == 1) {
    if (data[3] == NAL_AUD) {
      LOGMSG("H.264 scanner: Possible H.264 NAL AUD");
      return BUF_VIDEO_H264;
    }
    if (data[3] == 0) {
      LOGDBG("H.264 scanner: Possible MPEG2 start code PICTURE (0x00)");
      return BUF_VIDEO_MPEG;
    }
    if (data[3] >= 0x80) {
      LOGDBG("H.264 scanner: Possible MPEG2 start code (0x%02x)", data[3]);
      return BUF_VIDEO_MPEG;
    }
    LOGMSG("H.264 scanner: Unregonized header 00 00 01 %02x", data[3]);
  }

  return 0;
}

static int32_t parse_video_stream(demux_xvdr_t *this, uint8_t *p, buf_element_t *buf)
{
  int32_t result;

  result = parse_pes_for_pts(this, p, buf);
  if (result < 0) return -1;

  /* Handle marker packets after still images */
  if (buf->size == 14 && result == 14 && this->pts == 0) {
    /*if (this->stream->metronom->get_option(this->stream->metronom, XVDR_METRONOM_STILL_MODE))*/ {
      LOGDBG("video fifo flush and decoder reset after still image");
      put_control_buf(this->video_fifo, this->video_fifo, BUF_CONTROL_FLUSH_DECODER);
      put_control_buf(this->video_fifo, this->video_fifo, BUF_CONTROL_RESET_DECODER);
      buf->free_buffer(buf);
      return -1;
    }
  }

  p += result;

  if (this->video_type == 0 && buf->size >= result + 4) {
    this->video_type = detect_h264(p);
  }

  buf->type      = this->video_type ?: BUF_VIDEO_MPEG;
  buf->pts       = this->pts;
  buf->decoder_info[0] = this->pts - this->dts;

  /* MPEG2 */
  if (this->video_type == BUF_VIDEO_MPEG) {
    /* Special handling for FFMPEG MPEG2 decoder */
    if (this->ffmpeg_mpeg2_decoder) {
      uint8_t type = pes_get_picture_type(buf->content, buf->size);
      if (type) {
        /* signal FRAME_END to decoder */
        post_frame_end(this, buf);
        /* for some reason ffmpeg mpeg2 decoder does not understand pts'es in B frames ?
         * (B-frame pts's are smaller than in previous P-frame)
         * Anyway, without this block of code B frames with pts are dropped. */
        if (type == B_FRAME)
          buf->pts = 0;
      }
    }
  }

  /* H.264 */
  else if (this->video_type == BUF_VIDEO_H264 &&
           buf->size        >= 4) {

    /* Access Unit Delimiter */
    if (IS_NAL_AUD(p))
      post_frame_end (this, buf);

    /* Check for end of still image.
       VDR ensures that H.264 still images end with an end of sequence NAL unit */
    uint8_t *end = buf->content + buf->size;
    if (IS_NAL_END_SEQ(end-4)) {
      LOGMSG("post_frame_h264: Still frame ? (frame ends with end of sequence NAL unit)");
      buf->decoder_flags |= BUF_FLAG_FRAME_END;
    }
  }

  buf->content   = p;
  buf->size      = this->packet_len;

  check_newpts( this, buf, PTS_VIDEO );

  this->video_fifo->put (this->video_fifo, buf);

  return -1;
}

static int32_t parse_audio_stream(demux_xvdr_t *this, uint8_t *p, buf_element_t *buf)
{
  int track;
  int32_t result;

  result = parse_pes_for_pts(this, p, buf);
  if (result < 0) return -1;

  p += result;

  track = this->stream_id & 0x1f;

  buf->content   = p;
  buf->size      = this->packet_len;
  buf->type      = BUF_AUDIO_MPEG + track;
  buf->pts       = this->pts;

  if (this->audio_fifo) {
    check_newpts( this, buf, PTS_AUDIO );
    track_audio_stream_change (this, buf);
    this->audio_fifo->put (this->audio_fifo, buf);
  } else {
    buf->free_buffer(buf);
  }

  return -1;
}

/*
 * interface
 */

static int demux_xvdr_send_chunk (demux_plugin_t *this_gen)
{
  demux_xvdr_t *this = (demux_xvdr_t *) this_gen;

  demux_xvdr_parse_pack(this);

  return this->status;
}

static void demux_xvdr_dispose (demux_plugin_t *this_gen)
{
  demux_xvdr_t *this = (demux_xvdr_t *) this_gen;

  LOGDBG("demux_xvdr_dispose()");

  ts_data_dispose(&this->ts_data);

  free (this);
}

static int demux_xvdr_get_status (demux_plugin_t *this_gen)
{
  demux_xvdr_t *this = (demux_xvdr_t *) this_gen;

  if (this->status != DEMUX_OK) {
    if (this->ts_data) {
      LOGMSG("demux_xvdr_get_status(): status != DEMUX_OK. -> freeing ts_data");
      ts_data_dispose(&this->ts_data);
    }
  }

  return this->status;
}

static void demux_xvdr_send_headers (demux_plugin_t *this_gen)
{
  demux_xvdr_t *this = (demux_xvdr_t *) this_gen;

  this->video_fifo  = this->stream->video_fifo;
  this->audio_fifo  = this->stream->audio_fifo;

  /*
   * send start buffer
   */

  _x_demux_control_start(this->stream);

  this->status = DEMUX_OK;

  _x_stream_info_set(this->stream, XINE_STREAM_INFO_HAS_VIDEO, 1);
  _x_stream_info_set(this->stream, XINE_STREAM_INFO_HAS_AUDIO, 1);
  _x_stream_info_set(this->stream, XINE_STREAM_INFO_BITRATE, 5000000);
}


static int demux_xvdr_seek (demux_plugin_t *this_gen,
                            off_t start_pos, int start_time, int playing)
{
  demux_xvdr_t *this = (demux_xvdr_t *) this_gen;

  /*
   * now start demuxing
   */
  this->send_newpts   = 1;
  this->video_type    = 0;
  this->audio_type    = 0;
  this->subtitle_type = 0;
  this->bih_posted    = 0;
  ts_data_dispose(&this->ts_data);

  if (!playing) {

    this->buf_flag_seek = 0;
    this->status        = DEMUX_OK;
    this->last_pts[0]   = 0;
    this->last_pts[1]   = 0;
  } else {
    this->buf_flag_seek = 1;
    this->last_vpts     = INT64_C(-1);
    _x_demux_flush_engine(this->stream);
  }

  return this->status;
}

/*
 * demux class
 */

static int demux_xvdr_get_stream_length (demux_plugin_t *this_gen)
{
  return 0;
}

static uint32_t demux_xvdr_get_capabilities(demux_plugin_t *this_gen)
{
  return DEMUX_CAP_NOCAP;
}

static int demux_xvdr_get_optional_data(demux_plugin_t *this_gen,
                                        void *data, int data_type)
{
  return DEMUX_OPTIONAL_UNSUPPORTED;
}

static demux_plugin_t *demux_xvdr_open_plugin (demux_class_t *class_gen,
                                               xine_stream_t *stream,
                                               input_plugin_t *input_gen)
{
  input_plugin_t *input = (input_plugin_t *) input_gen;
  demux_xvdr_t   *this;
  const char     *mrl = input->get_mrl(input);

  if (strncmp(mrl, MRL_ID ":/",       MRL_ID_LEN + 2 ) &&
      strncmp(mrl, MRL_ID "+pipe://", MRL_ID_LEN + 8) &&
      strncmp(mrl, MRL_ID "+tcp://",  MRL_ID_LEN + 7) &&
      strncmp(mrl, MRL_ID "+udp://",  MRL_ID_LEN + 7) &&
      strncmp(mrl, MRL_ID "+rtp://",  MRL_ID_LEN + 7))
    return NULL;

  this         = calloc(1, sizeof(demux_xvdr_t));
  this->stream = stream;
  this->input  = input;

  this->demux_plugin.send_headers      = demux_xvdr_send_headers;
  this->demux_plugin.send_chunk        = demux_xvdr_send_chunk;
  this->demux_plugin.seek              = demux_xvdr_seek;
  this->demux_plugin.dispose           = demux_xvdr_dispose;
  this->demux_plugin.get_status        = demux_xvdr_get_status;
  this->demux_plugin.get_stream_length = demux_xvdr_get_stream_length;
  this->demux_plugin.get_capabilities  = demux_xvdr_get_capabilities;
  this->demux_plugin.get_optional_data = demux_xvdr_get_optional_data;
  this->demux_plugin.demux_class       = class_gen;

  this->status     = DEMUX_FINISHED;

  detect_video_decoders(this);
  detect_audio_decoders(this);

  return &this->demux_plugin;
}

#if DEMUXER_PLUGIN_IFACE_VERSION < 27
static const char *demux_xvdr_get_description (demux_class_t *this_gen)
{
  return MRL_ID " demux plugin";
}

static const char *demux_xvdr_get_identifier (demux_class_t *this_gen)
{
  return MRL_ID;
}

static const char *demux_xvdr_get_extensions (demux_class_t *this_gen)
{
  return NULL;
}

static const char *demux_xvdr_get_mimetypes (demux_class_t *this_gen)
{
  return NULL;
}

static void demux_xvdr_class_dispose (demux_class_t *this_gen)
{
  demux_xvdr_class_t *this = (demux_xvdr_class_t *) this_gen;

  free (this);
}
#endif

void *demux_xvdr_init_class (xine_t *xine, void *data)
{
  demux_xvdr_class_t     *this;

  this         = calloc(1, sizeof(demux_xvdr_class_t));
  this->config = xine->config;
  this->xine   = xine;

  this->demux_class.open_plugin     = demux_xvdr_open_plugin;
#if DEMUXER_PLUGIN_IFACE_VERSION < 27
  this->demux_class.get_description = demux_xvdr_get_description;
  this->demux_class.get_identifier  = demux_xvdr_get_identifier;
  this->demux_class.get_mimetypes   = demux_xvdr_get_mimetypes;
  this->demux_class.get_extensions  = demux_xvdr_get_extensions;
  this->demux_class.dispose         = demux_xvdr_class_dispose;
#else
  this->demux_class.description     = N_("XVDR demux plugin");
  this->demux_class.identifier      = MRL_ID;
  this->demux_class.mimetypes       = NULL;
  this->demux_class.extensions      =
    MRL_ID":/ "
    MRL_ID"+pipe:/ "
    MRL_ID"+tcp:/ "
    MRL_ID"+udp:/ "
    MRL_ID"+rtp:/ "
    MRL_ID"+slave:/";
  this->demux_class.dispose         = default_demux_class_dispose;
#endif

  return this;
}


