/*
 * ts.h: MPEG-TS header definitions
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: ts.h,v 1.19 2013-01-06 21:23:43 phintuka Exp $
 *
 */

#ifndef _XINELIBOUTPUT_TS_H_
#define _XINELIBOUTPUT_TS_H_

#ifdef __cplusplus
extern "C" {
#endif


/* Avoid warnings when included to VDR plugin */
#undef TS_SYNC_BYTE
#undef TS_SIZE
#undef TS_PAYLOAD_EXISTS
#undef TS_ADAPT_FIELD_EXISTS
#undef TS_PAYLOAD_START
#undef TS_ERROR
#undef TS_PID_MASK_HI

/*
 * Constants
 */

#define TS_SYNC_BYTE          0x47
#define TS_SIZE               188
#define TS_ADAPT_FIELD_EXISTS 0x20
#define TS_PAYLOAD_EXISTS     0x10
#define TS_PAYLOAD_START      0x40
#define TS_ERROR              0x80
#define TS_PID_MASK_HI        0x1F

#define ts_HAS_PAYLOAD(ts)      ((ts)[3] & TS_PAYLOAD_EXISTS)
#define ts_PAYLOAD_START(ts)    ((ts)[1] & TS_PAYLOAD_START)
#define ts_HAS_ERROR(ts)        ((ts)[1] & TS_ERROR)
#define ts_PID(ts)            ((((ts)[1] & TS_PID_MASK_HI) << 8) + (ts)[2])
#define ts_PAYLOAD_OFFSET(ts)  (((ts)[3] & TS_ADAPT_FIELD_EXISTS) ? (ts)[4] + 5 : 4)

#define ts_GET_PAYLOAD(ts)      ((ts) + ts_PAYLOAD_OFFSET(ts))
#define ts_PAYLOAD_SIZE(ts)     (TS_SIZE - ts_PAYLOAD_OFFSET(ts))

#define ts_ADAPT_FIELD_EXISTS(ts)  ((ts)[3] & TS_ADAPT_FIELD_EXISTS)
#define ts_ADAPT_FIELD_LENGTH(ts)  (ts_ADAPT_FIELD_EXISTS(ts) ? (ts)[4] : 0)


#define DATA_IS_TS(data)        ((data)[0] == TS_SYNC_BYTE)

/*
 * stream types (PMT)
 */

typedef enum {
  ISO_11172_VIDEO        = 0x01,  /* ISO/IEC 11172 Video */
  ISO_13818_VIDEO        = 0x02,  /* ISO/IEC 13818-2 Video */
  ISO_11172_AUDIO        = 0x03,  /* ISO/IEC 11172 Audio */
  ISO_13818_AUDIO        = 0x04,  /* ISO/IEC 13818-3 Audi */
  ISO_13818_PRIVATE      = 0x05,  /* ISO/IEC 13818-1 private sections */
  ISO_13818_PES_PRIVATE  = 0x06,  /* ISO/IEC 13818-1 PES packets containing private data */
  ISO_13522_MHEG         = 0x07,  /* ISO/IEC 13512 MHEG */
  ISO_13818_DSMCC        = 0x08,  /* ISO/IEC 13818-1 Annex A  DSM CC */
  ISO_13818_TYPE_A       = 0x0a,  /* ISO/IEC 13818-6 Multiprotocol encapsulation */
  ISO_13818_TYPE_B       = 0x0b,  /* ISO/IEC 13818-6 DSM-CC U-N Messages */
  ISO_13818_TYPE_C       = 0x0c,  /* ISO/IEC 13818-6 Stream Descriptors */
  ISO_13818_TYPE_D       = 0x0d,  /* ISO/IEC 13818-6 Sections (any type, including private data) */
  ISO_13818_AUX          = 0x0e,  /* ISO/IEC 13818-1 auxiliary */
  ISO_13818_PART7_AUDIO  = 0x0f,  /* ISO/IEC 13818-7 Audio with ADTS transport sytax */
  ISO_14496_PART2_VIDEO  = 0x10,  /* ISO/IEC 14496-2 Visual (MPEG-4) */
  ISO_14496_PART3_AUDIO  = 0x11,  /* ISO/IEC 14496-3 Audio with LATM transport syntax */
  ISO_14496_PART10_VIDEO = 0x1b,  /* ISO/IEC 14496-10 Video (MPEG-4 part 10/AVC, aka H.264) */
  STREAM_VIDEO_MPEG      = 0x80,
  STREAM_AUDIO_AC3       = 0x81,

  STREAM_VIDEO_VC1       = 0xea,

  /* private streams, payload type detected from PMT stream descriptor tag */

  STREAM_AUDIO_EAC3      = 0x7a06, // ISO_13818_PES_PRIVATE | (STREAM_DESCR_EAC3   << 8)
  STREAM_AUDIO_DTS       = 0x7b06, // ISO_13818_PES_PRIVATE | (STREAM_DESCR_DTS    << 8)
  STREAM_AUDIO_AAC       = 0x7c06, // ISO_13818_PES_PRIVATE | (STREAM_DESCR_AAC    << 8)
  STREAM_DVBSUB          = 0x5906, // ISO_13818_PES_PRIVATE | (STREAM_DESCR_DVBSUB << 8)

  /* HDMV */
  STREAM_HDMV             = 0xff00,
  HDMV_AUDIO_80_PCM       = 0x80 | STREAM_HDMV, /* BluRay PCM */
  HDMV_AUDIO_82_DTS       = 0x82 | STREAM_HDMV, /* DTS */
  HDMV_AUDIO_83_TRUEHD    = 0x83 | STREAM_HDMV, /* Dolby TrueHD, primary audio */
  HDMV_AUDIO_84_EAC3      = 0x84 | STREAM_HDMV, /* Dolby Digital plus, primary audio */
  HDMV_AUDIO_85_DTS_HRA   = 0x85 | STREAM_HDMV, /* DTS-HRA */
  HDMV_AUDIO_86_DTS_HD_MA = 0x86 | STREAM_HDMV, /* DTS-HD Master audio */

  HDMV_SPU_90_BITMAP      = 0x90 | STREAM_HDMV, /* Presentation Graphics */
  HDMV_SPU_91_INTERACTIVE = 0x91 | STREAM_HDMV, /* Interactive Graphics */
  HDMV_SPU_92_TEXT        = 0x92 | STREAM_HDMV, /* Text subtitles */

} ts_stream_type;

/* stream info descriptors */
#define STREAM_DESCR_DVBSUB 0x59
#define STREAM_DESCR_AC3    0x6a
#define STREAM_DESCR_EAC3   0x7a
#define STREAM_DESCR_DTS    0x7b
#define STREAM_DESCR_AAC    0x7c

/*
 * PAT
 */

#define TS_MAX_PROGRAMS     64
#define TS_MAX_PMTS         32
#define TS_MAX_AUDIO_TRACKS 32
#define TS_MAX_SPU_TRACKS   32

typedef struct {
  uint16_t program_number[TS_MAX_PROGRAMS];
  uint16_t pmt_pid[TS_MAX_PROGRAMS];

  uint8_t  version;
  uint32_t crc32;
  uint8_t  pat_changed_flag;
} pat_data_t;

/*
 * ts_parse_pat()
 *
 * returns n : number of programs in PAT
 *         0 : error or unchanged PAT
 */

int ts_parse_pat(pat_data_t *pat_data, const uint8_t *ts_data);


/*
 * PMT
 */

#define INVALID_PID 0xffff

typedef struct {
  uint8_t         *pmt; /* raw data */
  uint8_t         *pmt_write_ptr;

  uint32_t         crc32;
  uint             version_number;

  uint16_t         pcr_pid;
  uint16_t         video_pid;
  ts_stream_type   video_type;

  uint8_t          audio_tracks_count;
  uint8_t          spu_tracks_count;

  struct {
    uint16_t       pid;
    ts_stream_type type;
    /*uint8_t        lang[8];*/
  } audio_tracks[TS_MAX_AUDIO_TRACKS];

  struct {
    uint16_t       pid;
    uint8_t        lang[8];
    uint16_t       comp_page_id;
    uint16_t       aux_page_id;
  } spu_tracks[TS_MAX_SPU_TRACKS];

} pmt_data_t;

/*
 * ts_parse_pmt()
 *
 * returns 1 : PMT parsed and changed
 *         0 : error or unchanged PMT
 */
int ts_parse_pmt(pmt_data_t *pmt, uint program_no, const uint8_t *ts_data);

/*
 * PCR
 */

int64_t ts_get_pcr(const uint8_t *data);
int     ts_get_pcr_n(const uint8_t *pkt, int npkt, int64_t *pcr);

/*
 * TS->ES, simple ES parsers
 */

typedef struct ts_state_s ts_state_t;
struct video_size_s;

ts_state_t *ts_state_init(size_t buffer_size);
void        ts_state_reset(ts_state_t *ts);
void        ts_state_dispose(ts_state_t *ts);

int64_t ts_get_pts(ts_state_t *ts, const uint8_t *data);
int     ts_get_picture_type(ts_state_t *ts, const uint8_t *data, int h264);
int     ts_get_video_size(ts_state_t *ts, const uint8_t *data, struct video_size_s *size, int h264);


#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif // _XINELIBOUTPUT_TS_H_
