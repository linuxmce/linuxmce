/*
 * ts2es.h: demux MPEG-TS -> ES
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: ts2es.h,v 1.2 2009-08-18 10:04:32 phintuka Exp $
 *
 */

#ifndef _DEMUX_XVDR_TS2ES_H_
#define _DEMUX_XVDR_TS2ES_H_

typedef struct ts2es_s ts2es_t;

ts2es_t        *ts2es_init    (fifo_buffer_t *dst_fifo, ts_stream_type stream_type, uint stream_index);
buf_element_t  *ts2es_put     (ts2es_t *ts2es, uint8_t *ts_packet, fifo_buffer_t *src_fifo);
void            ts2es_flush   (ts2es_t *ts2es);
void            ts2es_dispose (ts2es_t *ts2es);

#endif /* _DEMUX_XVDR_TS2ES_H_ */
