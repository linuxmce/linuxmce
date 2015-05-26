/*
 * xine_input_vdr.c: xine VDR input plugin
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: xine_input_vdr.c,v 1.366 2014-06-23 12:20:28 phintuka Exp $
 *
 */


#define XINE_ENGINE_INTERNAL

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <poll.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <errno.h>
#include <sys/time.h>
#include <dlfcn.h>
#include <sys/resource.h> /* setpriority() */
#include <sys/stat.h>
#include <syslog.h>
#include <ctype.h>

#if !defined(__APPLE__) && !defined(__FreeBSD__)
# define DVD_STREAMING_SPEED
#endif

#ifdef DVD_STREAMING_SPEED
# include <linux/cdrom.h>
# include <scsi/sg.h>
#endif

#include <xine/xine_internal.h>
#include <xine/xineutils.h>
#include <xine/input_plugin.h>
#include <xine/plugin_catalog.h>
#include <xine/io_helper.h>
#include <xine/buffer.h>
#include <xine/post.h>

#ifndef XINE_VERSION_CODE 
# error XINE_VERSION_CODE undefined !
#endif

#if XINE_VERSION_CODE >= 10190
# include "features.h"
# ifdef HAVE_LIBAVUTIL
#  include <libavutil/mem.h>
# else
#  error "plugin was configured without libavutil. It can't be compiled against xine-lib 1.2 !"
# endif
#endif

#include "xine/adjustable_scr.h"
#include "xine/osd_manager.h"
#include "xine/xvdr_metronom.h"

#include "xine_input_vdr.h"
#include "xine_input_vdr_net.h"

#include "tools/osd_command.h"
#include "tools/mpeg.h"
#include "tools/pes.h"
#include "tools/ts.h"
#include "tools/rle.h"

/***************************** DEFINES *********************************/

/*#define LOG_UDP*/
/*#define LOG_OSD*/
/*#define LOG_CMD*/
/*#define LOG_SCR*/
/*#define LOG_SCR_BUF_LEVEL_METER*/
/*#define LOG_TRACE*/
/*#define LOG_GRAPH*/
/*#define DEBUG_LOCKING*/

#define METRONOM_PREBUFFER_VAL  (4 * 90000 / 25 )
#define HD_BUF_NUM_BUFS         (2500)  /* 2k payload * 2500 = 5MB */
#define HD_BUF_ELEM_SIZE        (2048+64)
#define RADIO_MAX_BUFFERS       10

#define SLAVE_VIDEO_FIFO_SIZE 1000

#ifndef NOSIGNAL_IMAGE_FILE
#  define  NOSIGNAL_IMAGE_FILE "/usr/share/vdr/xineliboutput/nosignal.mpv"
#endif
#ifndef NOSIGNAL_MAX_SIZE
#  define NOSIGNAL_MAX_SIZE 0x10000 /* 64k */
#endif

/*
  Note:
  I tried to set speed to something very small instead of full pause
  when pausing SCR but it didn't work in all systems. 
  TEST_SCR_PAUSE replaces this by adding delay before stream 
  is paused (pause is triggered by first received PES containing PTS).
  This should allow immediate processing of still frames and let video_out
  run in paused_loop when there is gap in feed (ex. channel can't be decrypted).
  Not running video_out in paused_loop caused very long delays in 
  OSD updating in some setups.
*/
#define TEST_SCR_PAUSE

/* picture-in-picture support */
/*#define TEST_PIP 1*/


#define CONTROL_BUF_BASE  (                 0x0f000000) /* 0x0f000000 */
#define CONTROL_BUF_BLANK (CONTROL_BUF_BASE|0x00010000) /* 0x0f010000 */
#define CONTROL_BUF_CLEAR (CONTROL_BUF_BASE|0x00020000) /* 0x0f020000 */
#define BUF_NETWORK_BLOCK (BUF_DEMUX_BLOCK |0x00010000) /* 0x05010000 */
#define BUF_LOCAL_BLOCK   (BUF_DEMUX_BLOCK |0x00020000) /* 0x05020000 */

typedef struct {
  uint64_t pos;
  uint8_t  payload[0];
} stream_local_header_t;

#define SPU_CHANNEL_NONE   (-2)
#define SPU_CHANNEL_AUTO   (-1)

/******************************* LOG ***********************************/

#if !defined(__APPLE__) && !defined(__FreeBSD__)
#  include <linux/unistd.h> /* syscall(__NR_gettid) */
#endif

static const char module_revision[] = "$Id: xine_input_vdr.c,v 1.366 2014-06-23 12:20:28 phintuka Exp $";
static const char log_module_input_vdr[] = "[input_vdr] ";
#define LOG_MODULENAME log_module_input_vdr
#define SysLogLevel    iSysLogLevel

#include "logdefs.h"

int iSysLogLevel  = 1; /* 0:none, 1:errors, 2:info, 3:debug */
int bLogToSysLog  = 0;
int bSymbolsFound = 0;

void x_syslog(int level, const char *module, const char *fmt, ...)
{
  va_list argp;
  char buf[512];
  va_start(argp, fmt);
  vsnprintf(buf, sizeof(buf), fmt, argp);
  buf[sizeof(buf)-1] = 0;
#if defined(__APPLE__) || defined(__FreeBSD__)
  if(!bLogToSysLog) {
    fprintf(stderr, "%s%s\n", module, buf);
  } else {
    syslog(level, "%s%s", module, buf);
  }
#else
  if(!bLogToSysLog) {
    fprintf(stderr,"[%ld] %s%s\n", syscall(__NR_gettid), module, buf);
  } else {
    syslog(level, "[%ld] %s%s", syscall(__NR_gettid), module, buf);
  }
#endif
  va_end(argp);
}

static void SetupLogLevel(void)
{
  void *lib = NULL;
  if( !(lib = dlopen (NULL, RTLD_LAZY | RTLD_GLOBAL))) {
    LOGERR("Can't dlopen self: %s", dlerror());
  } else {
    int *pLogToSyslog = (int*)dlsym(lib, "LogToSysLog");
    int *pSysLogLevel = (int*)dlsym(lib, "SysLogLevel");
    bLogToSysLog = pLogToSyslog && *pLogToSyslog;
    iSysLogLevel = pSysLogLevel ? (*pSysLogLevel) : iSysLogLevel;
    LOGDBG("Symbol SysLogLevel %s : value %d", 
	   pSysLogLevel ? "found" : "not found", iSysLogLevel);
    LOGDBG("Symbol LogToSysLog %s : value %s", 
	   pLogToSyslog ? "found" : "not found", bLogToSysLog ? "yes" : "no");
    bSymbolsFound = pSysLogLevel && pLogToSyslog;
    dlclose(lib);
  }
}

#ifdef LOG_SCR
#  define LOGSCR(x...) LOGMSG("SCR: " x)
#  define LOGSCR_VERBOSE(x...) LOGVERBOSE("SCR: " x)
#else
#  define LOGSCR(x...)
#  define LOGSCR_VERBOSE(x...)
#endif
#ifdef LOG_OSD
#  define LOGOSD(x...) LOGMSG("OSD: " x)
#else
#  define LOGOSD(x...)
#endif
#ifdef LOG_UDP
#  define LOGUDP(x...) LOGMSG("UDP:" x)
#else
#  define LOGUDP(x...)
#endif
#ifdef LOG_CMD
#  define LOGCMD(x...) LOGMSG("CMD:" x)
#else
#  define LOGCMD(x...)
#endif
#ifdef LOG_TRACE
#  undef TRACE
#  define TRACE(x...) printf(x)
#else
#  undef TRACE
#  define TRACE(x...)
#endif


#ifdef DEBUG_LOCKING
# include "tools/debug_mutex.h"
#endif

#ifndef MIN
# define MIN(a,b) ((a)<(b)?(a):(b))
#endif
#ifndef MAX
# define MAX(a,b) ((a)>(b)?(a):(b))
#endif

#ifdef LOG_GRAPH
static void log_graph(int val, int symb)
{
  static char headr[] = "|<- 0                                                                                       100% ->|";
  static char meter[sizeof(headr)];

  if (!symb || symb == 1) {
    time_t t;
    struct tm *tm;

    time(&t);
    tm = localtime(&t);
    printf("%02d:%02d:%02d %s", tm->tm_hour, tm->tm_min, tm->tm_sec, symb ? meter : headr);
    memset(meter, ' ', sizeof(headr) - 1);
    return;
  }

  val = MIN(val, (int)sizeof(headr) - 2);
  val = MAX(val, 0);
#if 0
  if (symb == ':') {
    meter[val] = meter[val] == '%' ? '#' : symb;
  } else if (symb == '*') {
    meter[val] = meter[val] == '%' ? '1' :
      meter[val] == ':' ? '2' :
      meter[val] == '#' ? '3' : symb;
  } else {
    meter[val] = symb;
  }
#else
  meter[val] = symb;
#endif
}
#endif

/******************************* DATA ***********************************/

#define KILOBYTE(x)   (1024 * (x))

typedef struct udp_data_s udp_data_t;
typedef struct slave_stream_s slave_stream_t;

/* plugin class */
typedef struct vdr_input_class_s {
  input_class_t   input_class;
  xine_t         *xine;
  char           *mrls[ 2 ];
  int             fast_osd_scaling;
  int             smooth_scr_tuning;
  double          scr_tuning_step;
  int             num_buffers_hd;
  uint            scr_treshold_sd;
  uint            scr_treshold_hd;

} vdr_input_class_t;

struct slave_stream_s {
  xine_stream_t      *stream;
  xine_event_queue_t *event_queue;
};


/* input plugin */
typedef struct vdr_input_plugin_s {
  union {
    vdr_input_plugin_if_t      iface;
    struct {
      input_plugin_t           input_plugin;
      vdr_input_plugin_funcs_t funcs; 
    };
  };

  /* plugin */
  vdr_input_class_t  *class;
  xine_stream_t      *stream;
  xine_event_queue_t *event_queue;
  osd_manager_t      *osd_manager;

  char               *mrl;

  xine_stream_t      *pip_stream;

  /* Sync */
  pthread_mutex_t     lock;
  pthread_mutex_t     vdr_entry_lock;
  pthread_cond_t      engine_flushed;

  /* Playback */
  uint8_t             read_timeouts;        /* number of timeouts in read_block */
  uint8_t             write_overflows;
  uint8_t             no_video : 1;
  uint8_t             live_mode : 1;
  uint8_t             still_mode : 1;
  uint8_t             stream_start : 1;
  uint8_t             hd_stream : 1;        /* true if current stream is HD */
  uint8_t             sw_volume_control : 1;
  uint8_t             config_ok : 1;

  /* metronom */
  xvdr_metronom_t    *metronom;

  /* SCR */
  adjustable_scr_t   *scr;
  int16_t             scr_tuning;
  uint8_t             fixed_scr     : 1;
  uint8_t             scr_live_sync : 1;
  uint8_t             is_paused     : 1;
  uint8_t             is_trickspeed : 1;
  struct {
    /* buffer level data for scr tuning algorithm */
    uint cnt;
    uint fill_avg;
    uint fill_min;
    uint fill_max;
  } scr_buf;
  uint                I_frames;   /* amount of I-frames passed to demux */
  uint                B_frames;
  uint                P_frames;

  /* Network */
  pthread_t           control_thread;
  pthread_mutex_t     fd_control_lock;
  buf_element_t      *read_buffer;
  uint8_t             threads_initialized;
  uint8_t             tcp, udp, rtp;
  volatile int        control_running;
  volatile int        fd_data;
  volatile int        fd_control;
  udp_data_t         *udp_data;
  int                 client_id;
  int                 token;

  /* buffer */
  fifo_buffer_t      *block_buffer;  /* blocks to be demuxed */
  fifo_buffer_t      *buffer_pool;   /* stream's video fifo */
  fifo_buffer_t      *hd_buffer;     /* more buffer for HD streams */
  uint64_t            discard_index; /* index of next byte to feed to demux; 
					all data before this offset will 
					be discarded */
  uint64_t            discard_index_ds;
  uint                discard_frame;
  uint64_t            guard_index;   /* data before this offset will not be discarded */
  uint                guard_frame;
  uint64_t            curpos;        /* current position (demux side) */
  uint                curframe;
  uint                reserved_buffers;

  /* media player */
  slave_stream_t      slave;          /* slave stream (media player) data */
  slave_stream_t      bg_stream;      /* background image stream data */
  int                 autoplay_size;  /* size of slave stream autoplaylist (ex. CD tracks) */
  uint8_t             loop_play : 1;
  uint8_t             dvd_menu : 1;

  /* saved video properties */
  uint8_t video_properties_saved;
  int     orig_hue;
  int     orig_brightness;
  int     orig_saturation;
  int     orig_sharpness;
  int     orig_noise_reduction;
  int     orig_contrast;
  int     orig_vo_aspect_ratio;

} vdr_input_plugin_t;


/***************************** UDP DATA *********************************/

struct udp_data_s {

  /* Server address (used to validate incoming packets) */
  struct sockaddr_in server_address;
  uint32_t           ssrc;

  /* receiving queue for re-ordering and re-transmissions */
  buf_element_t *queue[UDP_SEQ_MASK+1];
  uint64_t       queue_input_pos;  /* stream position of next incoming byte */
  uint16_t       queued;   /* count of frames in queue */
  uint16_t       next_seq; /* expected sequence number of next incoming packet */

  uint16_t current_seq;  /* sequence number of last received packet */
  uint8_t  is_padding;   /* true, if last received packet was padding packet */

  /* missing frames ratio statistics */
  int16_t  missed_frames;
  int16_t  received_frames;

  /* SCR adjust */
  uint8_t  scr_jump_done;

  int resend_requested;
};

/* UDP sequence number handling */
#define NEXTSEQ(x)  ((x + 1) & UDP_SEQ_MASK)
#define PREVSEQ(x)  ((x + UDP_SEQ_MASK) & UDP_SEQ_MASK)
#define INCSEQ(x)   (x = NEXTSEQ(x))
#define ADDSEQ(x,n) ((x + UDP_SEQ_MASK + 1 + n) & UDP_SEQ_MASK)

static udp_data_t *init_udp_data(void)
{
  udp_data_t *data = calloc(1, sizeof(udp_data_t));

  data->received_frames = -1;

  return data;
}

static void free_udp_data(udp_data_t *data)
{
  int i;

  for (i = 0; i <= UDP_SEQ_MASK; i++)
    if (data->queue[i]) {
      data->queue[i]->free_buffer(data->queue[i]);
      data->queue[i] = NULL;
    }
  free(data);
}

/********************* cancellable mutex locking *************************/

/*
 * mutex cleanup()
 *
 *  Unlock mutex. Used as thread cleanup handler.
 */
static void mutex_cleanup(void *arg)
{
  pthread_mutex_unlock((pthread_mutex_t *)arg);
}

/*
 * mutex_lock_cancellable() / mutex_unlock_cancellable()
 *
 * mutex lock/unlock for cancellable sections
 *
 *  - do not enter protected section if locking fails
 *  - unlock mutex if thread is cancelled while holding the lock
 *
 *  - lock/unlock must be used pairwise within the same lexical scope !
 *
 */
#define mutex_lock_cancellable(mutex)                   \
  if (pthread_mutex_lock(mutex)) {                      \
    LOGERR("pthread_mutex_lock (%s) failed, skipping locked block !", #mutex); \
  } else {						\
    pthread_cleanup_push(mutex_cleanup, (void*) mutex);

#define mutex_unlock_cancellable(mutex)                     \
    if (pthread_mutex_unlock(mutex))                        \
      LOGERR("pthread_mutex_unlock (%s) failed !", #mutex); \
    pthread_cleanup_pop(0);                                 \
  }

/****************************** DEBUG **********************************/

#define CHECK_LOCKED(lock)                                              \
  if (!pthread_mutex_trylock(&lock)) {                                  \
    LOGMSG("%s: assertion failed: lock %s unlocked !", __PRETTY_FUNCTION__, #lock); \
    pthread_mutex_unlock(&lock);                                        \
    return;                                                             \
  }

#define CHECK_FALSE(flag) \
  if (flag) {                                                           \
    LOGMSG("%s: assertion failed: %s is true !", __PRETTY_FUNCTION__, #flag); \
    return;                                                             \
  }

/******************************* SCR ***********************************/
/*
 * SCR fine tuning
 *
 * fine tuning is used to change playback speed in live mode
 * to keep in sync with mpeg source
 *
 */

#define SCR_TUNING_PAUSED -10000
#define SCR_TUNING_OFF     0

#ifdef LOG_SCR
static inline const char *scr_tuning_str(int value)
{
  switch (value) {
    case 2:  return "SCR +2";
    case 1:  return "SCR +1";
    case SCR_TUNING_OFF:  return "SCR +0";
    case -1: return "SCR -1";
    case -2: return "SCR -2";
    case SCR_TUNING_PAUSED: return "SCR PAUSED";
    default: break;
  }
  return "ERROR";
}
#endif

#ifdef LOG_SCR
static void log_buffer_fill(vdr_input_plugin_t *this, int num_used, int num_free, int num_vid)
{
  /*
   * Trace current buffer and tuning status
   */

#ifndef LOG_SCR_BUF_LEVEL_METER
  static int cnt = 0;
  if ( ! ((cnt++) % 2500) ||
       (this->scr_tuning == SCR_TUNING_PAUSED && !(cnt%20)) ||
       (this->no_video                        && !(cnt%50))) {
    LOGSCR("Buffer %2d%% (%3d/%3d) %12s frames %d",
           100 * num_used / (num_used + num_free),
           num_used, num_used + num_free,
           scr_tuning_str(this->scr_tuning),
           num_vid);
  }
#else
  printf("Buffer %2d%% (%3d/%3d) %20s frames: %3d     \r",
         100 * num_used / (num_used + num_free),
         num_used, num_used + num_free,
         scr_tuning_str(this->scr_tuning),
         num_vid);
#endif
  if (this->scr_tuning == SCR_TUNING_PAUSED) {
    if (_x_get_fine_speed(this->stream) != XINE_SPEED_PAUSE) {
      LOGMSG("ERROR: SCR PAUSED ; speed=%d bool=%d",
	     _x_get_fine_speed(this->stream),
	     (int)_x_get_fine_speed(this->stream) == XINE_SPEED_PAUSE);
      _x_set_fine_speed(this->stream, XINE_SPEED_PAUSE);
    }
  }
}
#else
#  define log_buffer_fill(this,used,free,num_vid)
#endif

static void scr_tuning_set_paused(vdr_input_plugin_t *this)
{
  CHECK_LOCKED(this->lock);
  CHECK_FALSE(this->is_trickspeed);
  CHECK_FALSE(this->is_paused);
  CHECK_FALSE(this->slave.stream);
  if (this->still_mode)
    return;

  if (this->scr_tuning != SCR_TUNING_PAUSED) {

    this->scr_tuning = SCR_TUNING_PAUSED;  /* marked as paused */

    this->scr->set_speed_tuning(this->scr, 1.0);

    if (_x_get_fine_speed(this->stream) != XINE_SPEED_PAUSE) {
      _x_set_fine_speed(this->stream, XINE_SPEED_PAUSE);
    }

    this->I_frames = this->P_frames = this->B_frames = 0;
  }
}

static void reset_scr_tuning(vdr_input_plugin_t *this)
{
  CHECK_LOCKED(this->lock);

  if (this->scr_tuning != SCR_TUNING_OFF) {

    CHECK_FALSE(this->is_trickspeed);
    CHECK_FALSE(this->is_paused);

    this->scr_tuning = SCR_TUNING_OFF; /* marked as normal */

    this->scr->set_speed_tuning(this->scr, 1.0);

    if (_x_get_fine_speed(this->stream) != XINE_FINE_SPEED_NORMAL) {
      if (!this->is_paused)
        _x_set_fine_speed(this->stream, XINE_FINE_SPEED_NORMAL);
      else
        LOGDBG("reset_scr_tuning: playback is paused");
    }

    this->scr->scr.set_fine_speed(&this->scr->scr, XINE_FINE_SPEED_NORMAL);
  }
}

static void vdr_adjust_realtime_speed(vdr_input_plugin_t *this)
{
  CHECK_LOCKED(this->lock);
  CHECK_FALSE(this->still_mode);
  CHECK_FALSE(this->is_trickspeed);
  CHECK_FALSE(this->is_paused);

  /*
   * Grab current buffer usage
   */

  unsigned num_used, num_free, fill_level;

  /* amount of free buffers */

  if (this->hd_stream)
    num_free = this->hd_buffer->num_free(this->hd_buffer);
  else
    num_free = this->buffer_pool->num_free(this->buffer_pool);

  if (num_free < this->reserved_buffers)
    num_free = 0;
  else
    num_free -= this->reserved_buffers;

  /* amount of used buffers */

  num_used = this->buffer_pool->size(this->buffer_pool) +
             this->block_buffer->size(this->block_buffer);

  if (this->stream->audio_fifo)
    num_used += this->stream->audio_fifo->size(this->stream->audio_fifo);

  /* fill level */

  fill_level = 100 * num_used / (num_used + num_free);

  log_buffer_fill(this, num_used, num_free, num_frames);


  /*
   *
   */

  int scr_tuning = this->scr_tuning;

  /*
   * SCR -> PAUSE
   *  - If buffer is empty, pause SCR (playback) for a while
   */
  if (num_used < 1 &&
      scr_tuning != SCR_TUNING_PAUSED &&
      !this->no_video && !this->still_mode && !this->is_trickspeed) {

    int num_frames = this->stream->video_out->get_property(this->stream->video_out, VO_PROP_BUFS_IN_FIFO);

    if (num_frames < 5) {
      LOGSCR("SCR paused by adjust_speed, vbufs = %d", num_frames);
      scr_tuning_set_paused(this);
      return;
    }

    LOGSCR("adjust_speed: no pause, enough vbufs queued (%d)", num_frames);
  }

  /* SCR -> RESUME
   *  - If SCR (playback) is currently paused due to previous buffer underflow,
   *    revert to normal if SCR-Treshold > SD-/HD-Treshold (configured in xine-config)
   *    and Audio-Treshold > 0.
   *  - SD- and HD-Streams will be handled separately to improve SCR-playback after channel switch.
   *  - SCR-Treshold calculation based on configured SD-/HD-Buffers in xine-config.
   *  - Audio-Treshold calculation based on fixed 500 Audio-Buffers,
   *    to handle different data rates (DVB-S/DVB-C/DVB-T) and SD-/HD-channels with one treshold.
   */

  if (scr_tuning == SCR_TUNING_PAUSED) {
    uint scr_treshold = 100 * num_used / (num_used + num_free);
    LOGSCR("SCR-T %2d%%, FB %d, UB %d", scr_treshold, num_free, num_used);
    uint audio_treshold = 100 * this->stream->audio_fifo->size
                          (this->stream->audio_fifo) / (this->stream->audio_fifo->size
                          (this->stream->audio_fifo) + 500);
    LOGSCR("Audio-T %2d%%, UB %d", audio_treshold, this->stream->audio_fifo->size
                                                  (this->stream->audio_fifo));
    if (   (this->hd_stream  && scr_treshold > this->class->scr_treshold_hd && (audio_treshold > 0 || scr_treshold > 65))
        || (!this->hd_stream && scr_treshold > this->class->scr_treshold_sd && (audio_treshold > 0 || scr_treshold > 65))
        || (this->no_video && num_used > 5)
        || this->still_mode
        || this->is_trickspeed
        || (this->I_frames > 0 && (this->I_frames > 2 || this->P_frames > 6))) {

      LOGSCR("SCR tuning resetted by adjust_speed, "
             "I %d B %d P %d", this->I_frames, this->B_frames, this->P_frames);
      this->I_frames = 0;
      reset_scr_tuning(this);
    }

  /*
   * Adjust SCR rate
   *  - Live data is coming in at rate defined by sending transponder,
   *    there is no way to change it -> we must adapt to it
   *  - when playing realtime (live) stream, adjust our SCR to keep
   *    xine buffers half full. This efficiently synchronizes our SCR
   *    to transponder SCR and prevents buffer under/overruns caused by
   *    minor differences in clock speeds.
   *  - if buffer is getting empty, slow don SCR by 0.5...1%
   *  - if buffer is getting full, speed up SCR by 0.5...1%
   *
   *  TODO: collect simple statistics and try to calculate more exact
   *        clock rate difference to minimize SCR speed changes
   */
  } else if (_x_get_fine_speed(this->stream) == XINE_FINE_SPEED_NORMAL) {

    if (!this->scr_live_sync) {
        scr_tuning = SCR_TUNING_OFF;

    } else if (this->no_video) {  /* radio stream ? */
      if( num_used >= RADIO_MAX_BUFFERS - 1)
        scr_tuning = +1; /* play faster */
      else if( num_used <= RADIO_MAX_BUFFERS / 3)
        scr_tuning = -1; /* play slower */
      else
        scr_tuning = SCR_TUNING_OFF;

    } else if (this->class->smooth_scr_tuning) {

      /*
       * Experimental only.
       * Major sync point displacements are handled by xine-lib.
       * This provides much faster sync after channel switch, replay start etc.
       */

      int trim_rel, trim_act;

      #define DIVIDER 8000
      #define WEIGHTING 2
      #define CENTER_POS 0
      #define MAX_TRIM_REL 1
      #define MAX_TRIM_ABS 2
      #define MIN_FILL_PER_CENT 0
      #define MAX_FILL_PER_CENT 100
      #define TARGET_FILL_PER_CENT 50

#ifdef LOG_GRAPH
      if (!this->scr_buf.cnt) {
        log_graph(0, 0);
        printf("  R\n");
      }
#endif
      trim_act = scr_tuning - CENTER_POS;
      this->scr_buf.fill_avg += fill_level;
      this->scr_buf.fill_min = MIN(this->scr_buf.fill_min, fill_level);
      this->scr_buf.fill_max = MAX(this->scr_buf.fill_max, fill_level);

#ifdef LOG_GRAPH
      log_graph(fill_level, '.');
#endif
      ++this->scr_buf.cnt;
      if (!(this->scr_buf.cnt % DIVIDER)) {
        this->scr_buf.fill_avg /= DIVIDER;
        trim_rel = (this->scr_buf.fill_avg - TARGET_FILL_PER_CENT) / WEIGHTING;
        trim_rel = MIN(trim_rel,  MAX_TRIM_REL);
        trim_rel = MAX(trim_rel, -MAX_TRIM_REL);

#ifdef LOG_GRAPH
        log_graph(this->scr_buf.fill_avg, '|');
        log_graph(0, 1);
        printf(" %2d%% %2d%% %2d%% [%3d%+4d]\n",
               this->scr_buf.fill_min,
               this->scr_buf.fill_max,
               this->scr_buf.fill_avg,
               trim_act, trim_rel);
#endif

        this->scr_buf.fill_avg = 0;
        this->scr_buf.fill_min = MAX_FILL_PER_CENT;
        this->scr_buf.fill_max = MIN_FILL_PER_CENT;

        if (trim_rel) {
          trim_act += trim_rel;
          trim_act = MIN(trim_act,  MAX_TRIM_ABS);
          trim_act = MAX(trim_act, -MAX_TRIM_ABS);
          /* reprog clock correction */
          scr_tuning = trim_act + CENTER_POS;
        }
      }

    } else {
      if (fill_level > 85) {
        scr_tuning = +2; /* play 1% faster */
      } else if (fill_level > 70) {
        scr_tuning = +1; /* play .5% faster */
      } else if (fill_level < 15) {
        scr_tuning = -2; /* play 1% slower */
      } else if (fill_level < 30) {
        scr_tuning = -1; /* play .5% slower */
      } else if ((scr_tuning > 0 && num_free > num_used) ||
                 (scr_tuning < 0 && num_used > num_free)) {
        scr_tuning = SCR_TUNING_OFF;
      }
    }

    if (scr_tuning != this->scr_tuning) {
      LOGSCR("scr_tuning: %s -> %s (buffer %d/%d) (tuning now %f%%)",
             scr_tuning_str(this->scr_tuning),
             scr_tuning_str(scr_tuning), num_used, num_free, this->class->scr_tuning_step * scr_tuning * 100.0);
      this->scr_tuning = scr_tuning;

      /* make it play .5% / 1% faster or slower */
      if (this->scr)
        this->scr->set_speed_tuning(this->scr, 1.0 + (this->class->scr_tuning_step * scr_tuning) );
    }

  /*
   * SCR -> NORMAL
   *  - If we are in replay (or trick speed) mode, switch SCR tuning off
   *    as we can always have complete control on incoming data rate
   */
  } else if (this->scr_tuning) {
    reset_scr_tuning(this);
  }
}

/******************************* TOOLS ***********************************/

static char *strn0cpy(char *dest, const char *src, int n)
{
  char *s = dest;
  for ( ; --n && (*dest = *src) != 0; dest++, src++) ;
  *dest = 0;
  return s;
}

static char *unescape_filename(const char *fn)
{
  char *d = strdup(fn), *s = d, *result = d;
  while(*s && *s != '#') {
    if(s[0] == '%' && s[1] && s[2]) {
      unsigned int c;
      if (sscanf(s+1, "%02x", &c) == 1) {
	*d++ = (char)c;
	s += 3;
	continue;
      }
    }
    *d++ = *s++;
  }
  *d = 0;
  return result;
}

static void create_timeout_time(struct timespec *abstime, int timeout_ms)
{
  struct timeval now;
  gettimeofday(&now, NULL);
  now.tv_usec += timeout_ms * 1000;
  while (now.tv_usec >= 1000000) {   /* take care of an overflow */
    now.tv_sec++;
    now.tv_usec -= 1000000;
  }
  abstime->tv_sec = now.tv_sec;
  abstime->tv_nsec = now.tv_usec * 1000;
}

/**************************** socket I/O *********************************/

/*
 * io_select_rd()
 *
 * - poll socket for read
 * - timeouts in 500 ms
 * - returns XIO_*
 */
static int io_select_rd (int fd)
{
  fd_set fdset, eset;
  int ret;
  struct timeval select_timeout;

  if(fd < 0)
    return XIO_ERROR;

  FD_ZERO (&fdset);
  FD_ZERO (&eset);
  FD_SET  (fd, &fdset);
  FD_SET  (fd, &eset);

  select_timeout.tv_sec  = 0;
  select_timeout.tv_usec = 500*1000; /* 500 ms */
  errno = 0;
  ret = select (fd + 1, &fdset, NULL, &eset, &select_timeout);

  if (ret == 0)
    return XIO_TIMEOUT;
  if (ret < 0) {
    if (errno == EINTR || errno == EAGAIN)
      return XIO_TIMEOUT;
    return XIO_ERROR;
  }

  if (FD_ISSET(fd, &eset))
    return XIO_ERROR;
  if (FD_ISSET(fd, &fdset))
    return XIO_READY;

  return XIO_TIMEOUT;
}

/*
 * write_control_data()
 *
 * - write len bytes to control socket.
 * - returns number of bytes written, < 0 on error.
 *
 * NOTE: caller must hold fd_control lock !
 */
static ssize_t write_control_data(vdr_input_plugin_t *this, const void *str, size_t len)
{
  size_t ret, result = len;

  while (len > 0) {

    if (!this->control_running) {
      LOGMSG("write_control aborted");
      return -1;
    }

    /* poll the socket */
    fd_set fdset, eset;
    struct timeval select_timeout;
    FD_ZERO (&fdset);
    FD_ZERO (&eset);
    FD_SET  (this->fd_control, &fdset);
    FD_SET  (this->fd_control, &eset);
    select_timeout.tv_sec  = 0;
    select_timeout.tv_usec = 500*1000; /* 500 ms */
    errno = 0;
    if (1 != select (this->fd_control + 1, NULL, &fdset, &eset, &select_timeout) ||
        !FD_ISSET(this->fd_control, &fdset) ||
        FD_ISSET(this->fd_control, &eset)) {
      LOGERR("write_control failed (poll timeout or error)");
      this->control_running = 0;
      return -1;
    }

    if (!this->control_running) {
      LOGERR("write_control aborted");
      return -1;
    }

    errno = 0;
    ret = write (this->fd_control, str, len);

    if (ret <= 0) {
      if (ret == 0) {
        LOGMSG("write_control: disconnected");
      } else if (errno == EAGAIN) {
        LOGERR("write_control failed: EAGAIN");
        continue;
      } else if (errno == EINTR) {
        LOGERR("write_control failed: EINTR");
        pthread_testcancel();
        continue;
      } else {
        LOGERR("write_control failed");
      }
      this->control_running = 0;
      return -1;
    }
    len -= ret;
    str  = (uint8_t*)str + ret;
  }

  return result;
}

/*
 * write_control()
 *
 * - write null-terminated string to control socket.
 * - returns number of bytes written, < 0 on error
 *
 * NOTE: caller should NOT hold fd_control lock !
 */
static ssize_t write_control(vdr_input_plugin_t *this, const char *str)
{
  ssize_t ret = -1;
  mutex_lock_cancellable (&this->fd_control_lock);
  ret = write_control_data(this, str, strlen(str));
  mutex_unlock_cancellable (&this->fd_control_lock);
  return ret;
}

/*
 * printf_control()
 *
 * - returns number of bytes written, < 0 on error
 *
 * NOTE: caller should NOT hold fd_control lock !
 */
static ssize_t printf_control(vdr_input_plugin_t *this, const char *fmt, ...)
{
  va_list argp;
  char    buf[512];
  ssize_t result;

  va_start(argp, fmt);
  vsnprintf(buf, sizeof(buf), fmt, argp);
  buf[sizeof(buf)-1] = 0;

  result = write_control(this, buf);

  va_end(argp);

  return result;
}

/*
 * readline_control()
 *
 * - read CR/LF terminated string from control socket
 * - remove trailing CR/LF
 * - returns > 0 : length of string
 *           = 0 : timeout
 *           < 0 : error
 */
static ssize_t readline_control(vdr_input_plugin_t *this, char *buf, size_t maxlen, int timeout)
{
  int     poll_result;
  ssize_t read_result;
  size_t  total_bytes = 0;

  *buf = 0;
  while (total_bytes < maxlen - 1) {

    if (!this->control_running && timeout < 0)
      return -1;

    pthread_testcancel();
    poll_result = io_select_rd(this->fd_control);
    pthread_testcancel();

    if (!this->control_running && timeout < 0)
      return -1;

    if (poll_result == XIO_TIMEOUT) {
      if (timeout == 0)
        return 0;
      if (timeout > 0)
        timeout--;
      continue;
    }
    if (poll_result == XIO_ABORTED) {
      LOGERR("readline_control: XIO_ABORTED at [%u]", (uint)total_bytes);
      continue;
    }
    if (poll_result != XIO_READY /* == XIO_ERROR */) {
      LOGERR("readline_control: poll error at [%u]", (uint)total_bytes);
      return -1;
    }

    errno = 0;
    read_result = read (this->fd_control, buf + total_bytes, 1);
    pthread_testcancel();

    if (!this->control_running && timeout < 0)
      return -1;

    if (read_result <= 0) {
      if (read_result == 0)
        LOGERR("Control stream disconnected");
      else
        LOGERR("readline_control: read error at [%u]", (uint)total_bytes);
      if (read_result < 0 && (errno == EINTR || errno == EAGAIN))
        continue;
      return -1;
    }

    if (buf[total_bytes]) {
      if (buf[total_bytes] == '\r') {
        buf[total_bytes] = 0;
      } else if (buf[total_bytes] == '\n') {
        buf[total_bytes] = 0;
        break;
      } else {
        total_bytes ++;
        buf[total_bytes] = 0;
      }
    }
    TRACE("readline_control: %d bytes ... %s\n", len, buf);
  }

  TRACE("readline_control: %d bytes (max %d)\n", len, maxlen);

  return total_bytes;
}

/*
 * read_control()
 *
 * - read len bytes from control socket
 * - returns < 0 on error
 */
static ssize_t read_control(vdr_input_plugin_t *this, uint8_t *buf, size_t len)
{
  int     poll_result;
  ssize_t num_bytes;
  size_t  total_bytes = 0;

  while (total_bytes < len) {

    if (!this->control_running)
      return -1;

    pthread_testcancel();
    poll_result = io_select_rd(this->fd_control);
    pthread_testcancel();

    if (!this->control_running)
      return -1;

    if (poll_result == XIO_TIMEOUT) {
      continue;
    }
    if (poll_result == XIO_ABORTED) {
      LOGERR("read_control: XIO_ABORTED");
      continue;
    }
    if (poll_result == XIO_ERROR) {
      LOGERR("read_control: poll error");
      return -1;
    }

    errno = 0;
    num_bytes = read (this->fd_control, buf + total_bytes, len - total_bytes);
    pthread_testcancel();

    if (num_bytes <= 0) {
      if (this->control_running && num_bytes < 0)
        LOGERR("read_control read() error");
      return -1;
    }
    total_bytes += num_bytes;
  }

  return total_bytes;
}

static void puts_vdr(vdr_input_plugin_t *this, const char *s)
{
  if (this->fd_control < 0) {
    if (this->funcs.xine_input_event) {
      this->funcs.xine_input_event(this->funcs.fe_handle, s, NULL);
    }
  } else {
    write_control(this, s);
  }
}

static void printf_vdr(vdr_input_plugin_t *this, const char *fmt, ...)
{
  va_list argp;
  char    buf[512];

  va_start(argp, fmt);
  vsnprintf(buf, sizeof(buf), fmt, argp);
  buf[sizeof(buf)-1] = 0;

  puts_vdr(this, buf);

  va_end(argp);
}

/************************** BUFFER HANDLING ******************************/

static void buffer_pool_free (buf_element_t *element) 
{
  fifo_buffer_t *this = (fifo_buffer_t *) element->source;

  pthread_mutex_lock (&this->buffer_pool_mutex);

  element->next = this->buffer_pool_top;
  this->buffer_pool_top = element;

  this->buffer_pool_num_free++;
  if (this->buffer_pool_num_free > this->buffer_pool_capacity) {
    LOGERR("xine-lib:buffer: There has been a fatal error: TOO MANY FREE's");
    _x_abort();
  }

  if(this->buffer_pool_num_free > 20)
    pthread_cond_signal (&this->buffer_pool_cond_not_empty);

  pthread_mutex_unlock (&this->buffer_pool_mutex);
}

static buf_element_t *fifo_buffer_try_get(fifo_buffer_t *fifo)
{
  int i;
  buf_element_t *buf;

  pthread_mutex_lock (&fifo->mutex);

  if (fifo->first==NULL) {
    pthread_mutex_unlock (&fifo->mutex);
    return NULL;
  }

  buf = fifo->first;

  fifo->first = fifo->first->next;
  if (fifo->first==NULL)
    fifo->last = NULL;

  fifo->fifo_size--;
  fifo->fifo_data_size -= buf->size;

  for(i = 0; fifo->get_cb[i]; i++)
    fifo->get_cb[i](fifo, buf, fifo->get_cb_data[i]);

  pthread_mutex_unlock (&fifo->mutex);

  return buf;
}

static buf_element_t *fifo_buffer_timed_get(fifo_buffer_t *fifo, int timeout)
{
  buf_element_t *buf = fifo_buffer_try_get (fifo);

  if (!buf) {
    struct timespec abstime;
    int result = 0;
    create_timeout_time (&abstime, timeout);

    mutex_lock_cancellable (&fifo->mutex);
    while (fifo->first == NULL && !result)
      result = pthread_cond_timedwait (&fifo->not_empty, &fifo->mutex, &abstime);
    mutex_unlock_cancellable (&fifo->mutex);
    buf = fifo_buffer_try_get (fifo);
  }

  return buf;
}

static void signal_buffer_pool_not_empty(vdr_input_plugin_t *this)
{
  if (this->buffer_pool) {
    pthread_mutex_lock(&this->buffer_pool->buffer_pool_mutex);
    pthread_cond_broadcast(&this->buffer_pool->buffer_pool_cond_not_empty);
    pthread_mutex_unlock(&this->buffer_pool->buffer_pool_mutex);
  }
  if (this->hd_buffer) {
    pthread_mutex_lock(&this->hd_buffer->buffer_pool_mutex);
    pthread_cond_broadcast(&this->hd_buffer->buffer_pool_cond_not_empty);
    pthread_mutex_unlock(&this->hd_buffer->buffer_pool_mutex);
  }
}

static void signal_buffer_not_empty(vdr_input_plugin_t *this)
{
  if(this->block_buffer) {
    pthread_mutex_lock(&this->block_buffer->mutex);
    pthread_cond_broadcast(&this->block_buffer->not_empty);
    pthread_mutex_unlock(&this->block_buffer->mutex);
  }
}

#if XINE_VERSION_CODE < 10190
#  define fifo_buffer_new(stream, n, s) _x_fifo_buffer_new(n, s)
#else
static fifo_buffer_t *fifo_buffer_new (xine_stream_t *stream, int num_buffers, uint32_t buf_size)
{
  fifo_buffer_t *ref = stream->video_fifo;
  fifo_buffer_t *this;
  int            i;
  unsigned char *multi_buffer;

  LOGDBG("fifo_buffer_new...");
  this = calloc(1, sizeof (fifo_buffer_t));

  this->first               = NULL;
  this->last                = NULL;
  this->fifo_size           = 0;
  this->put                 = ref->put;
  this->insert              = ref->insert;
  this->get                 = ref->get;
  this->clear               = ref->clear;
  this->size                = ref->size;
  this->num_free            = ref->num_free;
  this->data_size           = ref->data_size;
  this->dispose             = ref->dispose;
  this->register_alloc_cb   = ref->register_alloc_cb;
  this->register_get_cb     = ref->register_get_cb;
  this->register_put_cb     = ref->register_put_cb;
  this->unregister_alloc_cb = ref->unregister_alloc_cb;
  this->unregister_get_cb   = ref->unregister_get_cb;
  this->unregister_put_cb   = ref->unregister_put_cb;
  pthread_mutex_init (&this->mutex, NULL);
  pthread_cond_init (&this->not_empty, NULL);

  /*
   * init buffer pool, allocate nNumBuffers of buf_size bytes each
   */

  multi_buffer = this->buffer_pool_base = av_mallocz (num_buffers * buf_size);

  pthread_mutex_init (&this->buffer_pool_mutex, NULL);
  pthread_cond_init (&this->buffer_pool_cond_not_empty, NULL);

  this->buffer_pool_capacity  = num_buffers;
  this->buffer_pool_buf_size  = buf_size;
  this->buffer_pool_alloc     = ref->buffer_pool_alloc;
  this->buffer_pool_try_alloc = ref->buffer_pool_try_alloc;

  for (i = 0; i<num_buffers; i++) {
    buf_element_t *buf;

    buf = calloc(1, sizeof (buf_element_t));

    buf->mem = multi_buffer;
    multi_buffer += buf_size;

    buf->max_size    = buf_size;
    buf->free_buffer = buffer_pool_free;
    buf->source      = this;
    buf->extra_info  = malloc(sizeof(extra_info_t));

    buffer_pool_free (buf);
  }

  LOGDBG("fifo_buffer_new done.");
  return this;
}
#endif

static void flush_all_fifos (vdr_input_plugin_t *this, int full)
{
  LOGDBG("flush_all_fifos(%s)", full ? "full" : "");

  if (this->read_buffer) {
    this->read_buffer->free_buffer(this->read_buffer);
    this->read_buffer = NULL;
  }

  if (this->udp_data) {
    int i;
    for (i = 0; i <= UDP_SEQ_MASK; i++)
      if (this->udp_data->queue[i]) {
        this->udp_data->queue[i]->free_buffer(this->udp_data->queue[i]);
        this->udp_data->queue[i] = NULL;
      }
  }

  if (full) {
    if (this->stream && this->stream->audio_fifo)
      this->stream->audio_fifo->clear(this->stream->audio_fifo);
    if (this->stream && this->stream->video_fifo)
      this->stream->video_fifo->clear(this->stream->video_fifo);
  }

  if (this->block_buffer)
    this->block_buffer->clear(this->block_buffer);
  if (this->hd_buffer)
    this->hd_buffer->clear(this->hd_buffer);
}

static void wait_fifos_empty(xine_stream_t *stream, int timeout_ms)
{
  int V, A;

  do {
    V = stream->video_fifo->size(stream->video_fifo);
    A = stream->audio_fifo->size(stream->audio_fifo);
    LOGVERBOSE("wait_fifos_empty: video %d, audio %d", V, A);

    if (V <= 0 && A <= 0)
      return;

    xine_usec_sleep(5*1000);
    timeout_ms -= 5;

  } while (timeout_ms > 0);

  LOGMSG("wait_fifos_empty: timeout! video=%d audio=%d", V, A);
}

static buf_element_t *get_buf_element(vdr_input_plugin_t *this, int size, int force)
{
  buf_element_t *buf = NULL;

  /* HD buffer */
  if (this->hd_stream && size <= HD_BUF_ELEM_SIZE) {
    if (this->hd_buffer->buffer_pool_num_free > (int)this->reserved_buffers)
      buf = this->hd_buffer->buffer_pool_try_alloc(this->hd_buffer);
    if (!force && !buf)
      return NULL;
  }

  /* limit max. buffered data */
  if(!force && !buf) {
    if (this->buffer_pool->buffer_pool_num_free < (int)this->reserved_buffers)
      return NULL;
  }

  /* get smallest possible buffer */
  if(!buf) {
    if(size < 8000)
      buf = this->buffer_pool->buffer_pool_try_alloc(this->buffer_pool);
    else if(size < 0xffff) {
      buf = this->block_buffer->buffer_pool_try_alloc(this->block_buffer);
      LOGDBG("get_buf_element: big PES (%d bytes) !", size);
    }
    else { /* len>64k */
      LOGDBG("get_buf_element: jumbo PES (%d bytes) !", size);
    }
  }

  /* final try from audio fifo */
  if(!buf)
    buf = this->stream->audio_fifo->buffer_pool_try_alloc(this->stream->audio_fifo);

  /* set up defaults */
  if (buf) {
    buf->content = buf->mem;
    buf->size = 0;
    buf->type = BUF_DEMUX_BLOCK;
    buf->pts  = 0;

    buf->free_buffer = buffer_pool_free;
  }

  return buf;
}

static buf_element_t *get_buf_element_timed(vdr_input_plugin_t *this, int size, int timeout)
{
  buf_element_t *buf = get_buf_element (this, size, 0);
  if (!buf) {
    int             result = 0;
    fifo_buffer_t  *fifo   = this->hd_stream ? this->hd_buffer : this->buffer_pool;
    struct timespec abstime;
    create_timeout_time (&abstime, timeout);

    do {
      mutex_lock_cancellable (&fifo->buffer_pool_mutex);
      result = pthread_cond_timedwait (&fifo->buffer_pool_cond_not_empty, &fifo->buffer_pool_mutex, &abstime);
      mutex_unlock_cancellable (&fifo->buffer_pool_mutex);
      buf = get_buf_element (this, size, 0);
    } while (!buf && !result);
  }
  return buf;
}

/*
 * strip_network_headers()
 *
 * Remove network headers from buffer and update buffer type.
 * Update current stream position.
 * - caller must hold this->lock !
 */
static void strip_network_headers(vdr_input_plugin_t *this, buf_element_t *buf)
{
  CHECK_LOCKED(this->lock);

  if (buf->type == BUF_LOCAL_BLOCK) {
    stream_local_header_t *header = (stream_local_header_t *)buf->content;
    this->curpos  = header->pos;

    buf->content += sizeof(stream_local_header_t);
    buf->size    -= sizeof(stream_local_header_t);
    buf->type     = BUF_DEMUX_BLOCK;

    return;
  }

  if (buf->type == BUF_NETWORK_BLOCK) {
    if (this->udp || this->rtp) {
      stream_udp_header_t *header = (stream_udp_header_t *)buf->content;
      this->curpos  = header->pos;
      buf->content += sizeof(stream_udp_header_t);
      buf->size    -= sizeof(stream_udp_header_t);
    } else {
      stream_tcp_header_t *header = (stream_tcp_header_t *)buf->content;
      this->curpos  = header->pos;
      buf->content += sizeof(stream_tcp_header_t);
      buf->size    -= sizeof(stream_tcp_header_t);
    }
    buf->type = BUF_DEMUX_BLOCK;
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
 * set_buffer_limits()
 *
 * Set buffer usage limits depending on stream type.
 * - caller must hold this->lock !
 */
static void set_buffer_limits(vdr_input_plugin_t *this)
{
  CHECK_LOCKED(this->lock);

  int capacity = (this->hd_stream ? this->hd_buffer : this->buffer_pool)->buffer_pool_capacity;
  int max_buffers;

  if (this->no_video) {

    /* radio channel / recording. Limit buffers to 10 */
    max_buffers = RADIO_MAX_BUFFERS;

  } else {

    max_buffers = capacity;

    /* replay in local mode --> Limit buffers to 75% */
    if (!this->live_mode && this->fd_control < 0) {
      max_buffers -= (capacity >> 2);
    }

    /* always reserve few buffers for control messages and TS demuxer */
    max_buffers -= 10;
  }

  this->reserved_buffers = capacity - max_buffers;

  /* sanity checks */
  if (capacity < max_buffers) {
    LOGMSG("set_buffer_limits(): internal error: max=%d, capacity=%d", max_buffers, capacity);
    this->reserved_buffers = 10;
  }
  if (this->reserved_buffers < 2) {
    LOGMSG("set_buffer_limits(): internal error: reserved=%d", this->reserved_buffers);
    this->reserved_buffers = 2;
  }
}

/***************************** INTERNAL **********************************/

/*
 * set_still_mode()
 *
 * Set/reset still image mode
 * - caller must hold this->lock !
 */
static void set_still_mode(vdr_input_plugin_t *this, int still_mode)
{
  CHECK_LOCKED(this->lock);

  if (still_mode || this->still_mode)
    CHECK_FALSE(this->live_mode);

  pthread_mutex_lock (&this->stream->first_frame_lock);
  this->stream->first_frame_flag = 2;
  pthread_mutex_unlock (&this->stream->first_frame_lock);

  this->still_mode = !!still_mode;
  _x_stream_info_set(this->stream, XINE_STREAM_INFO_VIDEO_HAS_STILL, this->still_mode);

  if (this->still_mode)
    reset_scr_tuning(this);

  this->stream->metronom->set_option(this->stream->metronom, XVDR_METRONOM_STILL_MODE, still_mode);
}

/*
 * set_live_mode()
 *
 * Set/reset live TV mode
 * - caller must hold this->lock !
 */
static void set_live_mode(vdr_input_plugin_t *this, int onoff)
{
  CHECK_LOCKED(this->lock);

  if (this->live_mode != onoff) {
    config_values_t *config = this->class->xine->config;
    this->live_mode = onoff;

    this->stream->metronom->set_option(this->stream->metronom,
                                       METRONOM_PREBUFFER, METRONOM_PREBUFFER_VAL);

    if (this->live_mode || (this->fd_control >= 0 && !this->slave.stream))
      config->update_num(config, "audio.synchronization.av_sync_method", 1);
#if 0
    /* does not work after playing music files (?) */
    else
      config->update_num(config, "audio.synchronization.av_sync_method", 0);
#endif
  }

  set_buffer_limits(this);

  set_still_mode(this, 0);

  /* SCR tuning */
  if (!this->live_mode) {
    LOGSCR("reset scr tuning by set_live_mode");
    reset_scr_tuning(this);
  }

  signal_buffer_pool_not_empty(this);
}

/*
 * set_trick_speed()
 *
 * Set replay speed
 * - caller must hold this->lock !
 */
static void set_trick_speed(vdr_input_plugin_t *this, int speed, int backwards)
{
/*  speed:
      <0 - show each abs(n)'th frame (drop other frames)
        * no audio
      0 - paused
        * audio back if mute != 0
      >0 - show each frame n times
        * no audio
      1 - normal
*/

  CHECK_LOCKED(this->lock);

  if (speed > 64 || speed < -64)
    return;

  this->is_paused = !!(speed == 0);

  if (!this->is_paused)
    set_still_mode(this, 0);

  if (this->slave.stream)
    backwards = 0;
  this->stream->metronom->set_option(this->stream->metronom, XVDR_METRONOM_TRICK_SPEED, backwards ? speed : 0);

  if (speed > 1 || speed < -1) {
    CHECK_FALSE(this->live_mode);

    reset_scr_tuning(this);

    this->is_trickspeed = 1;
  } else {
    this->is_trickspeed = 0;
  }

  _x_stream_info_set(this->stream, XINE_STREAM_INFO_VIDEO_HAS_STILL, this->still_mode || speed==0);

  if (speed > 0)
    speed = XINE_FINE_SPEED_NORMAL / speed;
  else
    speed = XINE_FINE_SPEED_NORMAL * (-speed);

  if (this->scr_tuning != SCR_TUNING_PAUSED &&
      _x_get_fine_speed(this->stream) != speed) {
    _x_set_fine_speed (this->stream, speed);
  }

  if (this->slave.stream) {
    _x_set_fine_speed (this->slave.stream, speed);
  }
}

static void reset_trick_speed(vdr_input_plugin_t *this)
{
  set_trick_speed(this, 1, 0);
}

/*
 * generated images
 */

static void queue_blank_yv12(vdr_input_plugin_t *this)
{
  if(!this || !this->stream || !this->stream->video_out)
    return;

  vo_frame_t *img    = NULL;
  int         width  = _x_stream_info_get(this->stream, XINE_STREAM_INFO_VIDEO_WIDTH);
  int         height = _x_stream_info_get(this->stream, XINE_STREAM_INFO_VIDEO_HEIGHT);
  int         ratio  = _x_stream_info_get(this->stream, XINE_STREAM_INFO_VIDEO_RATIO);
  double      dratio;

  if (width < 360 || height < 288 || width > 1920 || height > 1200) {
    LOGMSG("queue_blank_yv12: invalid dimensions %dx%d in stream_info !", width, height);
    return;
  }

  if      (ratio > 13300 && ratio < 13400) dratio = 4.0  / 3.0;
  else if (ratio > 17700 && ratio < 17800) dratio = 16.0 / 9.0;
  else if (ratio > 21000 && ratio < 22000) dratio = 2.11 / 1.0;
  else                                     dratio = ((double)ratio) / 10000.0;

  set_still_mode(this, 0);
  reset_scr_tuning(this);
  _x_demux_control_newpts(this->stream, 0, BUF_FLAG_SEEK);


  this->class->xine->port_ticket->acquire (this->class->xine->port_ticket, 1);
  img = this->stream->video_out->get_frame (this->stream->video_out,
                                            width, height, dratio,
                                            XINE_IMGFMT_YV12, VO_BOTH_FIELDS);
  this->class->xine->port_ticket->release (this->class->xine->port_ticket, 1);

  if (img) {
    if (img->format == XINE_IMGFMT_YV12 && img->base[0] && img->base[1] && img->base[2]) {
      memset(img->base[0], 0x00, img->pitches[0] * img->height);
      memset(img->base[1], 0x80, img->pitches[1] * img->height / 2);
      memset(img->base[2], 0x80, img->pitches[2] * img->height / 2);
      img->duration  = 0;
      img->pts       = 0;
      img->bad_frame = 0;

      wait_fifos_empty(this->stream, 100);
      this->stream->metronom->set_option(this->stream->metronom, METRONOM_PREBUFFER, 2000);
      img->draw(img, this->stream);
      this->stream->metronom->set_option(this->stream->metronom, METRONOM_PREBUFFER, METRONOM_PREBUFFER_VAL);
    }
    img->free(img);
  }
}

static void queue_nosignal(vdr_input_plugin_t *this)
{
#define extern static
#include "nosignal_720x576.c"
#undef extern
  char          *data = NULL, *tmp = NULL;
  int            datalen = 0, pos = 0;
  buf_element_t *buf  = NULL;
  fifo_buffer_t *fifo = this->stream->video_fifo;
  char *path, *home;

  if (fifo->num_free(fifo) < 10) {
    LOGMSG("queue_nosignal: not enough free buffers (%d) !",
	   fifo->num_free(fifo));
    return;
  }

  if(asprintf(&home,"%s/.xine/nosignal.mpg", xine_get_homedir()) < 0)
    return;
  int fd = open(path=home, O_RDONLY);
  if(fd<0) fd = open(path="/etc/vdr/plugins/xineliboutput/nosignal.mpg", O_RDONLY);
  if(fd<0) fd = open(path="/etc/vdr/plugins/xine/noSignal.mpg", O_RDONLY);
  if(fd<0) fd = open(path="/video/plugins/xineliboutput/nosignal.mpg", O_RDONLY);
  if(fd<0) fd = open(path="/video/plugins/xine/noSignal.mpg", O_RDONLY);
  if(fd<0) fd = open(path=NOSIGNAL_IMAGE_FILE, O_RDONLY);
  if(fd>=0) {
    tmp = data = malloc(NOSIGNAL_MAX_SIZE);
    datalen = read(fd, data, NOSIGNAL_MAX_SIZE);
    if(datalen==NOSIGNAL_MAX_SIZE) {
	LOGMSG("WARNING: custom \"no signal\" image %s too large", path);
    } else if(datalen<=0) {
      LOGERR("error reading %s", path);
    } else {
      LOGMSG("using custom \"no signal\" image %s", path);
    }
    close(fd);
  }
  free(home);

  if(datalen<=0) {
    data    = (char*)&v_mpg_nosignal[0];
    datalen = v_mpg_nosignal_length;
  }

  /* need to reset decoder if video format is not the same */
  _x_demux_control_start(this->stream);

  while(pos < datalen) {
    buf = fifo->buffer_pool_try_alloc(fifo);
    if(buf) {
      buf->content = buf->mem;
      buf->size = MIN(datalen - pos, buf->max_size);
      buf->type = BUF_VIDEO_MPEG;
      xine_fast_memcpy(buf->content, &data[pos], buf->size);
      pos += buf->size;
      if(pos >= datalen)
        buf->decoder_flags |= BUF_FLAG_FRAME_END;
      fifo->put(fifo, buf);
    } else {
      LOGMSG("Error: queue_nosignal: no buffers !");
      break;
    }
  }

  put_control_buf(fifo, fifo, BUF_CONTROL_FLUSH_DECODER);
  put_control_buf(fifo, fifo, BUF_CONTROL_NOP);

  free(tmp);
}


/*************************** slave input (PIP stream) ********************/

typedef struct fifo_input_plugin_s {
  input_plugin_t      i;
  vdr_input_plugin_t *master;
  xine_stream_t      *stream;
  fifo_buffer_t      *buffer;
  fifo_buffer_t      *buffer_pool;
  off_t               pos;
} fifo_input_plugin_t;

static int   fifo_open(input_plugin_t *this_gen)
{ return 1; }
static uint32_t fifo_get_capabilities (input_plugin_t *this_gen) 
{ return INPUT_CAP_BLOCK; }
static uint32_t fifo_get_blocksize (input_plugin_t *this_gen) 
{ return 2 * 2048; }
static off_t fifo_get_current_pos (input_plugin_t *this_gen)
{ return -1; }
static off_t fifo_get_length (input_plugin_t *this_gen) 
{ return -1; }
static off_t fifo_seek (input_plugin_t *this_gen, off_t offset, int origin) 
{ return offset; }
static int   fifo_get_optional_data (input_plugin_t *this_gen, void *data, int data_type) 
{ return INPUT_OPTIONAL_UNSUPPORTED; }
#if XINE_VERSION_CODE > 10103
static const char* fifo_get_mrl (input_plugin_t *this_gen)
#else
static char* fifo_get_mrl (input_plugin_t *this_gen)
#endif
{ return MRL_ID "+slave:"; }

#if XINE_VERSION_CODE < 10190
static off_t fifo_read (input_plugin_t *this_gen, char *buf, off_t len) 
#else
static off_t fifo_read (input_plugin_t *this_gen, void *buf, off_t len) 
#endif
{
  int got = 0;
  LOGERR("fifo_input_plugin::fifo_read() not implemented !");
  exit(-1); /* assert(false); */
  return got;
}

static buf_element_t *fifo_read_block (input_plugin_t *this_gen,
				       fifo_buffer_t *fifo, off_t todo) 
{
  fifo_input_plugin_t *this = (fifo_input_plugin_t *) this_gen;
  /*LOGDBG("fifo_read_block");*/

  while (!_x_action_pending(this->stream)) {
    buf_element_t *buf = fifo_buffer_try_get(this->buffer);
    if(buf) {
      /* LOGDBG("fifo_read_block: got, return"); */
      return buf;
    }
    /* LOGDBG("fifo_read_block: no buf, poll..."); */
    /* poll(NULL, 0, 10); */
    xine_usec_sleep(5*1000);
    /* LOGDBG("fifo_read_block: poll timeout"); */
  }

  LOGDBG("fifo_read_block: return NULL !");
  errno = EAGAIN;
  return NULL;
}

static void fifo_dispose (input_plugin_t *this_gen) 
{
  fifo_input_plugin_t *this = (fifo_input_plugin_t *) this_gen;
  LOGDBG("fifo_dispose");

  if(this) { 
    if(this->buffer) 
      this->buffer->dispose(this->buffer); 
    free(this); 
  } 
}

static input_plugin_t *fifo_class_get_instance (input_class_t *class_gen,
						xine_stream_t *stream,
						const char *data) 
{
  fifo_input_plugin_t *slave = calloc(1, sizeof(fifo_input_plugin_t));
  unsigned long int imaster;
  vdr_input_plugin_t *master;
  LOGDBG("fifo_class_get_instance");

  sscanf(data+15, "%lx", &imaster);
  master = (vdr_input_plugin_t*)imaster;

  slave->master = (vdr_input_plugin_t*)master;
  slave->stream = stream;
  slave->buffer_pool = stream->video_fifo;
  slave->buffer = fifo_buffer_new(stream, 4, 4096);
  slave->i.open              = fifo_open;
  slave->i.get_mrl           = fifo_get_mrl;
  slave->i.dispose           = fifo_dispose;
  slave->i.input_class       = class_gen;
  slave->i.get_capabilities  = fifo_get_capabilities;
  slave->i.read              = fifo_read;
  slave->i.read_block        = fifo_read_block;
  slave->i.seek              = fifo_seek;
  slave->i.get_current_pos   = fifo_get_current_pos;
  slave->i.get_length        = fifo_get_length;
  slave->i.get_blocksize     = fifo_get_blocksize;
  slave->i.get_optional_data = fifo_get_optional_data;

  return (input_plugin_t*)slave;
}


/******************************** OSD ************************************/

static osd_rle_elem_t *uncompress_osd_net(uint8_t *raw, int elems, int datalen)
{
  osd_rle_elem_t *data = (osd_rle_elem_t *)malloc(elems * sizeof(osd_rle_elem_t));
  int i;

  /*
   * xine-lib rle format: 
   * - palette index and length are uint16_t
   *
   * network format: 
   * - palette entry is uint8_t
   * - length is uint8_t for lengths <=0x7f and uint16_t for lengths >0x7f
   * - high-order bit of first byte is used to signal size of length field:
   *   bit=0 -> 7-bit, bit=1 -> 15-bit
   */

  for(i=0; i<elems; i++) {
    if((*raw) & 0x80) {
      data[i].len = ((*(raw++)) & 0x7f) << 8;
      data[i].len |= *(raw++);
    } else
      data[i].len = *(raw++);
    data[i].color = *(raw++);
  }

  return data;
}

/*
 * vdr_plugin_exec_osd_command()
 *
 * - entry point for VDR-based osd_command_t events
 */
static int vdr_plugin_exec_osd_command(vdr_input_plugin_if_t *this_if, 
				       osd_command_t *cmd)
{
  vdr_input_plugin_t *this = (vdr_input_plugin_t *) this_if;

  if (this->fd_control >= 0 &&  /* remote mode */
      this->funcs.intercept_osd /* frontend handles OSD */ ) {
    return this->funcs.intercept_osd(this->funcs.fe_handle, cmd) ? CONTROL_OK : CONTROL_DISCONNECTED;
  }
  
  return this->osd_manager->command(this->osd_manager, cmd, this->slave.stream ?: this->stream);
}


/******************************* Control *********************************/

#if XINE_VERSION_CODE < 10111
# define DEMUX_MUTEX_LOCK
# define DEMUX_MUTEX_UNLOCK
# define DEMUX_RESUME_SIGNAL
#else
# define DEMUX_MUTEX_LOCK    pthread_mutex_lock(&stream->demux_mutex)
# define DEMUX_MUTEX_UNLOCK  pthread_mutex_unlock(&stream->demux_mutex)
# define DEMUX_RESUME_SIGNAL pthread_cond_signal(&this->stream->demux_resume)
#endif

#if XINE_VERSION_CODE < 10200
# define RAISE_ACTION_PENDING this->stream->demux_action_pending = 1
# define LOWER_ACTION_PENDING this->stream->demux_action_pending = 0
#else
# define RAISE_ACTION_PENDING _x_action_raise(this->stream)
# define LOWER_ACTION_PENDING _x_action_lower(this->stream)
#endif

static void suspend_demuxer(vdr_input_plugin_t *this)
{
  if (this->is_paused)
    LOGMSG("WARNING: called suspend_demuxer in paused mode !");

  /* request demuxer to release demux_lock */
  RAISE_ACTION_PENDING;

  /* signal all possible sync points to speed up this */
  pthread_cond_broadcast(&this->engine_flushed);
  signal_buffer_not_empty(this);

  /* let demuxer return from vdr_plugin_read_* */
  if (pthread_mutex_unlock( &this->lock ))
    LOGERR("pthread_mutex_unlock failed !");

  /* lock demuxer */
  pthread_mutex_lock( &this->stream->demux_lock );

  LOWER_ACTION_PENDING;

  pthread_mutex_lock( &this->lock );

  /* must be paired with resume_demuxer !!! */
}

static void resume_demuxer(vdr_input_plugin_t *this)
{
  /* must be paired with suspend_demuxer !!! */

  DEMUX_RESUME_SIGNAL;
  pthread_mutex_unlock( &this->stream->demux_lock );
}

static void vdr_x_demux_control_newpts( xine_stream_t *stream, int64_t pts, 
					uint32_t flags ) 
{
  buf_element_t *buf;

  DEMUX_MUTEX_LOCK;

  buf = stream->video_fifo ? stream->video_fifo->buffer_pool_try_alloc (stream->video_fifo) : NULL;
  if(buf) {
    buf->type = BUF_CONTROL_NEWPTS;
    buf->decoder_flags = flags;
    buf->disc_off = pts;
    stream->video_fifo->put (stream->video_fifo, buf); 
  } else {
    LOGMSG("vdr_x_demux_control_newpts: video fifo full !");
  }

  buf = stream->audio_fifo ? stream->audio_fifo->buffer_pool_try_alloc (stream->audio_fifo) : NULL;
  if (buf) {
    buf->type = BUF_CONTROL_NEWPTS;
    buf->decoder_flags = flags;
    buf->disc_off = pts;
    stream->audio_fifo->put (stream->audio_fifo, buf);
  } else {
    LOGMSG("vdr_x_demux_control_newpts: audio fifo full !");
  }

  DEMUX_MUTEX_UNLOCK;
}

static void vdr_flush_engine(vdr_input_plugin_t *this, uint64_t discard_index)
{
  CHECK_LOCKED(this->lock);

  if(this->stream_start) {
    LOGMSG("vdr_flush_engine: stream_start, flush skipped");
    return;
  }

  if(this->curpos > discard_index) {
    if(this->curpos < this->guard_index) {
      LOGMSG("vdr_flush_engine: guard > curpos, flush skipped");
      return;
    }
    LOGMSG("vdr_flush_engine: %"PRIu64" < current position %"PRIu64", flush skipped", 
	   discard_index, this->curpos);
    return;
  }

  /* reset speed */
  if(xine_get_param(this->stream, XINE_PARAM_FINE_SPEED) <= 0) {
    LOGMSG("vdr_flush_engine: playback is paused <0>");
    xine_set_param(this->stream, XINE_PARAM_FINE_SPEED, XINE_FINE_SPEED_NORMAL);
  }

  /* suspend demuxer */
  suspend_demuxer(this);

  reset_scr_tuning(this);

  /* reset speed again (adjust_realtime_speed might have set pause) */
  if(xine_get_param(this->stream, XINE_PARAM_FINE_SPEED) <= 0) {
    LOGMSG("vdr_flush_engine: playback is paused <1>");
    xine_set_param(this->stream, XINE_PARAM_FINE_SPEED, XINE_FINE_SPEED_NORMAL);
  }

#if 0
  _x_demux_flush_engine (this->stream);
  /* warning: after clearing decoders fifos an absolute discontinuity
   *          indication must be sent. relative discontinuities are likely
   *          to cause "jumps" on metronom.
   */
#else
  this->stream->demux_plugin->seek (this->stream->demux_plugin,
				    0, 0, this->stream->demux_thread_running);
#endif

#if XINE_VERSION_CODE < 10104
  /* disabled _x_demux_control_start as it causes alsa output driver to exit now and then ... */
#else
  _x_demux_control_start(this->stream);
#endif
  this->stream_start = 1;
  this->I_frames = this->B_frames = this->P_frames = 0;
  this->discard_index = discard_index;

  resume_demuxer(this);
}

static int set_deinterlace_method(vdr_input_plugin_t *this, const char *method_name)
{
  int method = 0;
  if(!strncasecmp(method_name,"bob",3)) {                 method = 1;
  } else if(!strncasecmp(method_name,"weave",5)) {        method = 2;
  } else if(!strncasecmp(method_name,"greedy",6)) {       method = 3;
  } else if(!strncasecmp(method_name,"onefield",8)) {     method = 4;
  } else if(!strncasecmp(method_name,"onefield_xv",11)) { method = 5;
  } else if(!strncasecmp(method_name,"linearblend",11)) { method = 6;
  } else if(!strncasecmp(method_name,"none",4)) {         method = 0;
  } else if(!*method_name) {                              method = 0;
  } else if(!strncasecmp(method_name,"tvtime",6)) {       method = -1;
  /* old deinterlacing system must be switched off.
     tvtime will be configured as all other post plugins with 
     "POST tvtime ..." control message  */
  } else return -2;
  
  this->class->xine->config->update_num(this->class->xine->config,
					"video.output.xv_deinterlace_method",
					method >= 0 ? method : 0);
  xine_set_param(this->stream, XINE_PARAM_VO_DEINTERLACE, !!method);
  
  return 0;
}

static int set_video_properties(vdr_input_plugin_t *this, 
				int hue, int saturation, 
				int brightness, int sharpness,
				int noise_reduction, int contrast,
                                int vo_aspect_ratio)
{
  pthread_mutex_lock(&this->lock);

  /* when changed first time, save original/default values */
  if(!this->video_properties_saved && 
     (hue>=0 || saturation>=0 || contrast>=0 || brightness>=0 || 
      sharpness>=0 || noise_reduction>=0 || vo_aspect_ratio>=0)) {
    this->video_properties_saved = 1;
    this->orig_hue        = xine_get_param(this->stream, 
					   XINE_PARAM_VO_HUE );
    this->orig_saturation = xine_get_param(this->stream, 
					   XINE_PARAM_VO_SATURATION );
    this->orig_brightness = xine_get_param(this->stream, 
					   XINE_PARAM_VO_BRIGHTNESS );
#ifdef XINE_PARAM_VO_SHARPNESS
    this->orig_sharpness   = xine_get_param(this->stream, 
					   XINE_PARAM_VO_SHARPNESS );
#endif
#ifdef XINE_PARAM_VO_NOISE_REDUCTION
    this->orig_noise_reduction = xine_get_param(this->stream, 
					   XINE_PARAM_VO_NOISE_REDUCTION );
#endif
    this->orig_contrast   = xine_get_param(this->stream, 
					   XINE_PARAM_VO_CONTRAST );
    this->orig_vo_aspect_ratio   = xine_get_param(this->stream,
                                           XINE_PARAM_VO_ASPECT_RATIO );
  }

  /* set new values, or restore default/original values */
  if(hue>=0 || this->video_properties_saved)
    xine_set_param(this->stream, XINE_PARAM_VO_HUE, 
		   hue>=0 ? hue : this->orig_hue );
  if(saturation>=0 || this->video_properties_saved)
    xine_set_param(this->stream, XINE_PARAM_VO_SATURATION, 
		   saturation>0 ? saturation : this->orig_saturation );
  if(brightness>=0 || this->video_properties_saved)
    xine_set_param(this->stream, XINE_PARAM_VO_BRIGHTNESS, 
		   brightness>=0 ? brightness : this->orig_brightness );
#ifdef XINE_PARAM_VO_SHARPNESS
  if(sharpness>=0 || this->video_properties_saved)
    xine_set_param(this->stream, XINE_PARAM_VO_SHARPNESS, 
		   sharpness>=0 ? sharpness : this->orig_sharpness );
#endif
#ifdef XINE_PARAM_VO_NOISE_REDUCTION
  if(noise_reduction>=0 || this->video_properties_saved)
    xine_set_param(this->stream, XINE_PARAM_VO_NOISE_REDUCTION, 
		   noise_reduction>=0 ? noise_reduction : this->orig_noise_reduction );
#endif
  if(contrast>=0 || this->video_properties_saved)
    xine_set_param(this->stream, XINE_PARAM_VO_CONTRAST, 
		   contrast>=0 ? contrast : this->orig_contrast );
  if(vo_aspect_ratio>=0 || this->video_properties_saved)
    xine_set_param(this->stream, XINE_PARAM_VO_ASPECT_RATIO,
                   vo_aspect_ratio>=0 ? vo_aspect_ratio : this->orig_vo_aspect_ratio );

  if(hue<0 && saturation<0 && contrast<0 && brightness<0 && sharpness<0 && noise_reduction<0 && vo_aspect_ratio<0)
    this->video_properties_saved = 0;

  pthread_mutex_unlock(&this->lock);
  return 0;
}

/*
 * slave stream
 */

static void send_meta_info(vdr_input_plugin_t *this)
{
  if (this->slave.stream) {

    /* send stream meta info */
    const char *title       = xine_get_meta_info(this->slave.stream, XINE_META_INFO_TITLE);
    const char *artist      = xine_get_meta_info(this->slave.stream, XINE_META_INFO_ARTIST);
    const char *album       = xine_get_meta_info(this->slave.stream, XINE_META_INFO_ALBUM);
    const char *tracknumber = xine_get_meta_info(this->slave.stream, XINE_META_INFO_TRACK_NUMBER);

    printf_vdr(this, "INFO METAINFO title=@%s@ artist=@%s@ album=@%s@ tracknumber=@%s@\r\n",
               title?:"", artist?:"", album?:"", tracknumber?:"");
  }
}

#ifdef DVD_STREAMING_SPEED
static void dvd_set_speed(const char *device, int speed)
{
  /*
   * Original idea & code from mplayer-dev-eng mailing list:
   * Date: Sun, 17 Dec 2006 09:15:30 +0100
   * From: Tobias Diedrich <ranma@xxxxxxxxxxxx>
   * Subject: [MPlayer-dev-eng] Re: [PATCH] Add "-dvd-speed", use SET_STREAMING command to quieten DVD drives
   * (http://lists-archives.org/mplayer-dev-eng/14383-add-dvd-speed-use-set_streaming-command-to-quieten-dvd-drives.html)
   */
#if defined(__linux__) && defined(SG_IO) && defined(GPCMD_SET_STREAMING)
  unsigned char buffer[28], cmd[16], sense[16];
  struct sg_io_hdr sghdr;
  struct stat st;
  int fd;

  /* remember previous device so we can restore default speed */
  static int dvd_speed = 0;
  static const char *dvd_dev = NULL;
  if (speed < 0 && dvd_speed == 0) return; /* we haven't touched the speed setting */
  if (!device) device = dvd_dev; /* use previous device */
  if (!device) return;
  if (!speed) return;

  if (stat(device, &st) == -1) return;

  if (!S_ISBLK(st.st_mode)) return; /* not a block device */
  
  if ((fd = open(device, O_RDWR | O_NONBLOCK)) == -1) {
    LOGMSG("set_dvd_speed: error opening DVD device %s for read/write", device);
    return;
  }

  memset(&sghdr, 0, sizeof(sghdr));
  memset(buffer, 0, sizeof(buffer));
  memset(sense, 0, sizeof(sense));
  memset(cmd, 0, sizeof(cmd));

  if(speed < 0) {
    /* restore default value */
    speed = 0;
    buffer[0] = 4; /* restore default */
    LOGMSG("Setting DVD streaming speed to <default>");
  } else {
    /* limit to <speed> KB/s */
    LOGMSG("Setting DVD streaming speed to %d", speed);
  }

  sghdr.interface_id = 'S';
  sghdr.timeout      = 5000;
  sghdr.dxfer_direction = SG_DXFER_TO_DEV;
  sghdr.mx_sb_len = sizeof(sense);
  sghdr.dxfer_len = sizeof(buffer);
  sghdr.cmd_len   = sizeof(cmd);
  sghdr.sbp       = sense;
  sghdr.dxferp    = buffer;
  sghdr.cmdp      = cmd;

  cmd[0]     = GPCMD_SET_STREAMING;
  cmd[10]    = 28;

  buffer[8]  = 0xff;
  buffer[9]  = 0xff;
  buffer[10] = 0xff;
  buffer[11] = 0xff;

  buffer[12] = buffer[20] = (speed >> 24) & 0xff; /* <speed> kilobyte */
  buffer[13] = buffer[21] = (speed >> 16) & 0xff;
  buffer[14] = buffer[22] = (speed >> 8)  & 0xff;
  buffer[15] = buffer[23] = speed & 0xff;

  buffer[18] = buffer[26] = 0x03; /* 1 second */
  buffer[19] = buffer[27] = 0xe8;

  if (ioctl(fd, SG_IO, &sghdr) < 0)
    LOGERR("Failed setting DVD streaming speed to %d", speed);
  else if(speed > 0)
    LOGMSG("DVD streaming speed set to %d", speed);
  else
    LOGMSG("DVD streaming speed set to <default>");

  dvd_speed = speed;
  dvd_dev = device;
  close(fd);
#else
# warning Changing DVD streaming speed not supported
#endif
}
#endif

static void vdr_event_cb (void *user_data, const xine_event_t *event);

static void select_spu_channel(xine_stream_t *stream, int channel)
{
  _x_select_spu_channel(stream, channel);
  if (channel == SPU_CHANNEL_NONE) {
    /* re-enable overlay for VDR OSD ... */
    if (stream->video_out) {
      pthread_mutex_lock (&stream->frontend_lock);
      stream->xine->port_ticket->acquire (stream->xine->port_ticket, 0);
    
      stream->video_out->enable_ovl (stream->video_out, 1);

      stream->xine->port_ticket->release (stream->xine->port_ticket, 0);
      pthread_mutex_unlock (&stream->frontend_lock);
    }
  }
}

static void dvd_menu_domain(vdr_input_plugin_t *this, int value)
{
  if (value) {
    LOGDBG("dvd_menu_domain(1)");
    this->dvd_menu = 1;
    this->slave.stream->spu_channel_user = SPU_CHANNEL_AUTO;
    this->slave.stream->spu_channel = this->slave.stream->spu_channel_auto;
  } else {
    LOGDBG("dvd_menu_domain(0)");
    this->dvd_menu = 0;
  }
}

static void close_slave_stream(vdr_input_plugin_t *this)
{
  if (!this->slave.stream)
    return;

  if(this->bg_stream.stream) {
    LOGMSG("Closing background stream");
    xine_stop(this->bg_stream.stream);

    if (this->bg_stream.event_queue) {
      xine_event_dispose_queue (this->bg_stream.event_queue);
      this->bg_stream.event_queue = NULL;
    }
    xine_close(this->bg_stream.stream);
    xine_dispose(this->bg_stream.stream);
    this->bg_stream.stream = NULL;
  }

  /* dispose event queue first to prevent processing of PLAYBACK FINISHED event */
  if (this->slave.event_queue) {
    xine_event_dispose_queue (this->slave.event_queue);
    this->slave.event_queue = NULL;
  }

  xine_stop(this->slave.stream);

  if(this->funcs.fe_control) {
    this->funcs.fe_control(this->funcs.fe_handle, "POST 0 Off\r\n");
    this->funcs.fe_control(this->funcs.fe_handle, "SLAVE 0x0\r\n");
  }
  xine_close(this->slave.stream);
  xine_dispose(this->slave.stream);

  pthread_mutex_lock(&this->lock);
  this->slave.stream = NULL;
  pthread_mutex_unlock(&this->lock);

  if(this->funcs.fe_control)
    this->funcs.fe_control(this->funcs.fe_handle, "SLAVE CLOSED\r\n");
}

static int handle_control_playfile(vdr_input_plugin_t *this, const char *cmd)
{
  const char *pt = cmd + 9;
  char filename[4096], av[256+4096], *pav = av;
  int loop = 0, pos = 0, err = 0, avsize = sizeof(av)-2, mix_streams = 0;

  while(*pt==' ') pt++;

  if(!strncmp(pt, "Loop ", 5)) {
    loop = 1;
    pt += 5;
    while(*pt==' ') pt++;
  }

  pos = atoi(pt);

  while(*pt && *pt != ' ') pt++;
  while(*pt == ' ') pt++;

  /* audio visualization / audio/video mixing */
  while(*pt && *pt != ' ' && --avsize)
    *pav++ = *pt++;
  *pav = 0;
  while(*pt == ' ') pt++;
  mix_streams = (!strcmp(av, "Audio")) || (!strcmp(av, "Video"));

  strn0cpy(filename, pt, sizeof(filename));

  this->autoplay_size = -1;

  if(*filename) {
    int is_file_mrl = !strncmp(filename, "file:/", 6) ? 5 : 0;
    this->loop_play = 0;

    /* mrlbase is needed for filename and for bgimage in remote mode */
    char mrlbase[256];
    if(this->fd_control >= 0) {
      char *host = strdup(strstr(this->mrl, "//")+2);
      char *port = strchr(host, ':');
      int  iport = port ? atoi(port+1) : DEFAULT_VDR_PORT;
      if(port) *port = 0;
      snprintf(mrlbase, sizeof(mrlbase), "http://%s:%d/PLAYFILE",
               host?:"127.0.0.1", iport);
      free(host);
    }

    if (this->slave.stream)
      handle_control_playfile(this, "PLAYFILE 0");

    LOGMSG("PLAYFILE  (Loop: %d, Offset: %ds, File: %s %s)",
	   loop, pos, av, filename);
    
    /* check if it is really a file (not mrl) and try to access it */
    if(is_file_mrl || filename[0] == '/') {
      struct stat st;
      char *f = unescape_filename(filename);
      errno = 0;
      if(stat(f+is_file_mrl, &st)) {
	if(errno == EACCES || errno == ELOOP)
	  LOGERR("Can't access file !");
	if(errno == ENOENT || errno == ENOTDIR) 
	  LOGERR("File not found !");
	if(this->fd_control >= 0) {
	  char mrl[sizeof(filename)+256];
	  char *sub  = strstr(filename, "#subtitle:");
	  if(sub) *sub = 0;
	  sprintf(mrl, "%s%s", mrlbase, filename + is_file_mrl);
	  if(sub) {
	    sub += 10; /*strlen("#subtitle:");*/
	    strcat(mrl, "#subtitle:");
	    strcat(mrl, mrlbase);
	    strcat(mrl, sub);
	  }
	  LOGMSG("  -> trying to stream from server (%s) ...", mrl);
	  strn0cpy(filename, mrl, sizeof(filename));
	}
      }
      free(f);
    }

    if(!strcmp(filename,"dvd:/")) {
#if 0
	/* input/media_helper.c */
	eject_media(0);	/* DVD tray in */
#endif
#ifdef DVD_STREAMING_SPEED
	  xine_cfg_entry_t device;
	  if (xine_config_lookup_entry(this->class->xine,
				       "media.dvd.device", &device))
	    dvd_set_speed(device.str_value, 2700);
#endif
    }
#if XINE_VERSION_CODE < 10109
    else if(!strncmp(filename,"dvd:/", 5)) {
      /* DVD plugin 'bug': unescaping is not implemented ... */
      char *mrl = unescape_filename(filename);
      strn0cpy(filename, mrl, sizeof(filename));
      free(mrl);
    }
#endif

    if (!this->slave.stream) {
      cfg_entry_t *e = this->class->xine->config->lookup_entry(this->class->xine->config,
                                                               "engine.buffers.video_num_buffers");
      int vbufs = e ? e->num_value : 250;
      this->class->xine->config->update_num(this->class->xine->config,
					    "engine.buffers.video_num_buffers", SLAVE_VIDEO_FIFO_SIZE);
      LOGMSG("xine_stream_new(slave_stream): using %dMB video fifo", SLAVE_VIDEO_FIFO_SIZE*8/1024);
      this->slave.stream = xine_stream_new(this->class->xine,
                                           this->stream->audio_out,
                                           this->stream->video_out);

      this->class->xine->config->update_num(this->class->xine->config,
					    "engine.buffers.video_num_buffers", vbufs);
    }

    if (!this->slave.event_queue) {
      this->slave.event_queue = xine_event_new_queue (this->slave.stream);
      xine_event_create_listener_thread (this->slave.event_queue,
					 vdr_event_cb, this);
    }
    select_spu_channel(this->slave.stream, SPU_CHANNEL_AUTO);
    this->dvd_menu = 0;

    errno = 0;
    err = !xine_open(this->slave.stream, filename);
    if(err) {
      LOGERR("Error opening file ! (File not found ? Unknown format ?)");
      *filename = 0; /* this triggers stop */
    } else {
#if 1
      if(this->stream->video_fifo->size(this->stream->video_fifo))
	LOGMSG("playfile: main stream video_fifo not empty ! (%d)",
	       this->stream->video_fifo->size(this->stream->video_fifo));
      
      /* flush decoders and output fifos, close decoders and free frames. */
      _x_demux_control_start(this->stream);
      xine_usec_sleep(50*1000);

      /* keep our own demux happy while playing another stream */
      pthread_mutex_lock(&this->lock);

      reset_trick_speed(this);
      this->live_mode = 1;
      set_live_mode(this, 0);
      reset_trick_speed(this);
      reset_scr_tuning(this);

      pthread_mutex_unlock(&this->lock);

      this->slave.stream->metronom->set_option(this->slave.stream->metronom,
                                                      METRONOM_PREBUFFER, 90000);
#endif

      this->loop_play = loop;
      err = !xine_play(this->slave.stream, 0, 1000 * pos);
      if(err) {
	LOGMSG("Error playing file");
	*filename = 0; /* this triggers stop */
      } else {
	send_meta_info(this);

	if(this->funcs.fe_control) {
	  char tmp[128];
	  int has_video;
          sprintf(tmp, "SLAVE 0x%lx %s\r\n",
                  (unsigned long int)this->slave.stream,
                  mix_streams ? av : "");
	  this->funcs.fe_control(this->funcs.fe_handle, tmp);
          has_video = _x_stream_info_get(this->slave.stream, XINE_STREAM_INFO_HAS_VIDEO);

          /* Play background image */
          if(!has_video && !mix_streams && *av && !strncmp(av, "image:", 6)) {

            const char *bgimage = av + 6;

            /* background image stream init */
            if (!this->bg_stream.stream) {
              LOGDBG("handle_control_playfile: Background stream init");
              this->bg_stream.stream = xine_stream_new(this->class->xine, NULL, this->slave.stream->video_out);
              xine_set_param(this->bg_stream.stream, XINE_PARAM_IGNORE_AUDIO, 1);
              xine_set_param(this->bg_stream.stream, XINE_PARAM_AUDIO_CHANNEL_LOGICAL, -2);
              xine_set_param(this->bg_stream.stream, XINE_PARAM_SPU_CHANNEL, -2);
              xine_set_param(this->bg_stream.stream, XINE_PARAM_AUDIO_REPORT_LEVEL, 0);
            }
            if (!this->bg_stream.event_queue) {
              LOGDBG("handle_control_playfile: Background event queue init");
              this->bg_stream.event_queue = xine_event_new_queue(this->bg_stream.stream);
              xine_event_create_listener_thread(this->bg_stream.event_queue, vdr_event_cb, this);
            }

            /* open background image */
            if (!xine_open(this->bg_stream.stream, bgimage) || !xine_play(this->bg_stream.stream, 0, 0)) {
              LOGMSG("Error opening background image %s (File not found ? Unknown format ?)", bgimage);

              int is_bg_file_mrl = !strncmp(bgimage, "file:/", 6) ? 5 : 0;
              if (this->fd_control >= 0 && (bgimage[0] == '/' || is_bg_file_mrl)) {
                /* Remote mode */
                char bgmrl[4096+256];
                snprintf(bgmrl, sizeof(bgmrl), "%s%s", mrlbase, bgimage + is_bg_file_mrl);
                bgmrl[sizeof(bgmrl)-1] = 0;
                LOGMSG("  -> trying to stream background image from server (%s) ...", bgmrl);
                if (!xine_open(this->bg_stream.stream, bgmrl) || !xine_play(this->bg_stream.stream, 0, 0))
                  LOGMSG("Error streaming background image from server!");
              }
            }
 
             has_video = 1;
          }

	  this->funcs.fe_control(this->funcs.fe_handle, 
				 has_video ? "NOVIDEO 1\r\n" : "NOVIDEO 0\r\n");
	  if(!has_video && !mix_streams && *av && strcmp(av, "none")) {
	    char str[128], *avopts;
	    if(NULL != (avopts = strchr(av, ':')))
	      *avopts++ = 0;
	    else
	      avopts = "";
	    snprintf(str, sizeof(str), "POST %s On %s\r\n", av, avopts);
	    str[sizeof(str)-1] = 0;
	    this->funcs.fe_control(this->funcs.fe_handle, str);
	  } else {
	    this->funcs.fe_control(this->funcs.fe_handle, "POST 0 Off\r\n");
	  }
	}
      }
    }
  }

  /* next code is also executed after failed open, so no "} else { " */
  if(!*filename) {
    LOGMSG("PLAYFILE <STOP>: Closing slave stream");
    this->loop_play = 0;
    if (this->slave.stream) {
      close_slave_stream(this);

      _x_demux_control_start(this->stream);

#ifdef DVD_STREAMING_SPEED
      dvd_set_speed(NULL, -1);
#endif
    }
  }

  return err ? CONTROL_PARAM_ERROR : CONTROL_OK;
}

/*
 * grab
 */

static int handle_control_grab(vdr_input_plugin_t *this, const char *cmd)
{
  int quality, width, height, jpeg;
  jpeg = !strcmp(cmd+5,"JPEG");

  if(3 == sscanf(cmd+5+4, "%d %d %d", &quality, &width, &height)) {
 
    if(this->fd_control >= 0) {

      grab_data_t *data = NULL; 
      LOGDBG("GRAB: jpeg=%d quality=%d w=%d h=%d", jpeg, quality, width, height);  

      /* grab takes long time and we don't want to lose data connection 
	 or interrupt video ... */
      if(pthread_mutex_unlock(&this->vdr_entry_lock))
	LOGERR("pthread_mutex_unlock failed");

      if(this->funcs.fe_control)
	data = (grab_data_t*)(this->funcs.fe_control(this->funcs.fe_handle, cmd));
      
      if(data && data->size>0 && data->data) {
	char s[128];
	sprintf(s, "GRAB %d %lu\r\n", this->token, (unsigned long)data->size);
	mutex_lock_cancellable (&this->fd_control_lock);
	write_control_data(this, s, strlen(s));
	write_control_data(this, data->data, data->size);
	mutex_unlock_cancellable (&this->fd_control_lock);
      } else {
	/* failed */
	printf_control(this, "GRAB %d 0\r\n", this->token);
      }

      pthread_mutex_lock(&this->vdr_entry_lock);

      if(data) {
	free(data->data);
	free(data);
      }

      return CONTROL_OK;
    }
  }

  return CONTROL_PARAM_ERROR;
}

/*
 * PIP
 */

static int handle_control_substream(vdr_input_plugin_t *this, const char *cmd)
{
  unsigned int pid;
  if(1 == sscanf(cmd, "SUBSTREAM 0x%x", &pid)) {
    pthread_mutex_lock(&this->lock);
    
    if(!this->funcs.fe_control)
      LOGERR("ERROR - no fe_control set !");
    
    if((pid & 0xf0) == 0xe0 && this->funcs.fe_control) { /* video 0...15 */
      if(!this->pip_stream) {
LOGMSG("create pip stream %s", cmd);
        this->pip_stream = 
	  this->funcs.fe_control(this->funcs.fe_handle, cmd);
LOGMSG("  pip stream created");
      }
    } else {
  /*} else if(audio) {*/
      if(this->pip_stream && this->funcs.fe_control) {
	LOGMSG("close pip stream");
 
	this->pip_stream = NULL;
	this->funcs.fe_control(this->funcs.fe_handle, cmd);
	/* xine_stop(this->pip_stream); */
	/* xine_close(this->pip_stream); */
	/* xine_dispose(this->pip_stream); */
      }
    }
    pthread_mutex_unlock(&this->lock);
    return CONTROL_OK;
  } 
  return CONTROL_PARAM_ERROR;
}

/*
 * OSD
 */

static int handle_control_osdcmd(vdr_input_plugin_t *this)
{
  osd_command_t osdcmd = {0};
  int err = CONTROL_OK;

  if (!this->control_running)
    return CONTROL_DISCONNECTED;

  /* read struct size first */
  size_t   todo, expect = sizeof(osd_command_t);
  uint8_t *pt = (uint8_t*)&osdcmd;
  if (read_control(this, pt, sizeof(osdcmd.size)) != sizeof(osdcmd.size)) {
    LOGMSG("control: error reading OSDCMD data length");
    return CONTROL_DISCONNECTED;
  }
  pt     += sizeof(osdcmd.size);
  expect -= sizeof(osdcmd.size);
  todo    = osdcmd.size - sizeof(osdcmd.size);

  /* read data */
  ssize_t bytes = MIN(todo, expect);
  if (read_control(this, pt, bytes) != bytes) {
    LOGMSG("control: error reading OSDCMD data");
    return CONTROL_DISCONNECTED;
  }

  if (expect < todo) {
    /* server uses larger struct, discard rest of data */
    ssize_t skip = todo - expect;
    uint8_t dummy[skip];
    LOGMSG("osd_command_t size %d, expected %zu", osdcmd.size, expect);
    if (read_control(this, dummy, skip) != skip) {
      LOGMSG("control: error reading OSDCMD data (unknown part)");
      return CONTROL_DISCONNECTED;
    }
  }

  ntoh_osdcmd(osdcmd);

  /* read palette */
  if (osdcmd.palette && osdcmd.colors>0) {
    ssize_t bytes = sizeof(osd_clut_t) * osdcmd.colors;
    osdcmd.palette = malloc(bytes);
    if (read_control(this, (unsigned char *)osdcmd.palette, bytes) != bytes) {
      LOGMSG("control: error reading OSDCMD palette");
      err = CONTROL_DISCONNECTED;
    }
  } else {
    osdcmd.palette = NULL;
  }

  /* read (RLE) data */
  if (err == CONTROL_OK && osdcmd.data && osdcmd.datalen>0) {
    osdcmd.data = (osd_rle_elem_t *)malloc(osdcmd.datalen);
    if(read_control(this, (unsigned char *)osdcmd.data, osdcmd.datalen)
       != (ssize_t)osdcmd.datalen) {
      LOGMSG("control: error reading OSDCMD bitmap");
      err = CONTROL_DISCONNECTED;
    } else {
      if (osdcmd.cmd == OSD_Set_HDMV) {
        uint8_t *raw = osdcmd.raw_data;
        int n = rle_uncompress_hdmv(&osdcmd.data,
                                    osdcmd.w, osdcmd.h,
                                    raw, osdcmd.num_rle, osdcmd.datalen);
        if (n < 1) {
          LOGMSG("HDMV mode OSD uncompress error");
          osdcmd.raw_data = raw;
        } else {
          osdcmd.cmd     = OSD_Set_RLE;
          osdcmd.datalen = osdcmd.num_rle*4;
          free(raw);
        }
      } else if (osdcmd.cmd == OSD_Set_RLE) {
        uint8_t *raw = osdcmd.raw_data;
        osdcmd.data    = uncompress_osd_net(raw, osdcmd.num_rle, osdcmd.datalen);
        osdcmd.datalen = osdcmd.num_rle*4;
        free(raw);
      }
    }
  } else {
    osdcmd.data = NULL;
  }

  if (err == CONTROL_OK)
    err = vdr_plugin_exec_osd_command(&this->iface, &osdcmd);

  free(osdcmd.data);
  free(osdcmd.palette);

  return err;
}

/************************** Control from VDR ******************************/

#define VDR_ENTRY_LOCK(ret...) \
  do { \
    if(pthread_mutex_lock(&this->vdr_entry_lock)) { \
      LOGERR("%s:%d: pthread_mutex_lock failed", __PRETTY_FUNCTION__, __LINE__); \
      return ret ; \
    } \
  } while(0)

#define VDR_ENTRY_UNLOCK() \
  do { \
    if(pthread_mutex_unlock(&this->vdr_entry_lock)) { \
      LOGERR("%s:%d: pthread_mutex_unlock failed", __PRETTY_FUNCTION__, __LINE__); \
    } \
  } while(0)

static const struct { 
  const uint32_t type;
  const char     name[28]; 
} eventmap[] = {
  {XINE_EVENT_INPUT_UP,      "XINE_EVENT_INPUT_UP"},
  {XINE_EVENT_INPUT_DOWN,    "XINE_EVENT_INPUT_DOWN"},
  {XINE_EVENT_INPUT_LEFT,    "XINE_EVENT_INPUT_LEFT"},
  {XINE_EVENT_INPUT_RIGHT,   "XINE_EVENT_INPUT_RIGHT"},
  {XINE_EVENT_INPUT_SELECT,  "XINE_EVENT_INPUT_SELECT"},
  {XINE_EVENT_INPUT_MENU1,   "XINE_EVENT_INPUT_MENU1"},
  {XINE_EVENT_INPUT_MENU2,   "XINE_EVENT_INPUT_MENU2"},
  {XINE_EVENT_INPUT_MENU3,   "XINE_EVENT_INPUT_MENU3"},
  {XINE_EVENT_INPUT_MENU4,   "XINE_EVENT_INPUT_MENU4"},
  {XINE_EVENT_INPUT_MENU5,   "XINE_EVENT_INPUT_MENU5"},
  {XINE_EVENT_INPUT_NEXT,    "XINE_EVENT_INPUT_NEXT"},
  {XINE_EVENT_INPUT_PREVIOUS,"XINE_EVENT_INPUT_PREVIOUS"},
};

/*
 * vdr_plugin_poll()
 *
 * Query buffer state
 * Returns amount of free PES buffer blocks in queue.
 */
static int vdr_plugin_poll(vdr_input_plugin_t *this, int timeout_ms)
{
  struct timespec abstime;
  fifo_buffer_t  *fifo          = this->hd_stream ? this->hd_buffer : this->buffer_pool;
  int             reserved_bufs = this->reserved_buffers;
  int             result        = 0;

  /* Caller must have locked this->vdr_entry_lock ! */

  if (this->slave.stream) {
    LOGMSG("vdr_plugin_poll: called while playing slave stream !");
    return 1;
  }

  TRACE("vdr_plugin_poll (%d ms), fifo: blocks=%d, bytes=%d",
	timeout_ms, fifo->size(fifo), fifo->data_size(fifo));

  pthread_mutex_lock (&fifo->buffer_pool_mutex);
  result = fifo->buffer_pool_num_free - reserved_bufs;
  pthread_mutex_unlock (&fifo->buffer_pool_mutex);

  if (timeout_ms > 0 && result <= 0) {
    if (timeout_ms > 250) {
      LOGMSG("vdr_plugin_poll: timeout too large (%d ms), forced to 250ms", timeout_ms);
      timeout_ms = 250;
    }
    create_timeout_time(&abstime, timeout_ms);
    pthread_mutex_lock(&this->lock);
    if (this->scr_tuning == SCR_TUNING_PAUSED) {
      LOGSCR("scr tuning reset by POLL");
      reset_scr_tuning(this);
    }
    pthread_mutex_unlock(&this->lock);

    signal_buffer_not_empty(this);

    VDR_ENTRY_UNLOCK();

    pthread_mutex_lock (&fifo->buffer_pool_mutex);
    while (result <= 5) {
      if (pthread_cond_timedwait (&fifo->buffer_pool_cond_not_empty,
                                  &fifo->buffer_pool_mutex,
                                  &abstime) == ETIMEDOUT)
	break;
      result = fifo->buffer_pool_num_free - reserved_bufs;
    }
    pthread_mutex_unlock (&fifo->buffer_pool_mutex);
    VDR_ENTRY_LOCK(0);
  }

  TRACE("vdr_plugin_poll returns, %d free (%d used, %d bytes)\n",
        result, fifo->size(fifo), fifo->data_size(fifo));

 /* handle priority problem in paused mode when
    data source has higher priority than control source */
  if (result <= 0) {
    result = 0;
    xine_usec_sleep(3*1000);
  }

  return result;
}

/*
 * vdr_plugin_flush()
 *
 * Flush all data from buffers to output devices.
 * Returns 0 when there is no data or frames in stream buffers.
 */
static int vdr_plugin_flush(vdr_input_plugin_t *this, int timeout_ms)
{
  struct timespec abstime;
  fifo_buffer_t *pool   = this->buffer_pool;
  fifo_buffer_t *buffer = this->block_buffer;
  int result = 0, waitresult=0;

  /* Caller must have locked this->vdr_entry_lock ! */

  if (this->slave.stream) {
    LOGDBG("vdr_plugin_flush: called while playing slave stream !");
    return 0;
  }

  TRACE("vdr_plugin_flush (%d ms) blocks=%d+%d, frames=%d", timeout_ms,
	buffer->size(buffer), pool->size(pool),
	this->stream->video_out->get_property(this->stream->video_out, 
					      VO_PROP_BUFS_IN_FIFO));

  if(this->live_mode /*&& this->fd_control < 0*/) {
    /* No flush in live mode */
    return 1; 
  }

  this->class->xine->port_ticket->acquire(this->class->xine->port_ticket, 1);
  result = MAX(0, pool->size(pool)) + 
           MAX(0, buffer->size(buffer)) +
           this->stream->video_out->get_property(this->stream->video_out, 
						 VO_PROP_BUFS_IN_FIFO);
  this->class->xine->port_ticket->release(this->class->xine->port_ticket, 1);

  put_control_buf(buffer, pool, BUF_CONTROL_FLUSH_DECODER);
  put_control_buf(buffer, pool, BUF_CONTROL_NOP);

  if (result <= 0)
    return 0;

  create_timeout_time(&abstime, timeout_ms);

  while(result > 0 && waitresult != ETIMEDOUT) {
    TRACE("vdr_plugin_flush waiting (max %d ms), %d+%d buffers used, "
	  "%d frames (rd pos=%" PRIu64 ")\n", timeout_ms,
	  pool->size(pool), buffer->size(buffer),
	  (int)this->stream->video_out->get_property(this->stream->video_out, 
						     VO_PROP_BUFS_IN_FIFO),
	  this->curpos);

    pthread_mutex_lock(&pool->buffer_pool_mutex);
    waitresult = pthread_cond_timedwait (&pool->buffer_pool_cond_not_empty, 
					 &pool->buffer_pool_mutex, &abstime);
    pthread_mutex_unlock(&pool->buffer_pool_mutex);

    this->class->xine->port_ticket->acquire(this->class->xine->port_ticket, 1);
    result = MAX(0, pool->size(pool)) +
             MAX(0, buffer->size(buffer)) +
             this->stream->video_out->get_property(this->stream->video_out, 
						   VO_PROP_BUFS_IN_FIFO);
    this->class->xine->port_ticket->release(this->class->xine->port_ticket, 1);
  }

  TRACE("vdr_plugin_flush returns %d (%d+%d used, %d frames)\n", result,
	pool->size(pool), buffer->size(buffer),
	(int)this->stream->video_out->get_property(this->stream->video_out, 
						   VO_PROP_BUFS_IN_FIFO));
  
  return result;
}

/*
 * vdr_plugin_flush_remote()
 *
 * vdr_plugin_flush() Wrapper for remote mode
 *  - wait for data in network buffers
 */
static int vdr_plugin_flush_remote(vdr_input_plugin_t *this, int timeout_ms, 
				   uint64_t offset, int frame)
{
  int r, live_mode;

  pthread_mutex_lock(&this->lock);

  live_mode = this->live_mode;
  this->live_mode = 0; /* --> 1 again when data arrives ... */

  LOGSCR("reset scr tuning by flush_remote");
  reset_scr_tuning(this);

  /* wait until all data has been received */
  while(this->curpos < offset && timeout_ms > 0) {
    TRACE("FLUSH: wait position (%" PRIu64 " ; need %" PRIu64 ")", 
	  this->curpos, offset);
    pthread_mutex_unlock(&this->lock);
    xine_usec_sleep(3*1000);
    pthread_mutex_lock(&this->lock);
    timeout_ms -= 3;
  }

  LOGSCR("reset scr tuning by flush_remote");
  reset_scr_tuning(this);

  pthread_mutex_unlock(&this->lock);

  r = vdr_plugin_flush(this, MAX(5, timeout_ms));
  printf_control(this, "RESULT %d %d\r\n", this->token, r);

  pthread_mutex_lock(&this->lock);

  this->live_mode = live_mode;
  this->stream->metronom->set_option(this->stream->metronom, 
				     METRONOM_PREBUFFER, 
				     METRONOM_PREBUFFER_VAL);
  this->guard_index = offset;

  pthread_mutex_unlock(&this->lock);

  return CONTROL_OK;
}

static int is_lang_code(const char *s, int len)
{
  while (len--)
    if (!islower(*(s++)))
      return 0;
  return !isalpha(*s);
}

static int vdr_plugin_parse_control(vdr_input_plugin_if_t *this_if, const char *cmd)
{
  vdr_input_plugin_t *this = (vdr_input_plugin_t *) this_if;
  int err = CONTROL_OK;
  int /*int32_t*/ tmp32 = 0;
  uint64_t tmp64 = 0ULL;
  xine_stream_t *stream = this->stream;
  static const char str_poll[] = "POLL";
  char *pt;

  VDR_ENTRY_LOCK(CONTROL_DISCONNECTED);

  LOGCMD("vdr_plugin_parse_control(): %s", cmd);

  if( !memcmp(cmd, str_poll, 4) ||
      !strncasecmp(cmd, "POLL ", 5)) {
    tmp32 = atoi(cmd+5);
    if(tmp32 >= 0 && tmp32 < 1000) {
      if(this->fd_control >= 0) {
        printf_control(this, "POLL %d\r\n", vdr_plugin_poll(this, tmp32));
      } else {
	err = vdr_plugin_poll(this, tmp32);
      }
    } else {
      err = CONTROL_PARAM_ERROR;
    }
    VDR_ENTRY_UNLOCK();
    return err;
  }

  if (this->slave.stream)
    stream = this->slave.stream;

  if(NULL != (pt = strstr(cmd, "\r\n")))
    *((char*)pt) = 0; /* auts */

  LOGVERBOSE("<control> %s",cmd);

  if(!strncasecmp(cmd, "OSDCMD", 6)) {
    err = handle_control_osdcmd(this);

  } else if(!strncasecmp(cmd, "VIDEO_PROPERTIES ", 17)) {
    int hue, saturation, brightness, sharpness, noise_reduction, contrast, vo_aspect_ratio;
    if(7 == sscanf(cmd+17, "%d %d %d %d %d %d %d", 
		   &hue, &saturation, &brightness, &sharpness, &noise_reduction, &contrast, &vo_aspect_ratio))
      err = set_video_properties(this, hue, saturation, brightness, sharpness, noise_reduction, contrast, vo_aspect_ratio);
    else
      err = CONTROL_PARAM_ERROR;

  } else if(!strncasecmp(cmd, "OVERSCAN ", 9)) {
    if(!this->funcs.fe_control)
      LOGMSG("No fe_control function! %s failed.", cmd);
    else
      this->funcs.fe_control(this->funcs.fe_handle, cmd);

  } else if(!strncasecmp(cmd, "VO_ASPECT ", 10)) {
    if(1 == sscanf(cmd+10, "%d", &tmp32)) {
      xine_set_param(stream, XINE_PARAM_VO_ASPECT_RATIO, tmp32);
    } else
      err = CONTROL_PARAM_ERROR;

  } else if(!strncasecmp(cmd, "DEINTERLACE ", 12)) {
    if(this->fd_control < 0)
      err = set_deinterlace_method(this, cmd+12);

  } else if(!strncasecmp(cmd, "EVENT ", 6)) {
    unsigned i;
    char *pt = strchr(cmd, '\n');
    if(pt) *pt=0;
    pt = strstr(cmd+6, " CHAPTER");
    if(pt) {
      *pt = 0;
      this->class->xine->config->update_num(this->class->xine->config,
					    "media.dvd.skip_behaviour", 1);
      this->class->xine->config->update_num(this->class->xine->config,
					    "media.bluray.skip_behaviour", 0);
    }
    pt = strstr(cmd+6, " TITLE");
    if(pt) {
      *pt = 0;
      this->class->xine->config->update_num(this->class->xine->config,
					    "media.dvd.skip_behaviour", 2);
      this->class->xine->config->update_num(this->class->xine->config,
					    "media.bluray.skip_behaviour", 1);
    }
    for(i=0; i<sizeof(eventmap)/sizeof(eventmap[0]); i++)
      if(!strcmp(cmd+6, eventmap[i].name)) {
        xine_event_t ev = {
          .type = eventmap[i].type,
          .stream = this->slave.stream ?: this->stream,
          /* tag event to prevent circular input events
             (vdr -> here -> event_listener -> vdr -> ...) */
          .data = "VDR",
          .data_length = 4,
        };
	xine_event_send(ev.stream, &ev);
	break;
      }

  } else if(!strncasecmp(cmd, "VERSION ", 7)) {
    if(strncmp(XINELIBOUTPUT_VERSION " ", cmd+8, 
	       strlen(XINELIBOUTPUT_VERSION)+1)) {
      if(this->fd_control < 0) {
      /* Check should use protocol version.
       * In remote mode check is done in connect */
	LOGMSG("WARNING! xineplug_inp_xvdr.so and libvdr-xineliboutput.so "
	       "are from different version (%s and %s)", XINELIBOUTPUT_VERSION, cmd+8);
	LOGMSG("Re-install plugin !");
	/*abort();*/
      }
    }

  } else if(!strncasecmp(cmd, "HDMODE ", 7)) {
    if(1 == sscanf(cmd+7, "%d", &tmp32)) {
      pthread_mutex_lock(&this->lock);
      if (tmp32 && !this->hd_stream) {
        cfg_entry_t *e = this->class->xine->config->lookup_entry(this->class->xine->config,
                                                                 "engine.buffers.video_num_frames");
        if (e && e->num_value < 32) {
          LOGMSG("WARNING: xine-engine setting \"engine.buffers.video_num_frames\":%d is "
                 "too small for some HD channels", e->num_value);
        }

      }
      if(tmp32) {
	if(!this->hd_buffer)
	  this->hd_buffer = fifo_buffer_new(this->stream, this->class->num_buffers_hd, HD_BUF_ELEM_SIZE);
	this->hd_stream = 1;
      } else {
	this->hd_stream = 0;
      }
      set_buffer_limits(this);
      pthread_mutex_unlock(&this->lock);
    }

  } else if(!strncasecmp(cmd, "NOVIDEO ", 8)) {
    if(1 == sscanf(cmd+8, "%d", &tmp32)) {
      pthread_mutex_lock(&this->lock);

      this->no_video = tmp32;
      set_buffer_limits(this);
#if 0
      if (tmp32)
        this->metronom->unwire(this->metronom);
      else
        this->metronom->wire(this->metronom);
#endif
      pthread_mutex_unlock(&this->lock);

    } else
      err = CONTROL_PARAM_ERROR;

    signal_buffer_pool_not_empty(this);

  } else if(!strncasecmp(cmd, "DISCARD ", 8)) {
    if(2 == sscanf(cmd+8, "%" PRIu64 " %d", &tmp64, &tmp32)) {
      pthread_mutex_lock(&this->lock);
      if(this->discard_index < tmp64) {
	this->discard_frame = tmp32;
	vdr_flush_engine(this, tmp64);
	this->discard_index = tmp64;
      } else if(this->discard_index != tmp64) {
	LOGMSG("Ignoring delayed control message %s", cmd);
      }
      pthread_cond_broadcast(&this->engine_flushed);
      pthread_mutex_unlock(&this->lock);
    } else 
      err = CONTROL_PARAM_ERROR;

  } else if(!strncasecmp(cmd, "STREAMPOS ", 10)) {
    if(1 == sscanf(cmd+10, "%" PRIu64, &tmp64)) {
      pthread_mutex_lock(&this->lock);
      vdr_flush_engine(this, tmp64);
      this->curpos = tmp64;
      this->discard_index = this->curpos;
      this->guard_index = 0;
      pthread_mutex_unlock(&this->lock);
    } else 
      err = CONTROL_PARAM_ERROR;

  } else if(!strncasecmp(cmd, "TRICKSPEED ", 11)) {
    if (1 == sscanf(cmd+11, "%d", &tmp32)) {
      pthread_mutex_lock(&this->lock);
      set_trick_speed(this, tmp32, !!strstr(cmd+11, "Back"));
      pthread_mutex_unlock(&this->lock);
    } else {
      err = CONTROL_PARAM_ERROR;
    }

  } else if(!strncasecmp(cmd, "STILL ", 6)) {
    if(1 == sscanf(cmd+6, "%d", &tmp32)) {
      pthread_mutex_lock(&this->lock);
      set_still_mode(this, tmp32);
      pthread_mutex_unlock(&this->lock);
    } else {
      err = CONTROL_PARAM_ERROR;
    }

  } else if(!strncasecmp(cmd, "SCR ", 4)) {
    pthread_mutex_lock(&this->lock);
    if(1 == sscanf(cmd, "SCR Sync %d", &tmp32)) {
      this->scr_live_sync = 1;
      this->scr->set_speed_base(this->scr, tmp32);
    }
    else if(1 == sscanf(cmd, "SCR NoSync %d", &tmp32)) {
      this->scr_live_sync = 0;
      this->scr->set_speed_base(this->scr, tmp32);
      reset_scr_tuning(this);
    }
    pthread_mutex_unlock(&this->lock);

  } else if(!strncasecmp(cmd, "LIVE ", 5)) {
    if (1 == sscanf(cmd+5, "%d", &tmp32)) {
      pthread_mutex_lock(&this->lock);
      set_live_mode(this, tmp32);
      pthread_mutex_unlock(&this->lock);
    } else {
      err = CONTROL_PARAM_ERROR;
    }

  } else if(!strncasecmp(cmd, "MASTER ", 7)) {
    if(1 == sscanf(cmd+7, "%d", &tmp32))
      this->fixed_scr = !!tmp32;
    else
      err = CONTROL_PARAM_ERROR;

  } else if(!strncasecmp(cmd, "VOLUME ", 7)) {
    if(1 == sscanf(cmd+7, "%d", &tmp32)) {
      int sw = !!strstr(cmd, "SW");
      if(!sw) {
	xine_set_param(stream, XINE_PARAM_AUDIO_VOLUME, tmp32);
	xine_set_param(stream, XINE_PARAM_AUDIO_MUTE, tmp32<=0 ? 1 : 0);
      } else {
	xine_set_param(stream, XINE_PARAM_AUDIO_AMP_LEVEL, tmp32);
	xine_set_param(stream, XINE_PARAM_AUDIO_AMP_MUTE, tmp32<=0 ? 1 : 0);
      }
      if(sw != this->sw_volume_control) {
	this->sw_volume_control = sw;
	if(sw) {
          /*
           * XXX make sure libxine's internal copy of the mixer
           * volume is initialized before using XINE_PARAM_AUDIO_MUTE...
           * (this fixes mixer volume being reset to 45 here every time
           *  at vdr-sxfe start when using software volume control.)
           */
          tmp32 = xine_get_param(stream, XINE_PARAM_AUDIO_VOLUME);
          xine_set_param(stream, XINE_PARAM_AUDIO_VOLUME, tmp32);

	  xine_set_param(stream, XINE_PARAM_AUDIO_MUTE, 0);
	} else {
	  xine_set_param(stream, XINE_PARAM_AUDIO_AMP_LEVEL, 100);
	  xine_set_param(stream, XINE_PARAM_AUDIO_AMP_MUTE, 0);
	}
      }
    } else
      err = CONTROL_PARAM_ERROR;

  } else if(!strncasecmp(cmd, "AUDIOCOMPRESSION ",17)) {
    if(1 == sscanf(cmd+17, "%d", &tmp32)) {
      xine_set_param(stream, XINE_PARAM_AUDIO_COMPR_LEVEL, tmp32);
    } else
      err = CONTROL_PARAM_ERROR;

  } else if(!strncasecmp(cmd, "AUDIOSURROUND ",14)) {
    if(1 == sscanf(cmd+14, "%d", &tmp32)) {
      this->class->xine->config->update_num(this->class->xine->config,
					    "audio.a52.surround_downmix", tmp32?1:0);
    } else
      err = CONTROL_PARAM_ERROR;

  } else if(!strncasecmp(cmd, "SPEAKERS ",9)) {
    if(1 == sscanf(cmd+9, "%d", &tmp32)) {
      if (this->fd_control < 0)
      this->class->xine->config->update_num(this->class->xine->config,
					    "audio.output.speaker_arrangement", tmp32);
    } else
      err = CONTROL_PARAM_ERROR;

  } else if(!strncasecmp(cmd, "EQUALIZER ", 10)) {
    int eqs[XINE_PARAM_EQ_16000HZ - XINE_PARAM_EQ_30HZ + 2] = {0}, i, j;
    sscanf(cmd+10,"%d %d %d %d %d %d %d %d %d %d",
	   eqs,eqs+1,eqs+2,eqs+3,eqs+4,eqs+5,eqs+6,eqs+7,eqs+8,eqs+9);
    for(i=XINE_PARAM_EQ_30HZ,j=0; i<=XINE_PARAM_EQ_16000HZ; i++,j++)
      xine_set_param(stream, i, eqs[j]);

  } else if(!strncasecmp(cmd, "AUDIOSTREAM ", 12)) {
    if (!this->slave.stream) {
#if 0
      int ac3 = !strncmp(cmd+12, "AC3", 3);
      if(1 == sscanf(cmd+12 + 4*ac3, "%d", &tmp32)) {
	pthread_mutex_lock(&this->lock);
	this->audio_stream_id = tmp32;
	pthread_mutex_unlock(&this->lock);
      } else {
	err = CONTROL_PARAM_ERROR;
      }
#endif
    } else {
      if(1 == sscanf(cmd+12, "AC3 %d", &tmp32)) {
	tmp32 &= 0xff;
	LOGDBG("Audio channel -> [%d]", tmp32);
	xine_set_param(stream, XINE_PARAM_AUDIO_CHANNEL_LOGICAL, tmp32);
      }
      LOGDBG("Audio channel selected: [%d]", _x_get_audio_channel (stream));
    }

  } else if(!strncasecmp(cmd, "SPUSTREAM ", 10)) {
    int old_ch  = _x_get_spu_channel(stream);
    int max_ch  = xine_get_stream_info(stream, XINE_STREAM_INFO_MAX_SPU_CHANNEL);
    int ch      = old_ch;
    int ch_auto = !!strstr(cmd+10, "auto");
    int is_dvd  = 0;

    if (this->slave.stream && this->slave.stream->input_plugin) {
      const char *mrl = this->slave.stream->input_plugin->get_mrl(this->slave.stream->input_plugin);
      is_dvd = !strncmp(mrl, "dvd:/", 5) ||
               !strncmp(mrl, "bd:/", 4) ||
               !strncmp(mrl, "bluray:/", 8);
    }

    if(strstr(cmd+10, "NEXT"))
      ch = ch < max_ch ? ch+1 : -2;
    else if(strstr(cmd+10, "PREV"))
      ch = ch > -2 ? ch-1 : max_ch-1;
    else if(1 == sscanf(cmd+10, "%d", &tmp32)) {
      ch = tmp32;
    } else if(is_lang_code(cmd+10, 2)) {
      /* ISO 639-1 language code */
      const char spu_lang[3] = {cmd[10], cmd[11], 0};
      LOGMSG("Preferred SPU language: %s", spu_lang);
      this->class->xine->config->update_string(this->class->xine->config,
					    "media.dvd.language", spu_lang);
      ch = old_ch = 0;
    } else if(is_lang_code(cmd+10, 3)) {
      /* ISO 639-2 language code */
      const char spu_lang[4] = {cmd[10], cmd[11], cmd[12], 0};
      LOGMSG("Preferred SPU language: %s", spu_lang);
      this->class->xine->config->update_string(this->class->xine->config,
                                               "media.bluray.language", spu_lang);

      ch = old_ch = 0;
    } else
      err = CONTROL_PARAM_ERROR;

    if (old_ch == SPU_CHANNEL_AUTO)
      old_ch = stream->spu_channel_auto;

    if (ch != old_ch) {
      if (is_dvd && ch_auto && stream->spu_channel_user == SPU_CHANNEL_AUTO) {
	LOGDBG("Automatic SPU channel %d->%d ignored", old_ch, ch);
      } else {
	LOGDBG("Forced SPU channel %d->%d", old_ch, ch);
	select_spu_channel(stream, ch);
      }
      LOGDBG("SPU channel selected: [%d]", _x_get_spu_channel (stream));
    }

  } else if(!strncasecmp(cmd, "AUDIODELAY ", 11)) {
    if(1 == sscanf(cmd+11, "%d", &tmp32))
      xine_set_param(stream, XINE_PARAM_AV_OFFSET, tmp32*90000/1000);
    else
      err = CONTROL_PARAM_ERROR;

  } else if(!strncasecmp(cmd, "SYNC", 4)) {
    if(this->fd_control >= 0)
      printf_control(this, "RESULT %d 1\r\n", this->token);

  } else if(!strncasecmp(cmd, "GETSTC", 6)) {
    int64_t pts = -1;
    pts = stream->metronom->get_option(stream->metronom, XVDR_METRONOM_LAST_VO_PTS);
    if (pts <= 0) {
      pts = xine_get_current_vpts(stream) -
            stream->metronom->get_option(stream->metronom, METRONOM_VPTS_OFFSET);
    }

    if(this->fd_control >= 0) {
      printf_control(this, "STC %" PRId64 "\r\n", pts);
    } else {
      *((int64_t *)cmd) = pts;
    }

  } else if(!strncasecmp(cmd, "FLUSH ", 6)) {
    if(1 == sscanf(cmd+6, "%d", &tmp32)) {
      if(this->fd_control >= 0) {
	uint32_t frame = 0;
	tmp64 = 0ULL; 
	tmp32 = 0;
	sscanf(cmd+6, "%d %" PRIu64 " %d", &tmp32, &tmp64, &frame);
	err = vdr_plugin_flush_remote(this, tmp32, tmp64, frame);
      } else {
	err = vdr_plugin_flush(this, tmp32);
      }
    } else
      err = CONTROL_PARAM_ERROR;

  } else if(!strncasecmp(cmd, "TOKEN ", 6)) {
    if(1 == sscanf(cmd+6, "%d", &tmp32))
      this->token = tmp32;
    else
      err = CONTROL_PARAM_ERROR;

  } else if(!strncasecmp(cmd, "SUBSTREAM ", 9)) {
    err = handle_control_substream(this, cmd);

  } else if(!strncasecmp(cmd, "POST ", 5)) {
    /* lock demuxer thread out of adjust_realtime_speed */
    pthread_mutex_lock(&this->lock);
    if(!this->funcs.fe_control)
      LOGMSG("No fe_control function! %s failed.", cmd);
    else
      this->funcs.fe_control(this->funcs.fe_handle, cmd);
    pthread_mutex_unlock(&this->lock);

  } else if(!strncasecmp(cmd, "PLAYFILE ", 9)) {
    err = handle_control_playfile(this, cmd);
    if(this->fd_control >= 0) {
      printf_control(this, "RESULT %d %d\r\n", this->token, err);
      err = CONTROL_OK;
    }
    
  } else if(!strncasecmp(cmd, "SEEK ", 5)) {
    if (this->slave.stream) {
      int pos_stream=0, pos_time=0, length_time=0;
      xine_get_pos_length(this->slave.stream,
			  &pos_stream, &pos_time, &length_time);
      if(cmd[5]=='+')
	pos_time += atoi(cmd+6) * 1000;
      else if(cmd[5]=='-')
	pos_time -= atoi(cmd+6) * 1000;
      else
	pos_time = atoi(cmd+5) * 1000;
      err = xine_play (this->slave.stream, 0, pos_time);
      if(this->fd_control >= 0)
	err = CONTROL_OK;
    }

  } else if(!strncasecmp(cmd, "GETLENGTH", 9)) {
    int pos_stream=0, pos_time=0, length_time=0;
    xine_get_pos_length(stream, &pos_stream, &pos_time, &length_time);
    err = length_time/*/1000*/;
    if(this->fd_control >= 0) {
      printf_control(this, "RESULT %d %d\r\n", this->token, err);
      err = CONTROL_OK;
    }

  } else if(!strncasecmp(cmd, "GETAUTOPLAYSIZE", 15)) {

    if (cmd[15]==' ' && cmd[16]) {
      /* query from specific input plugin */
      const char *cls_name = cmd + 16;
      this->autoplay_size = 0;
      if (! xine_get_browse_mrls (stream->xine,
                                  cls_name,
                                  NULL, &this->autoplay_size))
        /* try older method */
        xine_get_autoplay_mrls(stream->xine, cls_name, &this->autoplay_size);
    }

    if(this->autoplay_size < 0) {
      if (this->slave.stream &&
          this->slave.stream->input_plugin &&
          this->slave.stream->input_plugin->input_class)
        this->slave.stream->input_plugin->input_class->
          get_autoplay_list(this->slave.stream->input_plugin->input_class, &this->autoplay_size);
    }
    err = this->autoplay_size;
    if(this->fd_control >= 0) {
      printf_control(this, "RESULT %d %d\r\n", this->token, err);
      err = CONTROL_OK;
    }

  } else if(!strncasecmp(cmd, "GETPOS", 6)) {
    int pos_stream=0, pos_time=0, length_time=0;
    xine_get_pos_length(stream, &pos_stream, &pos_time, &length_time);
    err = pos_time/*/1000*/;
    if(this->fd_control >= 0) {
      printf_control(this, "RESULT %d %d\r\n", this->token, err);
      err = CONTROL_OK;
    }

  } else if(!strncasecmp(cmd, "SUBTITLES ", 10)) {
    if (this->slave.stream) {
      int vpos = 0;
      if(1 == sscanf(cmd+10, "%d", &vpos))
	this->class->xine->config->update_num(this->class->xine->config,
		          "subtitles.separate.vertical_offset", vpos);
      else
	err = CONTROL_PARAM_ERROR;
    }

  } else if(!strncasecmp(cmd, "EXTSUBSIZE ", 11)) {
    int size = 0;
    if(1 == sscanf(cmd+11, "%d", &size))
      /* size of separate subtitles :
	 -1 = xine default 
	 0...6 = { tiny  small  normal  large  very large  huge } */
      this->class->xine->config->update_num(this->class->xine->config, 
					    "subtitles.separate.subtitle_size", size);
    else
      err = CONTROL_PARAM_ERROR;

  } else if(!strncasecmp(cmd, "CONFIG END", 10)) {
    this->config_ok = 1;

  } else if(!strncasecmp(cmd, "GRAB ", 5)) {
    handle_control_grab(this, cmd);

  /* next ones need to be synchronized to data stream */
  } else if(!strncasecmp(cmd, "BLANK", 5)) {
    put_control_buf(this->block_buffer, this->buffer_pool, CONTROL_BUF_BLANK);

  } else if(!strncasecmp(cmd, "CLEAR", 5)) {
    /* #warning should be delayed and executed in read_block */

  } else {
    LOGMSG("vdr_plugin_parse_control(): unknown control %s", cmd);
    err = CONTROL_UNKNOWN;
  }

  LOGCMD("vdr_plugin_parse_control(): DONE (%d): %s", err, cmd);

  VDR_ENTRY_UNLOCK();

  return err;
}

static void *vdr_control_thread(void *this_gen)
{
  vdr_input_plugin_t *this = (vdr_input_plugin_t *) this_gen;
  char line[8128];
  int err;
  int counter = 100;

  LOGDBG("Control thread started");

  /*(void)nice(-1);*/

  /* wait until state changes from open to play */
  while(bSymbolsFound && counter>0 && ! this->funcs.fe_control) {
    xine_usec_sleep(50*1000);
    counter--;
  } 

  if (this->osd_manager && this->osd_manager->argb_supported(this->stream)) {
    LOGMSG("ARGB OSD supported by video driver");
    puts_vdr(this, "INFO ARGBOSD\r\n");
  }

  write_control(this, "CONFIG\r\n");
  
  while(this->control_running) {

    /* read next command */
    line[0] = 0;
    pthread_testcancel();
    if((err=readline_control(this, line, sizeof(line)-1, -1)) <= 0) {
      if(err < 0)
	break;
      continue;
    }
    LOGCMD("Received command %s",line);
    pthread_testcancel();
    
    if(!this->control_running) 
      break;

    /* parse */
    switch(err = vdr_plugin_parse_control(&this->iface, line)) {
      case CONTROL_OK:
        break;
      case CONTROL_UNKNOWN:
        LOGMSG("unknown control message %s", line);
        break;
      case CONTROL_PARAM_ERROR:
        LOGMSG("invalid parameter in control message %s", line);
        break;
      case CONTROL_DISCONNECTED:
        LOGMSG("control stream read error - disconnected ?");
        this->control_running = 0;
        break;
      default:
        LOGMSG("parse_control failed with result: %d", err);
        break;
    }
  }

  if(this->control_running)
    write_control(this, "CLOSE\r\n");
  this->control_running = 0;

  if (this->slave.stream) {
    xine_stop(this->slave.stream);
  }

  LOGDBG("Control thread terminated");
  pthread_exit(NULL);
}

/**************************** Control to VDR ********************************/

static void update_dvd_title_number(vdr_input_plugin_t *this)
{
  /* DVD title number and menu domain detection */

#ifdef XINE_STREAM_INFO_DVD_TITLE_NUMBER
  int tn = _x_stream_info_get(this->slave.stream, XINE_STREAM_INFO_DVD_TITLE_NUMBER);
  int tc = _x_stream_info_get(this->slave.stream, XINE_STREAM_INFO_DVD_TITLE_COUNT);

  if (tn >= 0 && tc > 0) {
    if (tn == 0)
      dvd_menu_domain(this, 1);

    printf_vdr(this, "INFO DVDTITLE %d/%d\r\n", tn, tc);
  }
#endif
}

static const char *trim_str(const char *s)
{
  while (*s == ' ' || *s == '\r' || *s == '\n')
    s++;
  return s;
}

static void slave_track_maps_changed(vdr_input_plugin_t *this)
{
  char tracks[1024], lang[128];
  int i, current, n = 0;
  size_t cnt;

  /* DVD title and menu domain detection */
  update_dvd_title_number(this);

  /* Audio tracks */
  
  strcpy(tracks, "INFO TRACKMAP AUDIO ");
  cnt = strlen(tracks);
  current = xine_get_param(this->slave.stream, XINE_PARAM_AUDIO_CHANNEL_LOGICAL);
  for(i=0; i<32 && cnt<sizeof(tracks)-32; i++)
    if (xine_get_audio_lang(this->slave.stream, i, lang)) {
      cnt += snprintf(tracks+cnt, sizeof(tracks)-cnt-32, 
		      "%s%d:%s ", i==current?"*":"", i, trim_str(lang));
      n++;
    } else if (i < this->slave.stream->audio_track_map_entries) {
      cnt += snprintf(tracks+cnt, sizeof(tracks)-cnt-32,
                      "%s%d:%d ", i==current?"*":"", i, i);
      n++;
    }
  tracks[sizeof(tracks)-1] = 0;
  if(n>1)
    LOGDBG("%s", tracks);

  strcpy(tracks + cnt, "\r\n");
  puts_vdr(this, tracks);

  /* DVD SPU tracks */

  n = 0;  
  strcpy(tracks, "INFO TRACKMAP SPU ");
  cnt = strlen(tracks);
  current = _x_get_spu_channel (this->slave.stream);
  if(current < 0) {
    /* -2 == none, -1 == auto */
    cnt += snprintf(tracks+cnt, sizeof(tracks)-cnt-32,
		    "*%d:%s ", current, 
		    current==SPU_CHANNEL_NONE ? "none" : "auto");
    n++;
    if(current == SPU_CHANNEL_AUTO)
      current = this->slave.stream->spu_channel_auto;
  }
  for(i=0; i<32 && cnt<sizeof(tracks)-32; i++)
    if (xine_get_spu_lang(this->slave.stream, i, lang)) {
      cnt += snprintf(tracks+cnt, sizeof(tracks)-cnt-32,
		      "%s%d:%s ", i==current?"*":"", i, trim_str(lang));
      n++;
    }
  tracks[sizeof(tracks)-1] = 0;
  if(n>1)
    LOGDBG("%s", tracks);

  strcpy(tracks + cnt, "\r\n");
  puts_vdr(this, tracks);
}

/* Map some xine input events to vdr input (remote key names) */
static const struct {
  const uint32_t  event;
  const char      name[12];
} vdr_keymap[] = {
  {XINE_EVENT_INPUT_NEXT,     "Next"},
  {XINE_EVENT_INPUT_PREVIOUS, "Previous"},

  {XINE_EVENT_INPUT_DOWN,     "Down"},
  {XINE_EVENT_INPUT_UP,       "Up"},
  {XINE_EVENT_INPUT_LEFT,     "Left"},
  {XINE_EVENT_INPUT_RIGHT,    "Right"},
  {XINE_EVENT_INPUT_SELECT,   "Ok"},

  {XINE_EVENT_INPUT_MENU1,    "Menu"},
  {XINE_EVENT_INPUT_MENU2,    "Red"},
  {XINE_EVENT_INPUT_MENU3,    "Green"},
  {XINE_EVENT_INPUT_MENU4,    "Yellow"},
  {XINE_EVENT_INPUT_MENU5,    "Blue"},
  {XINE_EVENT_INPUT_NUMBER_0, "0"},
  {XINE_EVENT_INPUT_NUMBER_1, "1"},
  {XINE_EVENT_INPUT_NUMBER_2, "2"},
  {XINE_EVENT_INPUT_NUMBER_3, "3"},
  {XINE_EVENT_INPUT_NUMBER_4, "4"},
  {XINE_EVENT_INPUT_NUMBER_5, "5"},
  {XINE_EVENT_INPUT_NUMBER_6, "6"},
  {XINE_EVENT_INPUT_NUMBER_7, "7"},
  {XINE_EVENT_INPUT_NUMBER_8, "8"},
  {XINE_EVENT_INPUT_NUMBER_9, "9"},

#if defined(XINE_EVENT_VDR_RED)
  {XINE_EVENT_VDR_BACK,         "Back"},
  {XINE_EVENT_VDR_CHANNELPLUS,  "Channel+"},
  {XINE_EVENT_VDR_CHANNELMINUS, "Channel-"},
  {XINE_EVENT_VDR_RED,          "Red"},
  {XINE_EVENT_VDR_GREEN,        "Green"},
  {XINE_EVENT_VDR_YELLOW,       "Yellow"},
  {XINE_EVENT_VDR_BLUE,         "Blue"},
  {XINE_EVENT_VDR_PLAY,         "Play"},
  {XINE_EVENT_VDR_PAUSE,        "Pause"},
  {XINE_EVENT_VDR_STOP,         "Stop"},
  {XINE_EVENT_VDR_RECORD,       "Record"},
  {XINE_EVENT_VDR_FASTFWD,      "FastFwd"},
  {XINE_EVENT_VDR_FASTREW,      "FastRew"},
  {XINE_EVENT_VDR_POWER,        "Power"},
  {XINE_EVENT_VDR_SCHEDULE,     "Schedule"},
  {XINE_EVENT_VDR_CHANNELS,     "Channels"},
  {XINE_EVENT_VDR_TIMERS,       "Timers"},
  {XINE_EVENT_VDR_RECORDINGS,   "Recordings"},
  {XINE_EVENT_VDR_SETUP,        "Setup"},
  {XINE_EVENT_VDR_COMMANDS,     "Commands"},
  {XINE_EVENT_VDR_USER1,        "User1"},
  {XINE_EVENT_VDR_USER2,        "User2"},
  {XINE_EVENT_VDR_USER3,        "User3"},
  {XINE_EVENT_VDR_USER4,        "User4"},
  {XINE_EVENT_VDR_USER5,        "User5"},
  {XINE_EVENT_VDR_USER6,        "User6"},
  {XINE_EVENT_VDR_USER7,        "User7"},
  {XINE_EVENT_VDR_USER8,        "User8"},
  {XINE_EVENT_VDR_USER9,        "User9"},
  {XINE_EVENT_VDR_VOLPLUS,      "Volume+"},
  {XINE_EVENT_VDR_VOLMINUS,     "Volume-"},
  {XINE_EVENT_VDR_MUTE,         "Mute"},
  {XINE_EVENT_VDR_AUDIO,        "Audio"},
#endif
#if defined(XINE_EVENT_VDR_INFO)
  {XINE_EVENT_VDR_INFO,         "Info"},
#endif
#if defined(XINE_EVENT_VDR_SUBTITLES)
  {XINE_EVENT_VDR_SUBTITLES,    "Subtitles"},
#endif
};

static void vdr_event_cb (void *user_data, const xine_event_t *event) 
{
  vdr_input_plugin_t *this = (vdr_input_plugin_t *)user_data;
  unsigned i;

  for (i = 0; i < sizeof(vdr_keymap) / sizeof(vdr_keymap[0]); i++) {
    if ((uint32_t)event->type == vdr_keymap[i].event) {
      if (event->data && event->data_length == 4 &&
          !strncmp(event->data, "VDR", 4)) {
        /*LOGMSG("Input event created by self, ignoring");*/
        return;
      }
      LOGDBG("XINE_EVENT (input) %d --> %s", 
             event->type, vdr_keymap[i].name);

      if (this->fd_control >= 0) {
        /* remote mode: -> input_plugin -> connection -> VDR */
        printf_control(this, "KEY %s\r\n", vdr_keymap[i].name);
      }
      if (this->funcs.xine_input_event) {
        /* local mode: -> VDR */
        this->funcs.xine_input_event(this->funcs.fe_handle, NULL, vdr_keymap[i].name);
      }
      return;
    }
  }

  switch (event->type) {
    case XINE_EVENT_UI_SET_TITLE:
      if (event->stream == this->slave.stream) {
	xine_ui_data_t *data = (xine_ui_data_t *)event->data;
	LOGMSG("XINE_EVENT_UI_SET_TITLE: %s", data->str);

        update_dvd_title_number(this);

        printf_vdr(this, "INFO TITLE %s\r\n", data->str);
	break;
      }

    case XINE_EVENT_UI_NUM_BUTTONS:
      if (event->stream == this->slave.stream) {
	xine_ui_data_t *data = (xine_ui_data_t*)event->data;
	dvd_menu_domain(this, data->num_buttons > 0);
	printf_vdr(this, "INFO DVDBUTTONS %d\r\n", data->num_buttons);
	break;
      }

    case XINE_EVENT_UI_CHANNELS_CHANGED:
      if (event->stream==this->slave.stream) {
	slave_track_maps_changed(this);
      }
      break;

    case XINE_EVENT_FRAME_FORMAT_CHANGE:
      {
        xine_format_change_data_t *frame_change = 
	    (xine_format_change_data_t *)event->data;
        LOGOSD("XINE_EVENT_FRAME_FORMAT_CHANGE (%dx%d, aspect=%d)", 
	       frame_change->width, frame_change->height, 
	       frame_change->aspect);
	if (!frame_change->aspect) /* from frontend */
	  this->osd_manager->video_size_changed(this->osd_manager, event->stream, 
						frame_change->width, frame_change->height);
      }
      break;

    case XINE_EVENT_UI_PLAYBACK_FINISHED:
      if(event->stream == this->stream) {
	LOGDBG("XINE_EVENT_UI_PLAYBACK_FINISHED");
	this->control_running = 0;
#if 1
	if(iSysLogLevel >= SYSLOGLEVEL_DEBUG) {
	  /* dump whole xine log as we should not be here ... */
	  xine_t *xine = this->class->xine;
	  int i, j;
	  int logs = xine_get_log_section_count(xine);
	  const char * const * names = xine_get_log_names(xine);
	  for(i=0; i<logs; i++) {
#if XINE_VERSION_CODE < 10105
	    const char * const * lines = xine_get_log(xine, i);
#else
	    char * const * lines = xine_get_log(xine, i);
#endif
	    if(lines[0]) {
	      printf("\nLOG: %s\n",names[i]);
	      for(j=0; lines[j] && *lines[j]; j++)
		printf("  %2d: %s", j, lines[j]);
	    }
	  }
	}
#endif
      }

      pthread_mutex_lock(&this->lock);
      if (event->stream == this->slave.stream) {
	LOGMSG("XINE_EVENT_UI_PLAYBACK_FINISHED (slave stream)");
	if (this->fd_control >= 0) {
	  write_control(this, "ENDOFSTREAM\r\n");
	} else {
	  if (this->funcs.fe_control)
	    this->funcs.fe_control(this->funcs.fe_handle, "ENDOFSTREAM\r\n");
	}
      } else if(event->stream == this->bg_stream.stream) {
        LOGMSG("XINE_EVENT_UI_PLAYBACK_FINISHED (background stream)");
        xine_play(this->bg_stream.stream, 0, 0);
      }
      pthread_mutex_unlock(&this->lock);

      break;

    default:
      LOGCMD("Got an xine event, type 0x%08x", event->type);
      break;
    }
}

/**************************** Data Stream *********************************/

/*
 * wait_stream_sync()
 *
 * Wait until data and control streams have reached the same sync point.
 *  - wait_stream_sync() is called only from data thread.
 *  - data stream handling is suspended until control stream has
 *    reached the same sync point (and xine engine has been flushed).
 *
 * Function is interrupred when
 *  - control thread has been terminated
 *  - demux_action_pending signal is set
 *
 * return value:
 *   1:  Both streams have reached the same sync point
 *   0:  timeout      (errno = EAGAIN)   or
 *       interrupted  (errno = EINTR)    or
 *       disconnected (errno = ENOTCONN)
 */
static int wait_stream_sync(vdr_input_plugin_t *this)
{
  int counter = 100;

  mutex_lock_cancellable(&this->lock);

  if (this->discard_index < this->discard_index_ds)
    LOGVERBOSE("wait_stream_sync: waiting for engine_flushed condition %"PRIu64"<%"PRIu64,
               this->discard_index, this->discard_index_ds);

  while(this->control_running &&
        this->discard_index < this->discard_index_ds &&
        !_x_action_pending(this->stream) &&
        --counter > 0) {
    struct timespec abstime;
    create_timeout_time(&abstime, 10);
    pthread_cond_timedwait(&this->engine_flushed, &this->lock, &abstime);
  }

  if (this->discard_index != this->curpos) {
    LOGMSG("wait_stream_sync: discard_index %"PRIu64" != curpos %"PRIu64" ! (diff %"PRId64")",
           this->discard_index, this->curpos, (int64_t)(this->discard_index - this->curpos));
  }

  mutex_unlock_cancellable(&this->lock);

  if (this->discard_index == this->discard_index_ds) {
    LOGVERBOSE("wait_stream_sync: streams synced at %"PRIu64"/%"PRIu64,
               this->discard_index_ds, this->discard_index);
    return 0;
  }

  if (!this->control_running) {
    errno = ENOTCONN;
  }
  else if (_x_action_pending(this->stream)) {
    LOGVERBOSE("wait_stream_sync: demux_action_pending set");
    errno = EINTR;
  }
  else if (counter <= 0) {
    LOGMSG("wait_stream_sync: Timed out ! diff %"PRId64,
           (int64_t)(this->discard_index - this->discard_index_ds));
    errno = EAGAIN;
  }

  return 1;
}

static void data_stream_parse_control(vdr_input_plugin_t *this, char *cmd)
{
  char *tmp;

  cmd[64] = 0;
  if(NULL != (tmp=strchr(cmd, '\r')))
    *tmp = '\0';
  if(NULL != (tmp=strchr(cmd, '\n')))
    *tmp = '\0';

  LOGVERBOSE("<control> <data> %s", cmd);

  if(!strncasecmp(cmd, "DISCARD ", 8)) {
    uint64_t index;
    if(1 == sscanf(cmd+8, "%" PRIu64, &index)) {

      this->discard_index_ds = index;

      this->block_buffer->clear(this->block_buffer);

      wait_stream_sync(this);
    }
    return;

  } else if(!strncasecmp(cmd, "BLANK", 5)) {
    put_control_buf(this->block_buffer, this->buffer_pool, CONTROL_BUF_BLANK);
    return;
  }

  LOGMSG("Unexpected data_stream_parse_control(%s)", cmd);
  vdr_plugin_parse_control(&this->iface, cmd);
}

/*
 * vdr_plugin_read_block_tcp()
 *
 * - Read single transport block from socket / pipe.
 *
 * Returns NULL if read failed or data is not available.
 * (sets errno to EAGAIN, EINTR or ENOTCONN)
 *
 */
static buf_element_t *vdr_plugin_read_block_tcp(vdr_input_plugin_t *this)
{
  buf_element_t *read_buffer = this->read_buffer;
  int            todo        = sizeof(stream_tcp_header_t);
  int            warnings    = 0;
  int            result, n;

  if (this->discard_index < this->discard_index_ds &&
      wait_stream_sync(this))
    return NULL;

  if (read_buffer && read_buffer->size >= (ssize_t)sizeof(stream_tcp_header_t))
    todo += ((stream_tcp_header_t *)read_buffer->content)->len;

  while (XIO_READY == (result = _x_io_select(this->stream, this->fd_data, XIO_READ_READY, 100))) {

    if (!this->control_running || this->fd_data < 0) {
      errno = ENOTCONN;
      return NULL;
    }
    if (_x_action_pending(this->stream)) {
      errno = EINTR;
      return NULL;
    }

    /* Allocate buffer */
    if (!read_buffer) {
      this->read_buffer = read_buffer = get_buf_element_timed(this, 2048+sizeof(stream_tcp_header_t), 100);
      if (!read_buffer) {
        /* do not drop any data here ; dropping is done only at server side. */
        if (!this->is_paused && !warnings)
          LOGDBG("TCP: fifo buffer full");
        warnings++;
        continue; /* must call select to check fd for errors / closing */
      }

      /* read the header first */
      todo = sizeof(stream_tcp_header_t);
      read_buffer->size = 0;
      warnings = 0;
    }

    /* Read data */
    errno = 0;
    n = read(this->fd_data, read_buffer->content + read_buffer->size, todo - read_buffer->size);
    if (n <= 0) {
      if (!n || (errno != EINTR && errno != EAGAIN)) {
        if (n < 0 && this->fd_data >= 0)
          LOGERR("TCP read error (data stream %d : %d)", this->fd_data, n);
        if (n == 0)
          LOGMSG("Data stream disconnected");
        errno = ENOTCONN;
      }
      /* errno == EINTR || errno == EAGAIN || errno == ENOTCONN */
      return NULL;
    }

    read_buffer->size += n;

    /* Header complete ? */
    if (read_buffer->size == sizeof(stream_tcp_header_t)) {
      stream_tcp_header_t *hdr = ((stream_tcp_header_t *)read_buffer->content);
      hdr->len = ntohl(hdr->len);
      hdr->pos = ntohull(hdr->pos);

      todo += hdr->len;

      if (todo + read_buffer->size >= read_buffer->max_size) {
        LOGMSG("TCP: Buffer too small (%d ; incoming frame %d bytes)",
               read_buffer->max_size, todo + read_buffer->size);
        errno = ENOTCONN;
        return NULL;
      }
    }

    /* Buffer complete ? */
    if (read_buffer->size >= todo) {
      stream_tcp_header_t *hdr = ((stream_tcp_header_t *)read_buffer->content);
      if (hdr->pos == (uint64_t)(-1ULL) /*0xffffffff ffffffff*/) {
        /* control data */
        uint8_t *pkt_data = read_buffer->content + sizeof(stream_tcp_header_t);
        if (!DATA_IS_TS(pkt_data) && pkt_data[0]) { /* -> can't be pes or ts frame */
          data_stream_parse_control(this, (char*)pkt_data);

          read_buffer->free_buffer(read_buffer);
          this->read_buffer = NULL;

          errno = EAGAIN;
          return NULL;
        }
      }

      /* frame ready */
      read_buffer->type = BUF_NETWORK_BLOCK;
      this->read_buffer = NULL;
      return read_buffer;
    }
  }

  errno = (result == XIO_TIMEOUT) ? EAGAIN :
          (result == XIO_ABORTED) ? EINTR  :
                                    ENOTCONN;
  return NULL;
}

/*
 * read_socket_udp()
 *
 * - Read single transport block from datagram socket
 *
 * Returns NULL if read failed or data is not available.
 * (sets errno to EAGAIN, EINTR or ENOTCONN)
 *
 */
static buf_element_t *read_socket_udp(vdr_input_plugin_t *this)
{
  /*
   * poll for incoming data
   */

  int result = _x_io_select(this->stream, this->fd_data, XIO_READ_READY, 100);

  if (!this->control_running) {
    errno = ENOTCONN;
    return NULL;
  }
  if (_x_action_pending(this->stream)) {
    errno = EINTR;
    return NULL;
  }
  if (result != XIO_READY) {
    if (result == XIO_ERROR)
      LOGERR("read_socket_udp(): select() failed");

    errno = (result == XIO_TIMEOUT) ? EAGAIN :
            (result == XIO_ABORTED) ? EINTR  :
                                      ENOTCONN;
    return NULL;
  }

  /*
   * allocate buffer
   */

  udp_data_t    *udp         = this->udp_data;
  buf_element_t *read_buffer = get_buf_element_timed(this, 2048+sizeof(stream_rtp_header_impl_t), 100);

  if (!read_buffer) {
    /* if queue is full, skip (video) frame.
       Waiting longer for free buffers just makes things worse ... */
    if (!this->is_paused) {
      LOGDBG("UDP Fifo buffer full !");
      if (this->scr && !udp->scr_jump_done) {
        this->scr->jump (this->scr, 40*90);
        LOGMSG("SCR jump: +40 ms (live=%d, tuning=%d)", this->live_mode, this->scr_tuning);
        udp->scr_jump_done = 50;
      }
    }
    errno = EAGAIN;
    return NULL;
  }

  if (udp->scr_jump_done)
    udp->scr_jump_done --;

  /*
   * Receive frame from socket and check for errors
   */

  struct sockaddr_in server_address;
  socklen_t          address_len = sizeof(server_address);

  ssize_t n = recvfrom(this->fd_data, read_buffer->mem,
                       read_buffer->max_size, MSG_TRUNC,
                       &server_address, &address_len);
  if (n <= 0) {
    if (!n || (errno != EINTR && errno != EAGAIN)) {
      LOGERR("read_socket_udp(): recvfrom() failed");
      errno = ENOTCONN;
    }
    read_buffer->free_buffer(read_buffer);
    /* errno == EAGAIN || errno == EINTR || errno == ENOTCONN */
    return NULL;
  }

  /*
   * check source address
   */

  if ((server_address.sin_addr.s_addr !=
       udp->server_address.sin_addr.s_addr) ||
      server_address.sin_port != udp->server_address.sin_port) {
#ifdef LOG_UDP
    uint32_t tmp_ip = ntohl(server_address.sin_addr.s_addr);
    LOGUDP("Received data from unknown sender: %d.%d.%d.%d:%d",
           ((tmp_ip>>24)&0xff), ((tmp_ip>>16)&0xff),
           ((tmp_ip>>8)&0xff), ((tmp_ip)&0xff),
           server_address.sin_port);
#endif
    read_buffer->free_buffer(read_buffer);
    errno = EAGAIN;
    return NULL;
  }

  /*
   * Check if frame size is valid
   */

  if (this->rtp) {
    if (n < (ssize_t)sizeof(stream_rtp_header_impl_t)) {
      LOGMSG("received invalid RTP packet (too short)");
      read_buffer->free_buffer(read_buffer);
      errno = EAGAIN;
      return NULL;
    }
  }
  else if (n < (ssize_t)sizeof(stream_udp_header_t)) {
    LOGMSG("received invalid UDP packet (too short)");
    read_buffer->free_buffer(read_buffer);
    errno = EAGAIN;
    return NULL;
  }

  if (n > read_buffer->max_size) {
    LOGMSG("received too large UDP packet (%zd bytes, buffer is %d bytes)", n, read_buffer->max_size);
    read_buffer->free_buffer(read_buffer);
    errno = EAGAIN;
    return NULL;
  }

  /*
   *
   */

  read_buffer->size = n;
  read_buffer->type = BUF_NETWORK_BLOCK;

  return read_buffer;
}

static buf_element_t *udp_parse_header(buf_element_t *read_buffer, int rtp)
{
  if (rtp) {

    /* check if RTP header is valid. If not, assume UDP without RTP. */
    stream_rtp_header_impl_t *rtp_pkt = (stream_rtp_header_impl_t*)read_buffer->content;
    if (rtp_pkt->rtp_hdr.raw[0] == (RTP_VERSION_BYTE | RTP_HDREXT_BIT) &&
        ( rtp_pkt->rtp_hdr.raw[1] == RTP_PAYLOAD_TYPE_PES ||
          rtp_pkt->rtp_hdr.raw[1] == RTP_PAYLOAD_TYPE_TS ) &&
        rtp_pkt->hdr_ext.hdr.size == htons(RTP_HEADER_EXT_X_SIZE) &&
        rtp_pkt->hdr_ext.hdr.type == htons(RTP_HEADER_EXT_X_TYPE)) {

      /* strip RTP header but leave UDP header (carried inside RTP header extension) */
      read_buffer->content += sizeof(stream_rtp_header_impl_t) - sizeof(stream_udp_header_t);
      read_buffer->size    -= sizeof(stream_rtp_header_impl_t) - sizeof(stream_udp_header_t);
    }
  }

  stream_udp_header_t *pkt = (stream_udp_header_t*)read_buffer->content;

  pkt->seq = ntohs(pkt->seq);
  pkt->pos = ntohull(pkt->pos);

  return read_buffer;
}

static buf_element_t *udp_check_packet(buf_element_t *read_buffer)
{
  stream_udp_header_t *pkt      = (stream_udp_header_t*)read_buffer->content;
  uint8_t             *pkt_data = read_buffer->content + sizeof(stream_udp_header_t);

  /* Check for MPEG-TS sync byte or PES header */
  if (read_buffer->size > (ssize_t)sizeof(stream_udp_header_t) &&
      !DATA_IS_TS(pkt_data) &&
      (pkt_data[0] || pkt_data[1] || pkt_data[2] != 1)) {
    LOGMSG("received invalid UDP packet (TS sync byte or PES header missing)");
    read_buffer->free_buffer(read_buffer);
    return NULL;
  }

  /* Check if header is valid */
  if (pkt->seq > UDP_SEQ_MASK) {
    LOGMSG("received invalid UDP packet (sequence number too big)");
    read_buffer->free_buffer(read_buffer);
    return NULL;
  }

  return read_buffer;
}

static buf_element_t *udp_parse_control(vdr_input_plugin_t *this, buf_element_t *read_buffer)
{
  stream_udp_header_t *pkt      = (stream_udp_header_t*)read_buffer->content;
  uint8_t             *pkt_data = UDP_PAYLOAD(read_buffer->content);

  /* Check for control messages */
  if (/*pkt->seq == (uint16_t)(-1) &&*/ /*0xffff*/
      pkt->pos == (uint64_t)(-1ULL) && /*0xffffffff ffffffff*/
      pkt_data[0]) { /* -> can't be PES frame */
    pkt_data[64] = 0;

    if (!strncmp((char*)pkt_data, "UDP MISSING", 11)) {
      /* Re-send failed */
      int         seq1 = 0;
      int         seq2 = 0;
      uint64_t    rpos = UINT64_C(0);
      udp_data_t *udp  = this->udp_data;

      sscanf(((char*)pkt_data)+12, "%d-%d %" PRIu64,
             &seq1, &seq2, &rpos);
      read_buffer->size = sizeof(stream_udp_header_t);
      read_buffer->type = BUF_NETWORK_BLOCK;
      pkt->pos = rpos;
      LOGUDP("Got UDP MISSING %d-%d (currseq=%d)", seq1, seq2, udp->next_seq);

      if (seq1 == udp->next_seq) {
        /* this is the one we are expecting ... */
        int n = ADDSEQ(seq2 + 1, -seq1);
        udp->missed_frames += n;
        seq2 &= UDP_SEQ_MASK;
        pkt->seq = seq2;
        udp->next_seq = seq2;
        LOGUDP("  accepted: now currseq %d", udp->next_seq);
        /* -> drop frame thru as empty ; it will trigger queue to continue */

      } else {
        LOGUDP("  UDP packet rejected: not expected seq ???");

        read_buffer->free_buffer(read_buffer);
        return NULL;
      }

    } else {
      data_stream_parse_control(this, (char*)pkt_data);

      read_buffer->free_buffer(read_buffer);
      return NULL;
    }
  }

  return read_buffer;
}

static buf_element_t *udp_process_queue(vdr_input_plugin_t *this)
{
  udp_data_t *udp = this->udp_data;

  if (udp->queued <= 0)
    return NULL;

  /*
   * Stay inside receiving window:
   * if window exceeded, skip missing frames
   */

  if (udp->queued > ((UDP_SEQ_MASK+1)>>2)) {
#ifdef LOG_UDP
    int start = udp->next_seq;
#endif
    while (!udp->queue[udp->next_seq]) {
      INCSEQ(udp->next_seq);
      udp->missed_frames++;
    }
    udp->resend_requested = 0;
    LOGUDP("Re-ordering window exceeded, skipped missed frames %d-%d",
           start, udp->next_seq-1);
  }

  /*
   * flush all packets when idle padding found
   */

  if (udp->is_padding && udp->queued > 0)
    while (!udp->queue[udp->next_seq]) {
      INCSEQ(udp->next_seq);
      udp->missed_frames++;
    }

  /*
   * return next packet if available
   */

  while (udp->queued > 0 && udp->queue[udp->next_seq]) {
    buf_element_t       *buf = NULL;
    stream_udp_header_t *pkt = (stream_udp_header_t*)udp->queue[udp->next_seq]->content;
    udp->queue_input_pos = pkt->pos + udp->queue[udp->next_seq]->size - sizeof(stream_udp_header_t);
    if (udp->queue[udp->next_seq]->size > (ssize_t)sizeof(stream_udp_header_t))
      buf = udp->queue[udp->next_seq];
    else
      udp->queue[udp->next_seq]->free_buffer(udp->queue[udp->next_seq]);

    udp->queue[udp->next_seq] = NULL;
    udp->queued --;
    INCSEQ(udp->next_seq);

    if (udp->resend_requested)
      udp->resend_requested --;

    /* flush all packets when idle padding found */
    if (udp->is_padding && udp->queued > 0)
      while (!udp->queue[udp->next_seq]) {
        INCSEQ(udp->next_seq);
        udp->missed_frames++;
      }

    if (buf)
      return buf;
  }

  errno = EAGAIN;
  return NULL;
}

static void udp_process_resend(vdr_input_plugin_t *this)
{
  udp_data_t *udp = this->udp_data;

  /* no new resend requests until previous has been completed or failed */
  if (udp->resend_requested)
    return;

  /* If frames are missing, request re-send */
  if (NEXTSEQ(udp->current_seq) != udp->next_seq  &&  udp->queued) {

    int max_req = 20;

    while (!udp->queue[udp->current_seq] && --max_req > 0)
      INCSEQ(udp->current_seq);

    printf_control(this, "UDP RESEND %d-%d %" PRIu64 "\r\n",
                   udp->next_seq, PREVSEQ(udp->current_seq),
                   udp->queue_input_pos);

    udp->resend_requested =
      (udp->current_seq + (UDP_SEQ_MASK+1) - udp->next_seq) & UDP_SEQ_MASK;

    LOGUDP("%d-%d missing, requested re-send for %d frames",
           udp->next_seq, PREVSEQ(udp->current_seq), udp->resend_requested);
  }
}

/*
 * vdr_plugin_read_block_udp()
 *
 * - Get next UDP transport block from (socket)/queue.
 *
 * Returns NULL if read failed or data is not available.
 * (sets errno to EAGAIN, EINTR or ENOTCONN)
 *
 */
static buf_element_t *vdr_plugin_read_block_udp(vdr_input_plugin_t *this)
{
  udp_data_t *udp = this->udp_data;

  while (this->control_running && this->fd_data >= 0) {

    if (this->discard_index < this->discard_index_ds &&
        wait_stream_sync(this))
      return NULL;

    if (!this->control_running || this->fd_data < 0) {
      errno = ENOTCONN;
      return NULL;
    }

    buf_element_t *read_buffer;

    /* return next packet from reordering queue (if any) */
    if (NULL != (read_buffer = udp_process_queue(this)))
      return read_buffer;

    if (_x_action_pending(this->stream)) {
      errno = EINTR;
      return NULL;
    }

    /* poll + read socket */
    if ( ! (read_buffer = read_socket_udp(this)))
      return NULL;

    if (! (read_buffer = udp_parse_header(read_buffer, this->rtp)) ||
        ! (read_buffer = udp_parse_control(this, read_buffer))     ||
        ! (read_buffer = udp_check_packet(read_buffer))) {
      errno = EAGAIN;
      return NULL;
    }

    /*
     * handle re-ordering and retransmissios
     */

    stream_udp_header_t *pkt      = (stream_udp_header_t*)read_buffer->content;
    uint8_t             *pkt_data = UDP_PAYLOAD(read_buffer->content);

    udp->current_seq = pkt->seq & UDP_SEQ_MASK;
    udp->is_padding  = DATA_IS_PES(pkt_data) && IS_PADDING_PACKET(pkt_data);

    /* first received frame initializes sequence counter */
    if (udp->received_frames == -1) {
      udp->next_seq        = udp->current_seq;
      udp->received_frames = 0;
    }

    /* check if received sequence number is inside allowed window
       (half of whole range) */

    if (ADDSEQ(udp->current_seq, -udp->next_seq) > ((UDP_SEQ_MASK+1) >> 1)/*0x80*/) {
      struct sockaddr_in sin;
      LOGUDP("Received SeqNo out of window (%d ; [%d..%d])",
             udp->current_seq, udp->next_seq,
             (udp->next_seq+((UDP_SEQ_MASK+1) >> 1)/*0x80*/) & UDP_SEQ_MASK);
      /* reset link */
      LOGDBG("UDP: resetting link");
      memcpy(&sin, &udp->server_address, sizeof(sin));
      free_udp_data(udp);
      udp = this->udp_data = init_udp_data();
      memcpy(&udp->server_address, &sin, sizeof(sin));
      read_buffer->free_buffer(read_buffer);
      errno = EAGAIN;
      return NULL;
    }

    /* Add received frame to incoming queue */
    if (udp->queue[udp->current_seq]) {
      /* Duplicate packet or lot of dropped packets */
      LOGUDP("Got duplicate or window exceeded ? (queue slot %d in use) !",
             udp->current_seq);
      udp->queue[udp->current_seq]->free_buffer(udp->queue[udp->current_seq]);
      udp->queue[udp->current_seq] = NULL;
      if (!udp->queued)
        LOGERR("UDP queue corrupt !!!");
      else
        udp->queued--;
    }

    udp->queue[udp->current_seq] = read_buffer;
    read_buffer = NULL;
    udp->queued ++;

    /* return next packet from queue (if any) */
    if (NULL != (read_buffer = udp_process_queue(this)))
      return read_buffer;

    udp_process_resend(this);

#ifdef LOG_UDP
    /* Link quality statistics */
    udp->received_frames++;
    if (udp->received_frames >= 1000) {
      if (udp->missed_frames)
        LOGUDP("packet loss %d.%d%% (%4d/%4d)",
               udp->missed_frames*100/udp->received_frames,
               (udp->missed_frames*1000/udp->received_frames)%10,
               udp->missed_frames, udp->received_frames);
      udp->missed_frames = udp->received_frames = 0;
    }
#endif
  }

  errno = ENOTCONN;
  return NULL;
}

#ifdef TEST_PIP
static int write_slave_stream(vdr_input_plugin_t *this, int stream, const char *data, int len)
{
  fifo_input_plugin_t *slave;
  buf_element_t *buf;

  TRACE("write_slave_stream (%d bytes)", len); 

  if(!this->pip_stream) {
    LOGMSG("Detected new video stream 0x%X", (unsigned int)data[3]);
    LOGMSG("  no xine stream yet, trying to create ...");
    vdr_plugin_parse_control((input_plugin_t*)this, "SUBSTREAM 0xE1 50 50 288 196");
  }
  if(!this->pip_stream) {
    LOGMSG("  pip substream: no stream !");
    return -1;
  }
  /*LOGMSG("  pip substream open, queuing data");*/

  slave = (fifo_input_plugin_t*)this->pip_stream->input_plugin;  
  if(!slave) {
    LOGMSG("  pip substream: no input plugin !");
    return len;
  }

  if(slave->buffer_pool->num_free(slave->buffer_pool) < 20) {
    /*LOGMSG("  pip substream: fifo almost full !");*/
    xine_usec_sleep(3000);
    return 0;
  }
  buf = slave->buffer_pool->buffer_pool_try_alloc(slave->buffer_pool);
  if(!buf) {
    LOGMSG("  pip substream: fifo full !");
    return 0;
  }
  if(len > buf->max_size) {
    LOGMSG("  pip substream: buf too small");
    buf->free_buffer(buf);
    return len;
  }

  buf->content = buf->mem;
  buf->size = len;
  buf->type = BUF_DEMUX_BLOCK;
  xine_fast_memcpy(buf->content, data, len);
  slave->buffer->put(slave->buffer, buf);
  return len;
}
#endif

static int vdr_plugin_write(vdr_input_plugin_if_t *this_if, int stream, uint64_t pos, const char *data, int len)
{
  vdr_input_plugin_t *this = (vdr_input_plugin_t *) this_if;
  buf_element_t      *buf = NULL;

  if (this->slave.stream)
    return len;

#ifdef TEST_PIP
  if (stream)
    return write_slave_stream(this, stream, data, len);
#else
  if (stream)
    return len;
#endif

  TRACE("vdr_plugin_write (%d bytes)", len); 

  VDR_ENTRY_LOCK(0);

  buf = get_buf_element(this, len, 0);
  if(!buf) {
    /* need counter to filter non-fatal overflows
       (VDR is not polling for every PES packet) */
    if (this->write_overflows++ > 1)
      LOGMSG("vdr_plugin_write: buffer overflow ! (%d bytes)", len);
    VDR_ENTRY_UNLOCK();
    xine_usec_sleep(5*1000);
    errno = EAGAIN;
    return 0; /* EAGAIN */
  }
  this->write_overflows = 0;

  if(len > buf->max_size) {
    LOGMSG("vdr_plugin_write: PES too long (%d bytes, max size "
	   "%d bytes), data ignored !", len, buf->max_size);
    buf->free_buffer(buf);
/* curr_pos will be invalid when this point is reached ! */
    VDR_ENTRY_UNLOCK();
    return len;
  }

  stream_local_header_t *hdr = (stream_local_header_t*)buf->content;
  hdr->pos = pos;

  buf->type = BUF_LOCAL_BLOCK;
  buf->size = len + sizeof(stream_local_header_t);
  xine_fast_memcpy(buf->content + sizeof(stream_local_header_t), data, len);

  this->block_buffer->put(this->block_buffer, buf);

  VDR_ENTRY_UNLOCK();

  TRACE("vdr_plugin_write returns %d", len);

  return len;
}

/*
 * post_vdr_event()
 *
 * - Called by frontend
 * - forward (input) events to VDR
 *
 * It is safe to cancel thread while this function is being executed.
 */
static int post_vdr_event(vdr_input_plugin_if_t *this_if, const char *msg)
{
  vdr_input_plugin_t *this = (vdr_input_plugin_t *) this_if;

  if (msg && this->fd_control >= 0)
    return write_control (this, msg);

  LOGMSG("post_vdr_event: error ! \"%s\" not delivered.", msg ?: "<null>");
  return -1;
}


/******************************* Plugin **********************************/

#if XINE_VERSION_CODE < 10190
static off_t vdr_plugin_read (input_plugin_t *this_gen, char *buf_gen, off_t len)
#else
static off_t vdr_plugin_read (input_plugin_t *this_gen, void *buf_gen, off_t len)
#endif
{
  memset(buf_gen, 0, len);
  return 0;
}

/*
 * update_frames()
 *
 * Update frame type counters.
 * Collected information is used to start replay when enough data has been buffered
 */
static uint8_t update_frames(vdr_input_plugin_t *this, const uint8_t *data, int len)
{
  uint8_t type = pes_get_picture_type(data, len);

  if (!this->I_frames)
    this->P_frames = this->B_frames = 0;
  switch (type) {
    case I_FRAME: this->I_frames++; LOGSCR_VERBOSE("I"); break;
    case P_FRAME: this->P_frames++; LOGSCR_VERBOSE("P"); break;
    case B_FRAME: this->B_frames++; LOGSCR_VERBOSE("B"); break;
    default: break;
  }
  return type;
}

/*
 * Preprocess buffer before passing it to demux
 *  - handle discard
 *  - handle display blanking
 *  - handle stream start
 *  - strip network headers
 */
static buf_element_t *preprocess_buf(vdr_input_plugin_t *this, buf_element_t *buf)
{
  /* internal control bufs */
  if(buf->type == CONTROL_BUF_BLANK) {

    pthread_mutex_lock(&this->lock);
    if(!this->stream_start) {
      LOGMSG("BLANK in middle of stream! bufs queue %d , video_fifo %d", 
	     this->block_buffer->fifo_size,
	     this->stream->video_fifo->fifo_size);
    } else {
      vdr_x_demux_control_newpts(this->stream, 0, BUF_FLAG_SEEK);
      queue_blank_yv12(this);
    }
    pthread_mutex_unlock(&this->lock);

    buf->free_buffer(buf);
    return NULL;
  }

  /* demuxed video, control messages, ... go directly to demuxer */
  if (buf->type != BUF_NETWORK_BLOCK &&
      buf->type != BUF_LOCAL_BLOCK &&
      buf->type != BUF_DEMUX_BLOCK)
    return buf;

  pthread_mutex_lock(&this->lock);

  /* Update stream position and remove network headers */
  strip_network_headers(this, buf);

  /* Handle discard */
  if (this->discard_index > this->curpos && this->guard_index < this->curpos) {
    pthread_mutex_unlock(&this->lock);
    buf->free_buffer(buf);
    return NULL;
  }

  /* Update stream position */
  this->curpos += buf->size;
  this->curframe ++;

  /* ignore UDP/RTP "idle" padding */
  if (!DATA_IS_TS(buf->content) && IS_PADDING_PACKET(buf->content)) {
    pthread_mutex_unlock(&this->lock);
    return buf;
  }

  /* First packet ? */
  if (this->stream_start) {
    this->stream_start = 0;
    pthread_mutex_lock (&this->stream->first_frame_lock);
    this->stream->first_frame_flag = 2;
    pthread_mutex_unlock (&this->stream->first_frame_lock);

    memset(&this->scr_buf, 0, sizeof(this->scr_buf));

    this->scr->got_pcr(this->scr, -1);
  }

  /* live mode clock sync input */
  if (this->live_mode || (this->fd_control >= 0 && !this->fixed_scr)) {
    int64_t pcr = -1;

    if (DATA_IS_TS(buf->content) &&
	ts_get_pcr_n(buf->content, buf->size / TS_SIZE, &pcr) &&
	pcr >= 0) {

      this->scr->got_pcr(this->scr, pcr);
    }

    /* PES stream has no PCR, use audio pts for vdr-1.6.0 compability */
    if (IS_AUDIO_PACKET(buf->content)) {
      pcr = pes_get_pts(buf->content, buf->size);
      if (pcr > 0) {
        this->scr->got_pcr(this->scr, pcr);
      }
    }
  }

  pthread_mutex_unlock(&this->lock);
  return buf;
}

/*
 * Postprocess buffer before passing it to demuxer
 * - Signal new pts upstream after stream changes
 * - Special handling for still images
 * - Count video frames for SCR tuning
 * - Special handling for ffmpeg mpeg2 video decoder
 */
static void postprocess_buf(vdr_input_plugin_t *this, buf_element_t *buf, int need_pause)
{
  if (need_pause) {
    pthread_mutex_lock(&this->lock);

    scr_tuning_set_paused(this);

    pthread_mutex_unlock(&this->lock);
  }

  if (buf->type != BUF_DEMUX_BLOCK || DATA_IS_TS(buf->content))
    return;

#if 0
  /* this is done in demux */

  /* generated still images start with empty video PES, PTS = 0.
     Reset metronom pts so images will be displayed */
  if(this->still_mode && buf->size == 14) {
    int64_t pts = pes_get_pts(buf->content, buf->size);
    if(pts==0) {
      vdr_x_demux_control_newpts(this->stream, pts, BUF_FLAG_SEEK);
      /* delay frame 10ms (9000 ticks) */
      /*buf->content[12] = (uint8_t)((10*90) >> 7);*/
    }
  }
#endif

  /* Count video frames for SCR tuning algorithm */
  if(this->live_mode && this->I_frames < 4)
    if(IS_VIDEO_PACKET(buf->content) && buf->size > 32)
      update_frames(this, buf->content, buf->size);
}

static void handle_disconnect(vdr_input_plugin_t *this)
{
  LOGMSG("read_block: no data source, returning NULL");

  flush_all_fifos (this, 0);

  pthread_mutex_lock(&this->lock);

  reset_trick_speed(this);
  this->live_mode = 0;
  reset_scr_tuning(this);
  this->stream->emergency_brake = 1;

  this->control_running = 0;
  errno = ENOTCONN;

  pthread_mutex_unlock(&this->lock);
}

static int adjust_scr_speed(vdr_input_plugin_t *this)
{
  int need_pause = 0;

  if(pthread_mutex_lock(&this->lock)) {
    LOGERR("adjust_scr_speed: pthread_mutex_lock failed");
    return 0;
  }

  if( (!this->live_mode && (this->fd_control < 0 ||
			    this->fixed_scr)) ||
      this->slave.stream) {
    if(this->scr_tuning)
      reset_scr_tuning(this);
  } else {

    if(this->stream_start) {
      reset_scr_tuning(this);
      need_pause = 1;
    } else {
      vdr_adjust_realtime_speed(this);
    }
  }
  pthread_mutex_unlock(&this->lock); 

  return need_pause;
}

static buf_element_t *vdr_plugin_read_block (input_plugin_t *this_gen,
					     fifo_buffer_t *fifo, off_t todo) 
{
  vdr_input_plugin_t *this = (vdr_input_plugin_t *) this_gen;
  buf_element_t      *buf  = NULL;
  int                 need_pause = 0;

  TRACE("vdr_plugin_read_block");

  if (this->slave.stream || !this->config_ok) {
    if (!this->config_ok) {
      LOGDBG("read_block waiting for configuration data");
      xine_usec_sleep(100*1000);
    }
    xine_usec_sleep(50*1000);
    errno = EAGAIN;
    return NULL;
  }

  do {

    /* check for disconnection/termination */
    if (!this->funcs.push_input_write /* reading from socket */ &&
        !this->control_running) {
      /* disconnected ? */
      handle_disconnect(this);
      return NULL;
    }

    /* Return immediately if demux_action_pending flag is set */
    if (_x_action_pending(this->stream)) {
      errno = EINTR;
      return NULL;
    }

    /* adjust SCR speed */
    need_pause = adjust_scr_speed(this);

    /* get next buffer */
    if (this->funcs.push_input_write /* local mode */) {
      buf = fifo_buffer_timed_get(this->block_buffer, 100);
    } else {
      if ( !(buf= fifo_buffer_try_get(this->block_buffer))) {
        if (this->udp || this->rtp)
          buf = vdr_plugin_read_block_udp(this);
        else
          buf = vdr_plugin_read_block_tcp(this);
        if (!buf && errno != EAGAIN && errno != EINTR) {
          handle_disconnect(this);
          return NULL;
        }
      }
    }

    if (!buf) {
      if (!this->is_paused &&
          !this->still_mode &&
          !this->is_trickspeed &&
          !this->slave.stream &&
          this->stream->video_fifo->fifo_size <= 0) {

        this->read_timeouts++;
	if (this->read_timeouts > 80) {
	  LOGMSG("No data in 8 seconds, queuing no signal image");
	  queue_nosignal(this);
	  this->read_timeouts = 0;
	}
      } else {
	this->read_timeouts = 0;
      }
      errno = _x_action_pending(this->stream) ? EINTR : EAGAIN;
      return NULL;
    }
    this->read_timeouts = 0;

    buf = preprocess_buf(this, buf);

  } while (!buf);

  postprocess_buf(this, buf, need_pause);

  TRACE("vdr_plugin_read_block: return data, pos end = %" PRIu64, this->curpos);
  return buf;
}

static off_t vdr_plugin_seek (input_plugin_t *this_gen, off_t offset, int origin)
{
  return -1;
}

static off_t vdr_plugin_get_length (input_plugin_t *this_gen)
{
  return -1;
}

static uint32_t vdr_plugin_get_capabilities (input_plugin_t *this_gen)
{
  return
#ifdef INPUT_CAP_NOCACHE
    INPUT_CAP_NOCACHE |
#endif
    INPUT_CAP_BLOCK;
}

static uint32_t vdr_plugin_get_blocksize (input_plugin_t *this_gen)
{
  return 2048;
}

static off_t vdr_plugin_get_current_pos (input_plugin_t *this_gen)
{
  vdr_input_plugin_t *this = (vdr_input_plugin_t *) this_gen;
  return this->discard_index > this->curpos ? this->discard_index : this->curpos;
}

static void vdr_plugin_dispose (input_plugin_t *this_gen) 
{
  vdr_input_plugin_t *this = (vdr_input_plugin_t *) this_gen;
  int local;
  int fd = -1, fc = -1;

  if(!this_gen)
    return;

  LOGDBG("vdr_plugin_dispose");

  /* stop slave stream */
  if (this->slave.stream) {
    close_slave_stream(this);
  }

  if(this->fd_control>=0)
    write_control(this, "CLOSE\r\n");

  this->control_running = 0;

  local = !!this->funcs.push_input_write;
  memset(&this->funcs, 0, sizeof(this->funcs));

  /* shutdown sockets */
  if(!local) {
    struct linger {
      int l_onoff;    /* linger active */
      int l_linger;   /* how many seconds to linger for */
    } l = {0,0};

    fd = this->fd_data;
    fc = this->fd_control;

    if(fc >= 0) {
      LOGDBG("Shutdown control");
      setsockopt(fc, SOL_SOCKET, SO_LINGER, &l, sizeof(struct linger));
      shutdown(fc, SHUT_RDWR);
    }

    if(fd >= 0 && this->tcp) {
      LOGDBG("Shutdown data");
      setsockopt(fc, SOL_SOCKET, SO_LINGER, &l, sizeof(struct linger));
      shutdown(fd, SHUT_RDWR);
    }
  }

  /* threads */
  if(!local && this->threads_initialized) {
    void *p;
    LOGDBG("Cancel control thread ...");
    /*pthread_cancel(this->control_thread);*/
    pthread_join (this->control_thread, &p);
    LOGDBG("Threads joined");
  }

  /* event queue(s) and listener threads */
  LOGDBG("Disposing event queues");
  if (this->event_queue) {
    xine_event_dispose_queue (this->event_queue);
    this->event_queue = NULL;
  }

  pthread_cond_broadcast(&this->engine_flushed);
  while(pthread_cond_destroy(&this->engine_flushed) == EBUSY) {
    LOGMSG("engine_flushed signal busy !");
    pthread_cond_broadcast(&this->engine_flushed);
    xine_usec_sleep(10);
  }

  /* destroy mutexes (keep VDR out) */
  LOGDBG("Destroying mutexes");
  while(pthread_mutex_destroy(&this->vdr_entry_lock) == EBUSY) {
    LOGMSG("vdr_entry_lock busy ...");
    pthread_mutex_lock(&this->vdr_entry_lock);
    pthread_mutex_unlock(&this->vdr_entry_lock);
  }
  while(pthread_mutex_destroy(&this->lock) == EBUSY) {
    LOGMSG("lock busy ...");
    pthread_mutex_lock(&this->lock);
    pthread_mutex_unlock(&this->lock);
  }
  while(pthread_mutex_destroy(&this->fd_control_lock) == EBUSY) {
    LOGMSG("fd_control_lock busy ...");
    pthread_mutex_lock(&this->fd_control_lock);
    pthread_mutex_unlock(&this->fd_control_lock);
  }

  signal_buffer_pool_not_empty(this);
  signal_buffer_not_empty(this);

  /* close sockets */
  if(!local) {
    LOGDBG("Closing data connection");
    if(fd >= 0)
      if(close(fd))
	LOGERR("close(fd_data) failed");
    LOGDBG("Closing control connection");
    if(fc >= 0)
      if(close(fc))
	LOGERR("close(fd_control) failed");
    this->fd_data = this->fd_control = -1;
    LOGMSG("Connections closed.");
  }

  /* OSD */
  if (this->osd_manager) {
    this->osd_manager->dispose(this->osd_manager, this->stream);
    this->osd_manager = NULL;
  }

  /* restore video properties */
  if(this->video_properties_saved)
    set_video_properties(this, -1,-1,-1,-1,-1, -1, -1); /* restore defaults */

  signal_buffer_pool_not_empty(this);
  signal_buffer_not_empty(this);

  /* SCR */
  if (this->scr)
    this->scr->dispose(this->scr);

  /* metronom */
  if (this->metronom)
    this->metronom->dispose(this->metronom);

  free (this->mrl);

  if(this->udp_data)
    free_udp_data(this->udp_data);

  /* fifos */
  LOGDBG("Disposing fifos");

  /* need to get all buffer elements back before disposing own buffers ... */
  flush_all_fifos (this, 1);

  if (this->block_buffer)
    this->block_buffer->dispose(this->block_buffer);
  if (this->hd_buffer)
    this->hd_buffer->dispose(this->hd_buffer);

  free (this);
  LOGDBG("dispose done.");
}

#if XINE_VERSION_CODE > 10103
static const char* vdr_plugin_get_mrl (input_plugin_t *this_gen) 
#else
static char* vdr_plugin_get_mrl (input_plugin_t *this_gen) 
#endif
{
  vdr_input_plugin_t *this = (vdr_input_plugin_t *) this_gen;
  return this->mrl;
}

static int vdr_plugin_get_optional_data (input_plugin_t *this_gen,
					 void *data, int data_type)
{
#ifdef INPUT_OPTIONAL_DATA_DEMUXER
  if (data_type == INPUT_OPTIONAL_DATA_DEMUXER) {
    static const char demux_name[] = "xvdr";
    *((const char **)data) = demux_name;
    return INPUT_OPTIONAL_SUCCESS;
  }
#endif
  return INPUT_OPTIONAL_UNSUPPORTED;
}

static int vdr_plugin_open(input_plugin_t *this_gen)
{
  vdr_input_plugin_t *this = (vdr_input_plugin_t *) this_gen;
  xine_t *xine = this->class->xine;
  cfg_entry_t *e;

  this->event_queue = xine_event_new_queue (this->stream);
  xine_event_create_listener_thread (this->event_queue, vdr_event_cb, this);

  this->buffer_pool = this->stream->video_fifo;

  this->reserved_buffers = this->buffer_pool->buffer_pool_capacity - RADIO_MAX_BUFFERS;

  /* enable resample method */
  xine->config->update_num(xine->config, "audio.synchronization.av_sync_method", 1);

  /* register our own scr provider */
  this->scr = adjustable_scr_start(this->class->xine);
  if (!this->scr)
    LOGMSG("adjustable_scr_start() FAILED !");
  this->scr_live_sync = 1;

  this->scr_tuning = SCR_TUNING_OFF;
  this->curpos = 0;

  /* replace stream metronom */
  this->metronom = xvdr_metronom_init(this->stream);

  /* buffer */
  this->block_buffer = fifo_buffer_new(this->stream, 4, 0x10000+64); /* dummy buf to be used before first read and for big PES frames */
  
  /* OSD */
  this->osd_manager = init_osd_manager();

  /* sync */
  pthread_mutex_init (&this->lock, NULL);
  pthread_mutex_init (&this->vdr_entry_lock, NULL);
  pthread_mutex_init (&this->fd_control_lock, NULL);
  pthread_cond_init  (&this->engine_flushed, NULL);

  LOGDBG("xine_input_xvdr: revision %s", module_revision);

  if (this->class->num_buffers_hd != HD_BUF_NUM_BUFS)
    LOGMSG("Using non-default \"media." MRL_ID ".num_buffers_hd:%d\"", this->class->num_buffers_hd);

  /* check stream audio fifo size and issue a warning if too small */
  e = this->class->xine->config->lookup_entry(this->class->xine->config,
                                              "engine.buffers.audio_num_buffers");
  if (e && e->num_value < 500) {
    LOGMSG("WARNING: xine-engine setting \"engine.buffers.audio_num_buffers\":%d is "
           "too low for HD-playback! Please use values between 500-1000!", e->num_value);
  }

  return 1;
}

static int vdr_plugin_open_local (input_plugin_t *this_gen) 
{
  LOGDBG("vdr_plugin_open_local");
  return vdr_plugin_open(this_gen);
}

static void set_recv_buffer_size(int fd, unsigned max_buf)
{
  /* try to have larger receiving buffer */

  /*while(max_buf) {*/
  if(setsockopt(fd, SOL_SOCKET, SO_RCVBUF, &max_buf, sizeof(int)) < 0) {
    LOGERR("setsockopt(SO_RCVBUF,%d) failed", max_buf);
    /*max_buf >>= 1;*/
  } else {
    unsigned int tmp = 0, len = sizeof(int);;
    if(getsockopt(fd, SOL_SOCKET, SO_RCVBUF, &tmp, &len) < 0) {
      LOGERR("getsockopt(SO_RCVBUF,%d) failed", max_buf);
      /*max_buf >>= 1;*/
    } else if(tmp != 2*max_buf) {
      LOGDBG("setsockopt(SO_RCVBUF): got %d bytes", tmp);
      /*max_buf >>= 1;*/
    }
  }
  /*}*/
  max_buf = 256;
  /* not going to send anything, so shrink send buffer ... */
  setsockopt(fd, SOL_SOCKET, SO_SNDBUF, &max_buf, sizeof(int));
}

static int alloc_udp_data_socket(int firstport, int trycount, int *port)
{
  int fd, one = 1;
  struct sockaddr_in name;

  name.sin_family = AF_INET;
  name.sin_port   = htons(firstport);
  name.sin_addr.s_addr = htonl(INADDR_ANY);

  fd = socket(PF_INET, SOCK_DGRAM, 0/*IPPROTO_UDP*/);

  set_recv_buffer_size(fd, KILOBYTE(512));

  if(setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(int)) < 0)
    LOGERR("UDP data stream: setsockopt(SO_REUSEADDR) failed");

  while(bind(fd, (struct sockaddr *)&name, sizeof(name)) < 0) {
    if(!--trycount) {
      LOGMSG("UDP Data stream: bind error, no free port found");
      close(fd);
      return -1;
    }
    LOGERR("UDP Data stream: bind error, port %d: %s",
	   name.sin_port, strerror(errno));
    name.sin_port = htons(++firstport);
  }

  *port = ntohs(name.sin_port);
  return fd;
}

static int connect_control_stream(vdr_input_plugin_t *this, const char *host, 
				  int port, int *client_id)
{
  char tmpbuf[256];
  int fd_control;
  int saved_fd = this->fd_control, one = 1;

  /* Connect to server */
  this->fd_control = fd_control = _x_io_tcp_connect(this->stream, host, port);

  if(fd_control < 0 || 
     XIO_READY != _x_io_tcp_connect_finish(this->stream, this->fd_control, 
					   3000)) {
    LOGERR("Can't connect to tcp://%s:%d", host, port);
    close(fd_control);
    this->fd_control = saved_fd;
    return -1;
  }

  set_recv_buffer_size(fd_control, KILOBYTE(128));

  /* request control connection */
  if(_x_io_tcp_write(this->stream, fd_control, "CONTROL\r\n", 9) < 0) {
    LOGERR("Control stream write error");
    return -1;
  }

  /* Check server greeting */
  if(readline_control(this, tmpbuf, sizeof(tmpbuf)-1, 4) <= 0) {
    LOGMSG("Server not replying");
    close(fd_control);
    this->fd_control = saved_fd;
    return -1;
  }
  LOGMSG("Server greeting: %s", tmpbuf);
  if(!strncmp(tmpbuf, "Access denied", 13)) {
    LOGMSG("Maybe host address is missing from server-side plugins/xineliboutput/allowed_hosts.conf ?");
    close(fd_control);
    this->fd_control = saved_fd;
    return -1;
  }
  if(!strstr(tmpbuf, "VDR-") || !strstr(tmpbuf, "xineliboutput-") || !strstr(tmpbuf, "READY")) {
    LOGMSG("Unregonized greeting !");
    close(fd_control);
    this->fd_control = saved_fd;
    return -1;
  }
  /* Check server xineliboutput version */
  if(!strstr(tmpbuf, "xineliboutput-" XINELIBOUTPUT_VERSION " ")) {
    LOGMSG("-----------------------------------------------------------------");
    LOGMSG("WARNING: Client and server versions of xinelibout are different !");
    LOGMSG("         Client version (xine_input_vdr.so) is " XINELIBOUTPUT_VERSION);
    LOGMSG("-----------------------------------------------------------------");
  }

  /* Store our client-id */
  if(readline_control(this, tmpbuf, sizeof(tmpbuf)-1, 4) > 0 &&
     !strncmp(tmpbuf, "CLIENT-ID ", 10)) {
    LOGDBG("Got Client-ID: %s", tmpbuf+10);
    if(client_id)
      if(1 != sscanf(tmpbuf+10, "%d", client_id))
	*client_id = -1;
  } else {
    LOGMSG("Warning: No Client-ID !");
    if(*client_id)
      *client_id = -1;
  }

  /* set socket to non-blocking mode */
  fcntl (fd_control, F_SETFL, fcntl (fd_control, F_GETFL) | O_NONBLOCK);

  /* set control socket to deliver data immediately 
     instead of waiting for full TCP segments */
  setsockopt(fd_control, IPPROTO_TCP, TCP_NODELAY, &one, sizeof(int));

  this->fd_control = saved_fd;
  return fd_control;
}


static int connect_rtp_data_stream(vdr_input_plugin_t *this)
{
  char cmd[256];
  unsigned int ip0, ip1, ip2, ip3, port;
  int fd=-1, one = 1, retries = 0;
  struct sockaddr_in multicastAddress;
  struct ip_mreq mreq;
  struct sockaddr_in server_address, sin;
  socklen_t len = sizeof(sin);
  stream_rtp_header_impl_t tmp_rtp;

  /* get server IP address */
  if(getpeername(this->fd_control, (struct sockaddr *)&server_address, &len)) {
    LOGERR("getpeername(fd_control) failed");
    /* close(fd); */
    return -1;
  }

  /* request RTP data transport from server */

  LOGDBG("Requesting RTP transport");
  if(_x_io_tcp_write(this->stream, this->fd_control, "RTP\r\n", 5) < 0) {
    LOGERR("Control stream write error");
    return -1;
  }

  cmd[0] = 0;
  if(readline_control(this, cmd, sizeof(cmd)-1, 4) < 8 ||
     strncmp(cmd, "RTP ", 4)) {
    LOGMSG("Server does not support RTP ? (%s)", cmd);
    return -1;
  }

  LOGDBG("Got: %s", cmd);
  if(5 != sscanf(cmd, "RTP %u.%u.%u.%u:%u", &ip0, &ip1, &ip2, &ip3, &port) ||
     ip0>0xff || ip1>0xff || ip2>0xff || ip3>0xff || port>0xffff) {
    LOGMSG("Server does not support RTP ? (%s)", cmd);
    return -1;
  }
  
  LOGMSG("Connecting (data) to rtp://@%u.%u.%u.%u:%u ...", 
	 ip0, ip1, ip2, ip3, port);
  multicastAddress.sin_family = AF_INET;
  multicastAddress.sin_port   = htons(port);
  multicastAddress.sin_addr.s_addr = htonl((ip0<<24)|(ip1<<16)|(ip2<<8)|ip3);

  if((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    LOGERR("socket() failed");
    return -1;
  }
  set_recv_buffer_size(fd, KILOBYTE(512));

  if(setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(int)) < 0) {
    LOGERR("setsockopt(SO_REUSEADDR) failed");
    close(fd);
    return -1;
  }

  if(bind(fd, (struct sockaddr *)&multicastAddress, 
	  sizeof(multicastAddress)) < 0) {
    LOGERR("bind() to multicast address failed");
    close(fd);
    return -1;
  }

  /* Join to multicast group */

  memset(&mreq, 0, sizeof(mreq));
  mreq.imr_multiaddr.s_addr = multicastAddress.sin_addr.s_addr;
  mreq.imr_interface.s_addr = htonl(INADDR_ANY);
  /*mreq.imr_ifindex = 0;*/
  if(setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq))) {
    LOGERR("setsockopt(IP_ADD_MEMBERSHIP) failed. No multicast in kernel?");
    close(fd);
    return -1;
  }

retry_select:

  /* wait until server sends first RTP packet */

  if( XIO_READY != io_select_rd(fd)) {
    LOGDBG("Requesting RTP transport: RTP poll timeout");
    if(++retries < 10) {
      LOGDBG("Requesting RTP transport");
      if(_x_io_tcp_write(this->stream, this->fd_control, "RTP\r\n", 5) < 0) {
	LOGERR("Control stream write error");
	close(fd);
	return -1;
      }
      goto retry_select;
    }
    LOGMSG("Data stream connection timed out (RTP)");
    close(fd);
    return -1;
  }

retry_recvfrom:

  /* check sender address */

  if (recvfrom(fd, &tmp_rtp, sizeof(tmp_rtp), 0, &sin, &len) < 0) {
    LOGERR("RTP recvrom() failed");
    return -1;
  }
  if(sin.sin_addr.s_addr != server_address.sin_addr.s_addr) {
    uint32_t tmp_ip = ntohl(sin.sin_addr.s_addr);
    LOGMSG("Received UDP/RTP multicast from unknown sender: %d.%d.%d.%d:%d",
	   ((tmp_ip>>24)&0xff), ((tmp_ip>>16)&0xff), 
	   ((tmp_ip>>8)&0xff), ((tmp_ip)&0xff), 
	   sin.sin_port);

    if(XIO_READY == _x_io_select(this->stream, fd, XIO_READ_READY, 0))
      goto retry_recvfrom;
    if(++retries < 4)
      goto retry_select;
    close(fd);
    return -1;
  }

  /* Succeed */

  this->udp_data = init_udp_data();

  /* store server address */
  memcpy(&this->udp_data->server_address, &sin, sizeof(sin));
  this->udp_data->ssrc = tmp_rtp.rtp_hdr.ssrc;
  
  return fd;
}


static int connect_udp_data_stream(vdr_input_plugin_t *this)
{
  char cmd[256];
  struct sockaddr_in server_address, sin;
  socklen_t len = sizeof(sin);
  uint32_t  tmp_ip;
  stream_udp_header_t tmp_udp;
  int retries = 0, port = -1, fd = -1;

  /* get server IP address */
  if(getpeername(this->fd_control, (struct sockaddr *)&server_address, &len)) {
    LOGERR("getpeername(fd_control) failed");
    /* close(fd); */
    return -1;
  }
  tmp_ip = ntohl(server_address.sin_addr.s_addr);

  LOGDBG("VDR server address: %d.%d.%d.%d", 
	 ((tmp_ip>>24)&0xff), ((tmp_ip>>16)&0xff), 
	 ((tmp_ip>>8)&0xff), ((tmp_ip)&0xff));

  /* allocate UDP socket */
  if((fd = alloc_udp_data_socket(DEFAULT_VDR_PORT, 20, &port)) < 0)
    return -1;
  /*LOGDBG("my UDP port is: %d", port);*/

retry_request:

  /* request UDP data transport from server */

  LOGDBG("Requesting UDP transport");
  sprintf(cmd, "UDP %d\r\n", port);
  if(_x_io_tcp_write(this->stream, this->fd_control, cmd, strlen(cmd)) < 0) {
    LOGERR("Control stream write error");
    close(fd);
    return -1;
  }

  cmd[0] = 0;
  if(readline_control(this, cmd, sizeof(cmd)-1, 4) < 6 ||
     strncmp(cmd, "UDP OK", 6)) {
    LOGMSG("Server does not support UDP ? (%s)", cmd);
    return -1;
  }

retry_select:

  /* wait until server sends first UDP packet */

  if( XIO_READY != io_select_rd(fd)) {
    LOGDBG("Requesting UDP transport: UDP poll timeout");
    if(++retries < 4)
      goto retry_request;
    LOGMSG("Data stream connection timed out (UDP)");
    close(fd);
    return -1;
  }

retry_recvfrom:

  /* check sender address */

  if (recvfrom(fd, &tmp_udp, sizeof(tmp_udp), 0, &sin, &len) < 0) {
    LOGERR("UDP recvrom() failed");
    return -1;
  }
  if(sin.sin_addr.s_addr != server_address.sin_addr.s_addr) {
    tmp_ip = ntohl(sin.sin_addr.s_addr);
    LOGMSG("Received UDP packet from unknown sender: %d.%d.%d.%d:%d",
	   ((tmp_ip>>24)&0xff), ((tmp_ip>>16)&0xff),
	   ((tmp_ip>>8)&0xff), ((tmp_ip)&0xff),
	   sin.sin_port);

    if(XIO_READY == _x_io_select(this->stream, fd, XIO_READ_READY, 0))
      goto retry_recvfrom;
    if(++retries < 4)
      goto retry_select;
    close(fd);
    return -1;
  }

  /* Succeed */

  this->udp_data = init_udp_data();

  /* store server address */
  memcpy(&this->udp_data->server_address, &sin, sizeof(sin));

  return fd;
}

static int connect_tcp_data_stream(vdr_input_plugin_t *this, const char *host, 
				   int port)
{
  static const char ackmsg[] = {'D','A','T','A','\r','\n'};
  struct sockaddr_in sinc;
  socklen_t len = sizeof(sinc);
  uint32_t ipc;
  char tmpbuf[256];
  int fd_data, n;

  /* Connect to server */
  fd_data = _x_io_tcp_connect(this->stream, host, port);

  if(fd_data < 0 || 
     XIO_READY != _x_io_tcp_connect_finish(this->stream, fd_data, 3000)) {
    LOGERR("Can't connect to tcp://%s:%d", host, port);
    close(fd_data);
    return -1;
  }

  set_recv_buffer_size(fd_data, KILOBYTE(128));

  /* request data connection */

  getsockname(this->fd_control, (struct sockaddr *)&sinc, &len);
  ipc = ntohl(sinc.sin_addr.s_addr);
  sprintf(tmpbuf, 
	  "DATA %d 0x%x:%u %d.%d.%d.%d\r\n", 
	  this->client_id, 
	  (unsigned int)ipc,
	  (unsigned int)ntohs(sinc.sin_port),
	  ((ipc>>24)&0xff), ((ipc>>16)&0xff), ((ipc>>8)&0xff), ((ipc)&0xff)
	  );
  if(_x_io_tcp_write(this->stream, fd_data, tmpbuf, strlen(tmpbuf)) < 0) {
    LOGERR("Data stream write error (TCP)");
  } else if( XIO_READY != io_select_rd(fd_data)) {
    LOGERR("Data stream poll failed (TCP)");
  } else if((n=read(fd_data, tmpbuf, sizeof(ackmsg))) <= 0) {
    LOGERR("Data stream read failed (TCP)");
  } else if(n<sizeof(ackmsg) || strncmp(tmpbuf, ackmsg, sizeof(ackmsg))) {
    tmpbuf[n] = 0;
    LOGMSG("Server does not support TCP ? (%s)", tmpbuf);
  } else {
    /* succeed */
    /* set socket to non-blocking mode */
    fcntl (fd_data, F_SETFL, fcntl (fd_data, F_GETFL) | O_NONBLOCK);
    return fd_data;
  }

  close(fd_data);
  return -1;
}

static int connect_pipe_data_stream(vdr_input_plugin_t *this)
{
  char tmpbuf[256];
  int fd_data = -1;

  /* check if IP address matches */
  if(!strstr(this->mrl, "127.0.0.1")) {
    struct sockaddr_in sinc;
    struct sockaddr_in sins;
    socklen_t len = sizeof(sinc);
    getsockname(this->fd_control, &sinc, &len);
    getpeername(this->fd_control, &sins, &len);
    if(sinc.sin_addr.s_addr != sins.sin_addr.s_addr) {
      LOGMSG("connect_pipe_data_stream: client ip=0x%x != server ip=0x%x !",
	     (unsigned int)sinc.sin_addr.s_addr, (unsigned int)sins.sin_addr.s_addr);
#if 0
      return -1;
#endif
    }
  }

  _x_io_tcp_write(this->stream, this->fd_control, "PIPE\r\n", 6);

  if(readline_control(this, tmpbuf, sizeof(tmpbuf), 4) <= 0) {
    LOGMSG("Pipe request failed");
  } else if(strncmp(tmpbuf, "PIPE /", 6)) {
    LOGMSG("Server does not support pipes ? (%s)", tmpbuf);
  } else {

    LOGMSG("Connecting (data) to pipe://%s", tmpbuf+5);
    if((fd_data = open(tmpbuf+5, O_RDONLY|O_NONBLOCK)) < 0) {
      if(errno == ENOENT)
	LOGMSG("Pipe not found");
      else
	LOGERR("Pipe opening failed");
    } else {
      _x_io_tcp_write(this->stream, this->fd_control, "PIPE OPEN\r\n", 11);
      if(readline_control(this, tmpbuf, sizeof(tmpbuf)-1, 4) >6 &&
	 !strncmp(tmpbuf, "PIPE OK", 7)) {
	fcntl (fd_data, F_SETFL, fcntl (fd_data, F_GETFL) | O_NONBLOCK);
	return fd_data;
      }
      LOGMSG("Data stream connection failed (PIPE)");
    } 
  }

  close(fd_data);
  return -1;
}

static int vdr_plugin_open_net (input_plugin_t *this_gen) 
{
  vdr_input_plugin_t *this = (vdr_input_plugin_t *) this_gen;
  char tmpbuf[256];
  int err;

  LOGDBG("vdr_plugin_open_net %s", this->mrl);

  if(strchr(this->mrl, '#')) 
    *strchr(this->mrl, '#') = 0;
  if((!strncasecmp(this->mrl, MRL_ID "+tcp://",  MRL_ID_LEN+7) && (this->tcp=1)) ||
     (!strncasecmp(this->mrl, MRL_ID "+udp://",  MRL_ID_LEN+7) && (this->udp=1)) ||
     (!strncasecmp(this->mrl, MRL_ID "+rtp://",  MRL_ID_LEN+7) && (this->rtp=1)) ||
     (!strncasecmp(this->mrl, MRL_ID "+pipe://", MRL_ID_LEN+8)) ||
     (!strncasecmp(this->mrl, MRL_ID "://",      MRL_ID_LEN+3))) {

    char *phost = strdup(strstr(this->mrl, "//") + 2);
    char host[256];
    char *port = strchr(phost, ':');
    int iport;
    int one = 1;
    if(port) *port++ = 0;
    iport = port ? atoi(port) : DEFAULT_VDR_PORT;
    strn0cpy(host, phost, 254);
    /*host[sizeof(host)-1] = 0;*/
    free(phost);
    /* TODO: use multiple input plugins - tcp/udp/file */

    /* connect control stream */

    LOGMSG("Connecting (control) to tcp://%s:%d ...", host, iport);
    this->fd_control = connect_control_stream(this, host, iport,
					      &this->client_id);
    if (this->fd_control < 0) {
      LOGERR("Can't connect to tcp://%s:%d", host, iport);
      return 0;
    }
    setsockopt(this->fd_control, IPPROTO_TCP, TCP_NODELAY, &one, sizeof(int));

    LOGMSG("Connected (control) to tcp://%s:%d", host, iport);

    /* connect data stream */

    /* try pipe ? */

    if(!this->tcp && !this->udp && !this->rtp) {
      if((this->fd_data = connect_pipe_data_stream(this)) < 0) {
	LOGMSG("Data stream connection failed (PIPE)");
      } else {
	this->tcp = this->udp = this->rtp = 0;
	LOGMSG("Data stream connected (PIPE)");
      }
    }

    /* try RTP ? */

    if(this->fd_data < 0 && this->rtp) {
      /* flush control buffer (if PIPE was tried first) */
      while(0 < read(this->fd_control, tmpbuf, 255)) ;
      if((this->fd_data = connect_rtp_data_stream(this)) < 0) {
	LOGMSG("Data stream connection failed (RTP)");
	this->rtp = 0;
      } else {
	this->rtp = 1;
	this->tcp = this->udp = 0;
	LOGMSG("Data stream connected (RTP)");
      }
    }

    /* try UDP ? */

    if(this->fd_data < 0 && !this->tcp) {
      LOGMSG("Connecting (data) to udp://%s ...", host);
      /* flush control buffer (if RTP was tried first) */
      while(0 < read(this->fd_control, tmpbuf, 255)) ;
      if((this->fd_data = connect_udp_data_stream(this)) < 0) {
	LOGMSG("Data stream connection failed (UDP)");
	this->udp = 0;
      } else {
	this->udp = 1;
	this->tcp = this->rtp = 0;
	LOGMSG("Data stream connected (UDP)");
      }
    }

    /* fall back to TCP ? */

    if(this->fd_data < 0) {
      LOGMSG("Connecting (data) to tcp://%s:%d ...", host, iport);
      this->tcp = 0;
      if((this->fd_data = connect_tcp_data_stream(this, host, iport)) < 0) {
	LOGMSG("Data stream connection failed (TCP)");
	this->tcp = 0;
      } else {
	this->tcp = 1;
      }
      if(this->tcp) {
	/* succeed */
	this->rtp = this->udp = 0;
	LOGMSG("Data stream connected (TCP)");
      } else {
	/* failed */
	close(this->fd_data);
	close(this->fd_control);
	this->fd_control = this->fd_data = -1;
	return 0;
      }
    }
    
  } else {
    LOGMSG("Unknown mrl (%s)", this->mrl);
    return 0;
  }

  if(!vdr_plugin_open(this_gen))
    return 0;

  queue_nosignal(this);

  this->control_running = 1;
  if ((err = pthread_create (&this->control_thread,
			     NULL, vdr_control_thread, (void*)this)) != 0) {
    LOGERR("Can't create new thread");
    return 0;
  }

  this->class->xine->port_ticket->acquire(this->class->xine->port_ticket, 1);
  if(!(this->stream->video_out->get_capabilities(this->stream->video_out) &
       VO_CAP_UNSCALED_OVERLAY))
    LOGMSG("WARNING: Video output driver reports it does not support unscaled overlays !");
  this->class->xine->port_ticket->release(this->class->xine->port_ticket, 1);

  this->threads_initialized = 1;
  return 1;
}

/**************************** Plugin class *******************************/
/* Callback on default mrl change */
static void vdr_class_default_mrl_change_cb(void *data, xine_cfg_entry_t *cfg) 
{
  vdr_input_class_t *class = (vdr_input_class_t *) data;

  class->mrls[0] = cfg->str_value;
} 

/* callback on scr tuning step change */
static void vdr_class_scr_tuning_step_cb(void *data, xine_cfg_entry_t *cfg) 
{
  vdr_input_class_t *class = (vdr_input_class_t *) data;

  class->scr_tuning_step = cfg->num_value / 1000000.0;
}

/* callback on scr tuning mode change */
static void vdr_class_smooth_scr_tuning_cb(void *data, xine_cfg_entry_t *cfg)
{
    vdr_input_class_t *class = (vdr_input_class_t *) data;

    class->smooth_scr_tuning = cfg->num_value;
}

/* callback on OSD scaling mode change */
static void vdr_class_fast_osd_scaling_cb(void *data, xine_cfg_entry_t *cfg) 
{
  vdr_input_class_t *class = (vdr_input_class_t *) data;

  class->fast_osd_scaling = cfg->num_value;
}

static input_plugin_t *vdr_class_get_instance (input_class_t *class_gen,
				    xine_stream_t *stream,
				    const char *data) 
{
  vdr_input_class_t  *class = (vdr_input_class_t *) class_gen;
  vdr_input_plugin_t *this;
  char               *mrl = (char *) data;
  int                local_mode;

  LOGDBG("vdr_class_get_instance");

  if (strncasecmp (mrl, MRL_ID ":", MRL_ID_LEN+1) && 
      strncasecmp (mrl, MRL_ID "+", MRL_ID_LEN+1))
    return NULL;

  if(!strncasecmp(mrl, MRL_ID "+slave://0x", MRL_ID_LEN+11)) {
    LOGMSG("vdr_class_get_instance: slave stream requested");
    return fifo_class_get_instance(class_gen, stream, data);
  }

  this = calloc(1, sizeof(vdr_input_plugin_t));

  this->stream       = stream;
  this->mrl          = strdup(mrl); 
  this->class        = class;

  this->fd_data      = -1;
  this->fd_control   = -1;

  this->stream_start = 1;
  this->autoplay_size = -1;

  local_mode         = ( (!strncasecmp(mrl, MRL_ID "://", MRL_ID_LEN+3)) && 
			 (strlen(mrl)==7))
                       || (!strncasecmp(mrl, MRL_ID ":///", MRL_ID_LEN+4));

  if(!bSymbolsFound) {
    /* not running under VDR or vdr-sxfe/vdr-fbfe */
    if(local_mode) {
      LOGDBG("vdr or vdr-??fe not detected, forcing remote mode");
      local_mode = 0;
    }
    if(!strcasecmp(mrl, MRL_ID ":") ||
       !strcasecmp(mrl, MRL_ID ":/") ||
       !strcasecmp(mrl, MRL_ID "://") ||
       !strcasecmp(mrl, MRL_ID ":///")) {
      /* default to local host */
      free(this->mrl);
      this->mrl = strdup(MRL_ID "://127.0.0.1");
      LOGMSG("Changed mrl from %s to %s", mrl, this->mrl);
    }
  }

  this->input_plugin.open = local_mode ? vdr_plugin_open_local 
                                       : vdr_plugin_open_net;
  this->input_plugin.get_mrl           = vdr_plugin_get_mrl;
  this->input_plugin.dispose           = vdr_plugin_dispose;
  this->input_plugin.input_class       = class_gen;

  this->input_plugin.get_capabilities  = vdr_plugin_get_capabilities;
  this->input_plugin.read              = vdr_plugin_read;
  this->input_plugin.read_block        = vdr_plugin_read_block;
  this->input_plugin.seek              = vdr_plugin_seek;
  this->input_plugin.get_current_pos   = vdr_plugin_get_current_pos;
  this->input_plugin.get_length        = vdr_plugin_get_length;
  this->input_plugin.get_blocksize     = vdr_plugin_get_blocksize;
  this->input_plugin.get_optional_data = vdr_plugin_get_optional_data;
  
  if(local_mode) {
    this->funcs.push_input_write  = vdr_plugin_write;
    this->funcs.push_input_control= vdr_plugin_parse_control;
    this->funcs.push_input_osd    = vdr_plugin_exec_osd_command;
    /*this->funcs.xine_input_event= NULL; -- frontend sets this */
  } else {
    this->funcs.post_vdr_event    = post_vdr_event;
  }
  
  LOGDBG("vdr_class_get_instance done.");
  return &this->input_plugin;
}

/*
 * vdr input plugin class stuff
 */

#if INPUT_PLUGIN_IFACE_VERSION < 18
#if XINE_VERSION_CODE > 10103
static const char *vdr_class_get_description (input_class_t *this_gen) 
#else
static char *vdr_class_get_description (input_class_t *this_gen) 
#endif
{
  return _("VDR (Video Disk Recorder) input plugin");
}

static const char *vdr_class_get_identifier (input_class_t *this_gen) 
{
  return MRL_ID;
}
#endif

#if XINE_VERSION_CODE >= 10200
static const char * const *vdr_plugin_get_autoplay_list(input_class_t *this_gen, int *num_files) 
{
  vdr_input_class_t *this = (vdr_input_class_t *)this_gen;
  *num_files = 1;

  return (const char * const *)this->mrls;
}
#else
static char **vdr_plugin_get_autoplay_list(input_class_t *this_gen, int *num_files) 
{
  vdr_input_class_t *this = (vdr_input_class_t *)this_gen;
  *num_files = 1;

  return this->mrls;
}
#endif

static void vdr_class_dispose (input_class_t *this_gen) 
{
  vdr_input_class_t *this = (vdr_input_class_t *) this_gen;
  config_values_t *config = this->xine->config;

  config->unregister_callback(config, "media." MRL_ID ".default_mrl");
  config->unregister_callback(config, "media." MRL_ID ".osd.fast_scaling");
  config->unregister_callback(config, "media." MRL_ID ".scr_tuning_step");
  config->unregister_callback(config, "media." MRL_ID ".smooth_scr_tuning");
  free (this);
}

static void *input_xvdr_init_class (xine_t *xine, void *data)
{
  vdr_input_class_t  *this;
  config_values_t     *config = xine->config;

  SetupLogLevel();

  if(!bSymbolsFound) {
    if(xine->verbosity > 0) {
      iSysLogLevel = xine->verbosity + 1;
      LOGMSG("detected verbose logging xine->verbosity=%d, setting log level to %d:%s",
	     xine->verbosity, iSysLogLevel, 
	     (iSysLogLevel < 1) ? "NONE" :
	     (iSysLogLevel < 2) ? "ERRORS" : 
	     (iSysLogLevel < 3) ? "INFO" :
	     (iSysLogLevel < 4) ? "DEBUG" :
	     "VERBOSE DEBUG");
    }
  }

  this = calloc(1, sizeof (vdr_input_class_t));

  this->xine   = xine;
  
  this->mrls[ 0 ] = config->register_string(config,                 
					    "media." MRL_ID ".default_mrl",
                                            MRL_ID "://127.0.0.1#nocache;demux:mpeg_block",
                                            _("default VDR host"),
                                            _("The default VDR host"),
                                            10, vdr_class_default_mrl_change_cb, (void *)this);
  this->mrls[ 1 ] = 0;

  this->fast_osd_scaling = config->register_bool(config,
						 "media." MRL_ID ".fast_osd_scaling", 0,
						 _("Fast (low-quality) OSD scaling"),
						 _("Enable fast (lower quality) OSD scaling.\n"
						   "Default is to use (slow) linear interpolation "
						   "to calculate pixels and full palette re-allocation "
						   "to optimize color palette.\n"
						   "Fast method only duplicates/removes rows and columns "
						   "and does not modify palette."),
						 10, vdr_class_fast_osd_scaling_cb, 
						 (void *)this);

  this->scr_tuning_step = config->register_num(config,
					       "media." MRL_ID ".scr_tuning_step", 5000,
					       _("SRC tuning step"),
					       _("SCR tuning step width unit %1000000."),
					       10, vdr_class_scr_tuning_step_cb, 
					       (void *)this) / 1000000.0;

  this->smooth_scr_tuning = config->register_bool(config,
                                                  "media." MRL_ID ".smooth_scr_tuning", 0,
                                                  _("Smoother SRC tuning"),
                                                  _("Smoother SCR tuning"),
                                                  10, vdr_class_smooth_scr_tuning_cb,
                                                  (void *)this);

  this->num_buffers_hd = config->register_num(config,
                                              "media." MRL_ID ".num_buffers_hd", HD_BUF_NUM_BUFS,
                                              _("number of buffers for HD content"),
                                              _("number of buffers for HD content"),
                                              10, NULL, NULL);

  this->scr_treshold_sd = config->register_num(config,
                                              "media." MRL_ID ".scr_treshold_sd", 50,
                                              _("SCR-Treshold for SD-Playback (%)"),
                                              _("SCR-Treshold for starting SD-Playback (%)"),
                                              10, NULL, NULL);

  this->scr_treshold_hd = config->register_num(config,
                                              "media." MRL_ID ".scr_treshold_hd", 40,
                                              _("SCR-Treshold for HD-Playback (%)"),
                                              _("SCR-Treshold for starting HD-Playback (%)"),
                                              10, NULL, NULL);

  this->input_class.get_instance       = vdr_class_get_instance;
#if INPUT_PLUGIN_IFACE_VERSION < 18
  this->input_class.get_identifier     = vdr_class_get_identifier;
  this->input_class.get_description    = vdr_class_get_description;
#else
  this->input_class.identifier         = MRL_ID;
  this->input_class.description        = N_("VDR (Video Disk Recorder) input plugin");
#endif
  this->input_class.get_autoplay_list  = vdr_plugin_get_autoplay_list;
  this->input_class.dispose            = vdr_class_dispose;

  LOGDBG("init class succeeded");

  return this;
}

/*
 * demuxer (xine/demux_xvdr.c)
 */

void *demux_xvdr_init_class (xine_t *xine, void *data);

static const demuxer_info_t demux_info_xvdr = {
  100                      /* priority */
};

/*
 * exported plugin catalog entry
 */

const plugin_info_t xine_plugin_info[] __attribute__((visibility("default"))) = {
  /* type, API, "name", version, special_info, init_function */
  { PLUGIN_INPUT, INPUT_PLUGIN_IFACE_VERSION,   MRL_ID, XINE_VERSION_CODE, NULL,             input_xvdr_init_class },

  { PLUGIN_DEMUX, DEMUXER_PLUGIN_IFACE_VERSION, MRL_ID, XINE_VERSION_CODE, &demux_info_xvdr, demux_xvdr_init_class },

  { PLUGIN_NONE, 0, "", 0, NULL, NULL }
};

const plugin_info_t *xine_plugin_info_xvdr = xine_plugin_info;


