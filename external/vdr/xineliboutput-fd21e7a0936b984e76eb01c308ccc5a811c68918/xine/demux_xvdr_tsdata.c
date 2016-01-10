/*
 * demux_xvdr_tsdata.h: data for MPEG-TS demuxer
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: demux_xvdr_tsdata.c,v 1.4 2010-03-19 21:44:27 phintuka Exp $
 *
 */

#include <stdlib.h>

#include <xine/buffer.h>

#define LOG_MODULENAME "[demux_vdr] "
#define SysLogLevel    iSysLogLevel
#include "../logdefs.h"

#include "../tools/ts.h"
#include "ts2es.h"

#include "demux_xvdr_tsdata.h"

static void ts_data_ts2es_reset(ts_data_t *ts_data)
{
  int i;

  ts2es_dispose(ts_data->video);
  ts_data->video = NULL;

  for (i = 0; ts_data->audio[i]; i++) {
    ts2es_dispose(ts_data->audio[i]);
    ts_data->audio[i] = NULL;
  }

  for (i = 0; ts_data->spu[i]; i++) {
    ts2es_dispose(ts_data->spu[i]);
    ts_data->spu[i] = NULL;
  }
}

void ts_data_reset_audio(ts_data_t *ts_data, fifo_buffer_t *audio_fifo, int keep_channel)
{
  if (ts_data) {

    int i;

    for (i = 0; ts_data->audio[i]; i++) {
      if (i != keep_channel) {
        ts2es_dispose(ts_data->audio[i]);
        ts_data->audio[i] = NULL;
      }
    }

    if (audio_fifo) {
      for (i = 0; i < ts_data->pmt.audio_tracks_count; i++)
        if (!ts_data->audio[i])
          ts_data->audio[i] = ts2es_init(audio_fifo, ts_data->pmt.audio_tracks[i].type, i);
    }
  }
}

void ts_data_ts2es_init(ts_data_t **ts_data, fifo_buffer_t *video_fifo, fifo_buffer_t *audio_fifo)
{
  if (*ts_data)
    ts_data_ts2es_reset(*ts_data);
  else
    *ts_data = calloc (1, sizeof(ts_data_t));

  ts_data_t *this = *ts_data;
  int i;

  if (video_fifo) {
    if (this->pmt.video_pid != INVALID_PID)
      this->video = ts2es_init(video_fifo, this->pmt.video_type, 0);

    for (i = 0; i < this->pmt.spu_tracks_count; i++)
      this->spu[i] = ts2es_init(video_fifo, STREAM_DVBSUB, i);
  }

  if (audio_fifo) {
    for (i = 0; i < this->pmt.audio_tracks_count; i++)
      this->audio[i] = ts2es_init(audio_fifo, this->pmt.audio_tracks[i].type, i);
  }
}

void ts_data_flush(ts_data_t *ts_data)
{
  if (ts_data) {

    int i;

    if (ts_data->video)
      ts2es_flush(ts_data->video);

    for (i = 0; ts_data->audio[i]; i++)
      ts2es_flush(ts_data->audio[i]);

    for (i = 0; ts_data->spu[i]; i++)
      ts2es_flush(ts_data->spu[i]);
  }
}

void ts_data_dispose(ts_data_t **ts_data)
{
  if (*ts_data) {

    ts_data_ts2es_reset(*ts_data);

    free(*ts_data);
    *ts_data = NULL;
  }
}
