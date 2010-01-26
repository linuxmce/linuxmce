/*
 * ts.c: MPEG-TS
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: ts.c,v 1.14 2009/09/09 11:58:40 phintuka Exp $
 *
 */

/*#define LOG_PCR*/
/*#define LOG_PMT*/

#include <inttypes.h>
#include <stdlib.h>
#include <string.h>

#ifndef LOG_MODULENAME
#  define LOG_MODULENAME "[mpeg-ts  ] "
#  define SysLogLevel    iSysLogLevel
#  include "../logdefs.h"
#endif

#include "mpeg.h"
#include "ts.h"
#include "pes.h"

#ifdef LOG_PMT
#  define LOGPMT LOGMSG
#else
#  define LOGPMT(x...)
#endif

#ifdef LOG_PCR
#  define LOGPCR LOGMSG
#else
#  define LOGPCR(x...)
#endif


/*
 * ts_compute_crc32()
 *
 * taken from xine-lib demux_ts.c
 */
static uint32_t ts_compute_crc32(const uint8_t *data, uint32_t length, uint32_t crc32)
{
  static uint32_t crc32_table[256];
  static uint     init_done = 0;

  if (!init_done) {
    uint32_t  i, j, k;
    init_done = 1;
    for (i = 0 ; i < 256 ; i++) {
      k = 0;
      for (j = (i << 24) | 0x800000 ; j != 0x80000000 ; j <<= 1)
        k = (k << 1) ^ (((k ^ j) & 0x80000000) ? 0x04c11db7 : 0);
      crc32_table[i] = k;
    }
  }

  uint32_t i;
  for(i = 0; i < length; i++)
    crc32 = (crc32 << 8) ^ crc32_table[(crc32 >> 24) ^ data[i]];

  return crc32;
}

/*
 * parse_pat()
 *
 * - parse PAT for PMT pid and program number
 *
 * modified from xine-lib demux_ts.c
 */
int ts_parse_pat(pat_data_t *pat, const uint8_t *pkt)
{
  const uint8_t *original_pkt = pkt;

  if (! ts_PAYLOAD_START(pkt)) {
    LOGMSG ("parse_pat: PAT without payload unit start indicator");
    return 0;
  }

  /* jump to payload */
  pkt += pkt[4]; /* pointer */
  if (pkt - original_pkt > TS_SIZE) {
    LOGMSG("parse_pat: PAT with invalid pointer");
    return 0;
  }

  uint     section_syntax_indicator = ((pkt[6] >> 7) & 0x01) ;
  uint     section_length           = ((pkt[6] & 0x03) << 8) | pkt[7];
/*uint     transport_stream_id      = (pkt[8]  << 8) | pkt[9];*/
/*uint     version_number           = (pkt[10] >> 1) & 0x1f;*/
  uint     current_next_indicator   = pkt[10] & 0x01;
  uint     section_number           = pkt[11];
  uint     last_section_number      = pkt[12];
  uint32_t crc32, calc_crc32;

  crc32 =  pkt[section_length + 4] << 24;
  crc32 |= pkt[section_length + 5] << 16;
  crc32 |= pkt[section_length + 6] << 8;
  crc32 |= pkt[section_length + 7] ;

  if ((section_syntax_indicator != 1) || !(current_next_indicator)) {
    LOGMSG("parse_pat: ssi error");
    return 0;
  }

  if (pkt - original_pkt > TS_SIZE - 4 - 1 - 3 - (int)section_length) {
    LOGMSG("parse_pat: unsupported PAT does not fit to single TS packet");
    return 0;
  }

  if ((section_number != 0) || (last_section_number != 0)) {
    LOGMSG("parse_pat: unsoupported PAT consists of multiple (%d) sections", last_section_number);
    return 0;
  }

  /* Check CRC */
  calc_crc32 = ts_compute_crc32 (pkt + 5, section_length + 3 - 4, 0xffffffff);
  if (crc32 != calc_crc32) {
    LOGMSG("parse_pat: invalid CRC");
    return 0;
  }

  /*
   * Process all programs in the program loop
   */

  const uint8_t *program;
  uint           program_count;

  program_count = 0;
  for (program = pkt + 13;
       program < pkt + 13 + section_length - 9;
       program += 4) {
    uint program_number = (program[0] << 8) | program[1];
    uint pmt_pid        = ((program[2] & 0x1f) << 8) | program[3];

    /* skip NIT pids */
    if (program_number == 0x0000)
      continue;

    pat->program_number[program_count] = program_number;
    pat->pmt_pid[program_count] = pmt_pid;

    LOGPMT("PAT acquired count=%d programNumber=0x%04x pmtPid=0x%04x",
           program_count,
           pat->program_number[program_count],
           pat->pmt_pid[program_count]);

    program_count++;
  }

  pat->program_number[program_count] = 0;

  return program_count;
}

/*
 * ts_get_reg_desc()
 *
 * Find the registration code (tag=5) and return it as a uint32_t
 * This should return "AC-3" or 0x41432d33 for AC3/A52 audio tracks.
 *
 * taken from xine-lib demux_ts.c
 */
static void ts_get_reg_desc(uint32_t *dest, const uint8_t *data, int length)
{
  const unsigned char *d = data;

  while (d < (data + length)) {
    if (d[0] == 5 && d[1] >= 4) {
      *dest = (d[2] << 24) | (d[3] << 16) | (d[4] << 8) | d[5];
      LOGPMT("parse_pmt: found registration format identifier 0x%.4x", *dest);
      return;
    }
    d += 2 + d[1];
  }
  LOGPMT("pare_pmt: found no format id");
  *dest = 0;
}

static int find_audio_track(pmt_data_t *pmt, uint pid)
{
  int i;
  for (i = 0; i < pmt->audio_tracks_count; i++) {
    if (pmt->audio_tracks[i].pid == pid)
      return i;
  }
  return -1;
}

/*
 * ts_parse_pmt()
 *
 * modified from xine-lib demux_ts.c
 */
int ts_parse_pmt (pmt_data_t *pmt, uint program_no, const uint8_t *pkt)
{
  const uint8_t *originalPkt = pkt;
  const uint8_t *ptr         = NULL;
  uint           pusi        = ts_PAYLOAD_START(pkt);

  uint32_t section_syntax_indicator;
  uint32_t section_length = 0; /* to calm down gcc */
  uint32_t program_number;
  uint32_t version_number;
  uint32_t current_next_indicator;
  uint32_t section_number;
  uint32_t last_section_number;
  uint32_t program_info_length;
  uint32_t crc32;
  uint32_t calc_crc32;
  uint32_t coded_length;
  uint     pid;
  uint8_t *stream;
  uint     i;
  int      count;
  uint8_t  len;
  uint     offset = 0;

  /*
   * A new section should start with the payload unit start
   * indicator set. We allocate some mem (max. allowed for a PM section)
   * to copy the complete section into one chunk.
   */
  if (pusi) {
    pkt += pkt[4]; /* pointer */
    offset = 1;

    if (pmt->pmt != NULL) 
      free(pmt->pmt);
    pmt->pmt = (uint8_t *) calloc(4096, sizeof(uint8_t));
    pmt->pmt_write_ptr = pmt->pmt;

    section_syntax_indicator  = (pkt[6] >> 7) & 0x01;
    section_length            = ((pkt[6] << 8) | pkt[7]) & 0x03ff;
    program_number            =  (pkt[8] << 8) | pkt[9];
    version_number            = (pkt[10] >> 1) & 0x1f;
    current_next_indicator    =  pkt[10] & 0x01;
    section_number            =  pkt[11];
    last_section_number       =  pkt[12];

    LOGPMT("PMT: section_syntax: %d", section_syntax_indicator);
    LOGPMT("     section_length: %d", section_length);
    LOGPMT("     program_number: %#.4x", program_number);
    LOGPMT("     version_number: %d", version_number);
    LOGPMT("     c/n indicator:  %d", current_next_indicator);
    LOGPMT("     section_number: %d", section_number);
    LOGPMT("     last_section_number: %d", last_section_number);

    if ((section_syntax_indicator != 1) || !current_next_indicator) {
      LOGMSG("parse_pmt: ssi error");
      return 0;
    }

    if (program_number != program_no) {
      /* several programs can share the same PMT pid */
      LOGMSG("parse_pmt: program number %i, looking for %i", program_number, program_no);
      return 0;
    }

    if ((section_number != 0) || (last_section_number != 0)) {
      LOGMSG("parse_pmt: unsupported PMT (%d sections)", last_section_number);
      return 0;
    }
  }

  if (!pusi) {
    section_length = (pmt->pmt[1] << 8 | pmt->pmt[2]) & 0x03ff;
    version_number = (pkt[10] >> 1) & 0x1f;
  }

  count = ts_PAYLOAD_SIZE(originalPkt);

  ptr = originalPkt + offset + (TS_SIZE - count);
  len = count - offset;
  memcpy(pmt->pmt_write_ptr, ptr, len);
  pmt->pmt_write_ptr += len;

  if (pmt->pmt_write_ptr < pmt->pmt + section_length) {
    LOGPMT("parse_pmt: didn't get all PMT TS packets yet...");
    return 0;
  }

  if (!section_length) {
    free(pmt->pmt);
    pmt->pmt = NULL;
    LOGMSG("parse_pmt: zero-length section");
    return 0;
  }

  LOGPMT("parse_pmt: have all TS packets for the PMT section");

  crc32  = (uint32_t) pmt->pmt[section_length+3-4] << 24;
  crc32 |= (uint32_t) pmt->pmt[section_length+3-3] << 16;
  crc32 |= (uint32_t) pmt->pmt[section_length+3-2] << 8;
  crc32 |= (uint32_t) pmt->pmt[section_length+3-1] ;

  /* Check CRC. */
  calc_crc32 = ts_compute_crc32 (pmt->pmt, section_length + 3 - 4, 0xffffffff);
  if (crc32 != calc_crc32) {
    LOGMSG("parse_pmt: invalid CRC32");
    return 0;
  }

  if (crc32 == pmt->crc32 && version_number == pmt->version_number) {
    LOGPMT("parse_pmt: PMT with CRC32=%d already parsed. Skipping.", crc32);
    return 0;
  }

  LOGPMT("parse_pmt: new PMT, parsing...");
  pmt->crc32 = crc32;
  pmt->version_number = version_number;

  /* reset PIDs */
  pmt->audio_tracks_count = 0;
  pmt->spu_tracks_count = 0;
  pmt->video_pid = INVALID_PID;

  /* ES definitions start here */
  program_info_length = ((pmt->pmt[10] << 8) | pmt->pmt[11]) & 0x0fff;

  stream = &pmt->pmt[12] + program_info_length;
  coded_length = 13 + program_info_length;
  if (coded_length > section_length) {
    LOGMSG("parse_pmt: PMT with inconsistent progInfo length");
    return 0;
  }
  section_length -= coded_length;


  /*
   * Extract the elementary streams.
   */
  while (section_length > 0) {
    unsigned int stream_info_length;

    pid = ((stream[1] << 8) | stream[2]) & 0x1fff;
    stream_info_length = ((stream[3] << 8) | stream[4]) & 0x0fff;
    coded_length = 5 + stream_info_length;
    if (coded_length > section_length) {
      LOGMSG("parse_pmt: PMT with inconsistent streamInfo length");
      return 0;
    }

    switch (stream[0]) {
      case ISO_11172_VIDEO:
      case ISO_13818_VIDEO:
      case ISO_14496_PART2_VIDEO:
      case ISO_14496_PART10_VIDEO:
        LOGPMT("parse_pmt: video pid 0x%.4x type %2.2x", pid, stream[0]);
        if (pmt->video_pid == INVALID_PID) {
          pmt->video_pid  = pid;
          pmt->video_type = (ts_stream_type)stream[0];
        }
        break;

      case ISO_11172_AUDIO:
      case ISO_13818_AUDIO:
      case ISO_13818_PART7_AUDIO:
      case ISO_14496_PART3_AUDIO:
        if (pmt->audio_tracks_count < TS_MAX_AUDIO_TRACKS) {
          if (find_audio_track(pmt, pid) < 0) {
            LOGPMT("parse_pmt: audio pid 0x%.4x type %2.2x", pid, stream[0]);
            pmt->audio_tracks[pmt->audio_tracks_count].pid  = pid;
            pmt->audio_tracks[pmt->audio_tracks_count].type = (ts_stream_type)stream[0];
            /* ts_get_lang_desc(pmt->audio_tracks[pmt->audio_tracks_count].lang, */
            /*                  stream + 5, stream_info_length); */
            pmt->audio_tracks_count++;
          }
        }
        break;

      case ISO_13818_PRIVATE:
      case ISO_13818_TYPE_C:
        break;
      case ISO_13818_PES_PRIVATE:
        for (i = 5; i < coded_length; i += stream[i+1] + 2) {
          if ((stream[i] == STREAM_DESCR_AC3) && (pmt->audio_tracks_count < TS_MAX_AUDIO_TRACKS)) {
            if (find_audio_track(pmt, pid) < 0) {
              LOGPMT("parse_pmt: AC3 audio pid 0x%.4x type %2.2x", pid, stream[0]);
              pmt->audio_tracks[pmt->audio_tracks_count].pid  = pid;
              pmt->audio_tracks[pmt->audio_tracks_count].type = STREAM_AUDIO_AC3;
              /* demux_ts_get_lang_desc(pmt->audio_tracks[pmt->audio_tracks_count].lang, */
              /*                        stream + 5, stream_info_length); */
              pmt->audio_tracks_count++;
              break;
            }
          }
          /* DVBSUB */
          else if (stream[i] == STREAM_DESCR_DVBSUB) {
            uint pos;
            for (pos = i + 2;
                 pos + 8 <= i + 2 + stream[i + 1]
                   && pmt->spu_tracks_count < TS_MAX_SPU_TRACKS;
                 pos += 8) {
              int no = pmt->spu_tracks_count;

              pmt->spu_tracks_count++;

              memcpy(pmt->spu_tracks[no].lang, &stream[pos], 3);
              pmt->spu_tracks[no].lang[3] = 0;
              pmt->spu_tracks[no].comp_page_id = (stream[pos + 4] << 8) | stream[pos + 5];
              pmt->spu_tracks[no].aux_page_id = (stream[pos + 6] << 8) | stream[pos + 7];
              pmt->spu_tracks[no].pid = pid;

              LOGPMT("parse_pmt: DVBSUB pid 0x%.4x: %s  page %d %d type %2.2x", pid,
                     pmt->spu_tracks[no].lang, pmt->spu_tracks[no].comp_page_id,
                     pmt->spu_tracks[no].aux_page_id, stream[0]);
            }
          }
        }
        break;

      default:

        /* This following section handles all the cases where the audio track info is stored
         * in PMT user info with stream id >= 0x80
         * We first check that the stream id >= 0x80, because all values below that are
         * invalid if not handled above, then we check the registration format identifier
         * to see if it holds "AC-3" (0x41432d33) and if is does, we tag this as an audio stream.
         */
        if ((pmt->audio_tracks_count < TS_MAX_AUDIO_TRACKS) && (stream[0] >= 0x80) ) {
          if (find_audio_track(pmt, pid) < 0) {
            uint32_t format_identifier = 0;
            ts_get_reg_desc(&format_identifier, stream + 5, stream_info_length);
            /* If no format identifier, assume A52 */
            if ((format_identifier == 0x41432d33) || (format_identifier == 0)) {
              pmt->audio_tracks[pmt->audio_tracks_count].pid  = pid;
              pmt->audio_tracks[pmt->audio_tracks_count].type = (ts_stream_type)stream[0];
              /* ts_get_lang_desc(pmt->audio_tracks[pmt->audio_tracks_count].lang, */
              /*                  stream + 5, stream_info_length); */
              pmt->audio_tracks_count++;
              break;
            }
          }
        } else {
          LOGPMT("parse_pmt: unknown stream_type: 0x%.2x pid: 0x%.4x", stream[0], pid);
        }
        break;
    }
    stream += coded_length;
    section_length -= coded_length;
  }


  /*
   * Get the current PCR PID.
   */
  pid = ((pmt->pmt[8] << 8) | pmt->pmt[9]) & 0x1fff;
  if (pmt->pcr_pid != pid) {

    if (pmt->pcr_pid == INVALID_PID)
      LOGPMT("parse_pmt: pcr pid 0x%.4x", pid);
    else
      LOGPMT("parse_pmt: pcr pid changed 0x%.4x", pid);

    pmt->pcr_pid = pid;
  }

  return 1;
}

/*
 * ts_get_pcr()
 */
static int ts_get_pcr_1(const uint8_t *pkt, int64_t *ppcr)
{
  if (!ts_ADAPT_FIELD_EXISTS(pkt)) {
    return 0;
  }

  if (ts_HAS_ERROR(pkt)) {
    LOGMSG("ts_get_pcr: transport error");
    return 0;
  }

  /* pcr flag ? */
  if (! (pkt[5] & 0x10))
    return 0;

  int64_t pcr;
  uint    epcr;

  pcr  = ((int64_t) pkt[6]) << 25;
  pcr += (int64_t) (pkt[7]  << 17);
  pcr += (int64_t) (pkt[8]  << 9);
  pcr += (int64_t) (pkt[9]  << 1);
  pcr += (int64_t) ((pkt[10] & 0x80) >> 7);

  epcr = ((pkt[10] & 0x1) << 8) | pkt[11];

  LOGPCR("ts_get_pcr: PCR: %"PRId64", EPCR: %u", pcr, epcr);
  *ppcr = pcr;
  return 1;
}

int64_t ts_get_pcr(const uint8_t *pkt)
{
  int64_t pcr = NO_PTS;
  ts_get_pcr_1(pkt, &pcr);
  return pcr;
}

int ts_get_pcr_n(const uint8_t *pkt, int npkt, int64_t *pcr)
{
  pkt += TS_SIZE * npkt;
  while (npkt > 0) {
    npkt--;
    pkt -= TS_SIZE;
    if (ts_get_pcr_1(pkt, pcr))
      return 1;
  }
  return 0;
}


/*
 * ts_state_t
 */

struct ts_state_s {

  uint8_t  pusi_seen;

  uint8_t  inside_pes; /* Scanning ES (PES start code seen and skipped) */

  uint32_t buf_len;    /* bytes queued */
  uint32_t buf_size;   /* buffer size */
  uint8_t  buf[0];     /* payload: partial PES / video stream header etc. */
};

ts_state_t *ts_state_init(int buffer_size)
{
  if (buffer_size < 8 * TS_SIZE)
    buffer_size = 8 * TS_SIZE;

  ts_state_t *ts = (ts_state_t*)calloc(1, sizeof(ts_state_t) + buffer_size);
  ts->buf_size = buffer_size;
  return ts;
}

void ts_state_reset(ts_state_t *ts)
{
  int buf_size = ts->buf_size;
  memset(ts, 0, sizeof(ts_state_t));
  ts->buf_size = buf_size;
}

void ts_state_dispose(ts_state_t *ts)
{
  free(ts);
}

/*
 * ts_add_payload()
 *
 * Add TS packet payload to buffer.
 *  - PUSI resets the buffer
 *  - all data before first PUSI is discarded
 */
static int ts_add_payload(ts_state_t *ts, const uint8_t *data)
{
  /* start from PUSI */
  if (!ts->pusi_seen) {
    if (!ts_PAYLOAD_START(data))
      return 0;
    ts->pusi_seen = 1;
    ts->buf_len   = 0;
  }

  if (ts->buf_len >= ts->buf_size - TS_SIZE) {
    LOGMSG("ts_add_payload: buffer full");
    ts->buf_len -= TS_SIZE;
    memcpy(ts->buf, ts->buf+TS_SIZE, ts->buf_len);
  }

  int len = ts_PAYLOAD_SIZE(data);
  if (len > 0) {
    memcpy(ts->buf + ts->buf_len, ts_GET_PAYLOAD(data), len);
    ts->buf_len += len;
  }

  return ts->buf_len;
}

/*
 * ts_skip_payload()
 */
static void ts_skip_payload(ts_state_t *ts, unsigned int n)
{
  if (n < ts->buf_len) {
    ts->buf_len -= n;
    memcpy(ts->buf, ts->buf + n, ts->buf_len);
  } else {
    ts->buf_len = 0;
  }
}

/*
 * ts_scan_startcode()
 *
 * - discard all data until startcode (00 00 01) is found
 * - returns number of bytes left
 */
static int ts_scan_startcode(ts_state_t *ts)
{
  if (ts->buf_len > 2) {
    /* scan for PES or MPEG 00 00 01 */
    unsigned int i = 0, n = ts->buf_len - 2;
    while (i < n) {
      if (ts->buf[i+2] != 1)
        i += 3;
      else if(ts->buf[i+1])
        i += 2;
      else if(ts->buf[i])
        i++;
      else
        break;
    }

    /* skip data until start code */
    ts_skip_payload(ts, i);
  }

  return ts->buf_len;
}

/*
 * ts_get_pes()
 *
 * - scan for PES start
 * - return (PES) bytes queued
 */
static int ts_get_pes(ts_state_t *ts, const uint8_t *data)
{
  if (ts_add_payload(ts, data) > 0)
    return ts_scan_startcode(ts);
  return 0;
}

/*
 * ts_get_pts()
 */

int64_t ts_get_pts(ts_state_t *ts, const uint8_t *data)
{
  int64_t pts = NO_PTS;
  int     cnt = ts_get_pes(ts, data);

  if (cnt > 14) {
    pts = pes_get_pts(ts->buf, ts->buf_len);

    if (pts < 0 && cnt > 2*TS_SIZE)
      ts_state_reset(ts);
  }

  return pts;
}

/*
 * ts_get_picture_type()
 */

int ts_get_picture_type(ts_state_t *ts, const uint8_t *data, int h264)
{
  int pic = NO_PICTURE;
  return pic;
}

/*
 * ts_get_video_size()
 */

int ts_get_video_size(ts_state_t *ts, const uint8_t *data, video_size_t *size, int h264)
{
  return 0;
}
