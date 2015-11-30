/*
 * demux_xvdr_tsdata.h: data for MPEG-TS demuxer
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: demux_xvdr_tsdata.h,v 1.3 2010-03-19 21:44:27 phintuka Exp $
 *
 */

#ifndef _DEMUX_XVDR_TSDATA_H_
#define _DEMUX_XVDR_TSDATA_H_

struct ts2es_s;

struct ts_data_s {
  uint16_t    pmt_pid;
  uint16_t    program_number;

  pmt_data_t  pmt;

  struct ts2es_s *video;
  struct ts2es_s *audio[TS_MAX_AUDIO_TRACKS];
  struct ts2es_s *spu[TS_MAX_SPU_TRACKS];
};

typedef struct ts_data_s ts_data_t;

void ts_data_ts2es_init (ts_data_t **ts_data, fifo_buffer_t *video_fifo, fifo_buffer_t *audio_fifo);
void ts_data_flush      (ts_data_t *ts_data);
void ts_data_dispose    (ts_data_t **ts_data);

void ts_data_reset_audio(ts_data_t *ts_data, fifo_buffer_t *audio_fifo, int keep_channel);


#endif /* _DEMUX_XVDR_TSDATA_H_ */
