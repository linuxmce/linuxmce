/*
 * xine_input_vdr.c: xine VDR input plugin
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: xine_input_vdr.c,v 1.138.2.15 2009/02/12 12:41:02 phintuka Exp $
 *
 */


#define XINE_ENGINE_INTERNAL
#define METRONOM_CLOCK_INTERNAL

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

#ifndef __APPLE__
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

#if XINE_VERSION_CODE >= 10190
# include <libavutil/mem.h>
#endif
#ifndef XINE_VERSION_CODE 
# error XINE_VERSION_CODE undefined !
#endif

#include "xine_input_vdr.h"
#include "xine_input_vdr_net.h"
#include "xine_osd_command.h"

/***************************** DEFINES *********************************/

/* Support for ffmpeg mpeg2 decoder. 
   Priority must be increased in $HOME/.xine/config_xineliboutput:
       engine.decoder_priorities.ffmpegvideo:1
*/
#define FFMPEG_DEC

/*#define LOG_UDP*/
/*#define LOG_OSD*/
/*#define LOG_CMD*/
/*#define LOG_SCR*/
/*#define LOG_TRACE*/

#define ADJUST_SCR_SPEED        1
#define METRONOM_PREBUFFER_VAL  (4 * 90000 / 25 )
#define HD_BUF_NUM_BUFS         (2048)  /* 2k payload * 2048 = 4Mb , ~ 1 second */
#define HD_BUF_ELEM_SIZE        (2048+64)
#define TEST_H264               1

#define RADIO_MAX_BUFFERS  10

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


#define  CONTROL_BUF       (0x0f000000)              /* 0x0f000000 */
#define  CONTROL_BUF_BLANK (CONTROL_BUF|0x00010000)  /* 0x0f010000 */
#define  CONTROL_BUF_CLEAR (CONTROL_BUF|0x00020000)  /* 0x0f020000 */

#define SPU_CHANNEL_NONE   (-2)
#define SPU_CHANNEL_AUTO   (-1)

/******************************* LOG ***********************************/

#define LOG_MODULENAME "[input_vdr] "
#define SysLogLevel iSysLogLevel

#include "logdefs.h"

#undef  x_syslog
#define x_syslog syslog_with_tid

int iSysLogLevel  = 1; /* 0:none, 1:errors, 2:info, 3:debug */
int bLogToSysLog  = 0;
int bSymbolsFound = 0;

static void syslog_with_tid(int level, const char *fmt, ...) __attribute__ ((format (printf, 2, 3)));
static void syslog_with_tid(int level, const char *fmt, ...)
{
  va_list argp;
  char buf[512];
  va_start(argp, fmt);
  vsnprintf(buf, sizeof(buf), fmt, argp);
  buf[sizeof(buf)-1] = 0;
#ifdef __APPLE__
  if(!bLogToSysLog) {
    fprintf(stderr, LOG_MODULENAME "%s\n", buf);
  } else {
    syslog(level, LOG_MODULENAME "%s", buf);
  }
#else
  if(!bLogToSysLog) {
    fprintf(stderr,"[%ld] " LOG_MODULENAME "%s\n", syscall(__NR_gettid), buf);
  } else {
    syslog(level, "[%ld] " LOG_MODULENAME "%s", syscall(__NR_gettid), buf);
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

#define LOG_UDP

#ifdef LOG_SCR
#  define LOGSCR(x...) LOGMSG("SCR: " x)
#else
#  define LOGSCR(x...)
#endif
#
#ifdef LOG_OSD
#  define LOGOSD(x...) LOGMSG("OSD: " x)
#else
#  define LOGOSD(x...)
#endif
#
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


/*#define DEBUG_LOCKING*/
#ifdef DEBUG_LOCKING
# include "tools/debug_mutex.h"
#endif

/******************************* DATA ***********************************/

#define KILOBYTE(x)   (1024 * (x))

typedef struct pvrscr_s pvrscr_t;
typedef struct udp_data_s udp_data_t;

/* plugin class */
typedef struct vdr_input_class_s {
  input_class_t   input_class;
  xine_t         *xine;
  char           *mrls[ 2 ];
  int             fast_osd_scaling;
} vdr_input_class_t;

/* input plugin */
typedef struct vdr_input_plugin_s {
  input_plugin_t      input_plugin;

  /* VDR */
  vdr_input_plugin_funcs_t funcs; 

  /* plugin */
  vdr_input_class_t  *class;
  xine_stream_t      *stream;
  xine_event_queue_t *event_queue;

  char               *mrl;

  xine_stream_t      *pip_stream;
  xine_stream_t      *slave_stream;
  xine_event_queue_t *slave_event_queue;
  int                 autoplay_size;

  /* Sync */
  pthread_mutex_t     lock;
  pthread_mutex_t     vdr_entry_lock;
  pthread_cond_t      engine_flushed;

  /* Playback */
  int                 no_video;
  int                 live_mode;
  int                 still_mode;
  int                 stream_start;
  int                 send_pts;
  int                 padding_cnt;
  int                 loop_play;
  int                 dvd_menu;
  int                 hd_stream;  /* true if current stream is HD */
  int                 h264;       /* -1: unknown, 0: no, 1: yes */

  int                 audio_stream_id; /* ((PES PID) << 8) | (SUBSTREAM ID) */
  int                 prev_audio_stream_id;
  int                 sw_volume_control;
  int                 ffmpeg_video_decoder;

  /* SCR */
  pvrscr_t           *scr;
  int                 scr_tunning;
  int                 fixed_scr;
  int                 scr_live_sync;
  int                 speed_before_pause;
  int                 is_paused;
  int                 is_trickspeed;

  int                 I_frames;   /* amount of I-frames passed to demux */
  int                 B_frames;
  int                 P_frames;

  /* Network */
  pthread_t           control_thread;
  pthread_t           data_thread;
  pthread_mutex_t     fd_control_lock;
  int                 threads_initialized;
  volatile int        control_running;
  volatile int        fd_data;
  volatile int        fd_control;
  int                 tcp, udp, rtp;
  udp_data_t         *udp_data;
  int                 client_id;
  int                 token;

  /* buffer */
  fifo_buffer_t      *block_buffer;  /* blocks to be demuxed */
  fifo_buffer_t      *buffer_pool;   /* stream's video fifo */
  fifo_buffer_t      *big_buffer;    /* for jumbo PES */
  fifo_buffer_t      *hd_buffer;     /* more buffer for HD streams */
  fifo_buffer_t      *iframe_buffer; /* buffer for cached I-frame */
  int                 saving_iframe;
  uint64_t            discard_index; /* index of next byte to feed to demux; 
					all data before this offset will 
					be discarded */
  int                 discard_frame;
  uint64_t            guard_index;   /* data before this offset will not be discarded */
  int                 guard_frame;
  uint64_t            curpos;        /* current position (demux side) */
  int                 curframe;      
  int                 max_buffers;   /* max no. of non-demuxed buffers */
  int64_t             last_delivered_vid_pts; /* detect PTS wraps */

  /* saved video properties */
  int   video_properties_saved;
  int   orig_hue;
  int   orig_brightness;
  int   orig_saturation;
  int   orig_contrast;
  int   orig_vo_aspect_ratio;

  /* OSD */
  pthread_mutex_t osd_lock;
  int vdr_osd_width, vdr_osd_height;
  int video_width, video_height;
  int video_changed;
  int osdhandle[MAX_OSD_OBJECT];
  int64_t last_changed_vpts[MAX_OSD_OBJECT];
  osd_command_t osddata[MAX_OSD_OBJECT];

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

  /* missing frames ratio statistics */
  int16_t  missed_frames;
  int16_t  received_frames; 

  /* SCR adjust */
  uint8_t  scr_jump_done;

  int resend_requested : 1;
};

/* UDP sequence number handling */
#define NEXTSEQ(x)  ((x + 1) & UDP_SEQ_MASK)
#define PREVSEQ(x)  ((x + UDP_SEQ_MASK) & UDP_SEQ_MASK)
#define INCSEQ(x)   (x = NEXTSEQ(x))
#define ADDSEQ(x,n) ((x + UDP_SEQ_MASK + 1 + n) & UDP_SEQ_MASK)

#define UDP_SIGNAL_FULL_TRESHOLD     50  /* ~100ms with DVB mpeg2 
					    (2k-blocks @ 8 Mbps)  */
#define UDP_SIGNAL_NOT_FULL_TRESHOLD 100 /* ~200ms with DVB mpeg2 
					    (2k-blocks @ 8 Mbps)  */

static udp_data_t *init_udp_data(void)
{
  udp_data_t *data = calloc(1, sizeof(udp_data_t));

  data->received_frames  = -1; 

  return data;
}

static void free_udp_data(udp_data_t *data)
{
  int i;

  for(i=0; i<=UDP_SEQ_MASK; i++)
    if(data->queue[i]) {
      data->queue[i]->free_buffer(data->queue[i]);
      data->queue[i] = NULL;
    }
  free(data);
}

#if 0
static void flush_udp_data(udp_data_t *data)
{
  /* flush all data immediately even if there are gaps */
}
#endif

#ifdef ADJUST_SCR_SPEED
/******************************* SCR *************************************
 *
 * unix System Clock Reference + fine tunning
 *
 * pvrscr code is mostly copied from xine, input_pvr.c
 *
 * fine tunning is used to change playback speed in live mode
 * to keep in sync with mpeg source
 *************************************************************************/

struct pvrscr_s {
  scr_plugin_t     scr;

  struct timeval   cur_time;
  int64_t          cur_pts;
  int              xine_speed;
  int              scr_speed_base;
  double           speed_factor;
  double           speed_tunning;

  pthread_mutex_t  lock;

  struct timeval   last_time;
};

static int pvrscr_get_priority (scr_plugin_t *scr) 
{
  return 50; /* high priority */
}

/* Only call pvrscr_set_pivot when already mutex locked ! */
static void pvrscr_set_pivot (pvrscr_t *this) 
{
  struct   timeval tv;
  int64_t pts;
  double   pts_calc;

  xine_monotonic_clock(&tv,NULL);

  pts_calc = (tv.tv_sec  - this->cur_time.tv_sec) * this->speed_factor;
  pts_calc += (tv.tv_usec - this->cur_time.tv_usec) * this->speed_factor / 1e6;
  pts = this->cur_pts + pts_calc;

  /* This next part introduces a one off inaccuracy
   * to the scr due to rounding tv to pts.
   */
  this->cur_time.tv_sec=tv.tv_sec;
  this->cur_time.tv_usec=tv.tv_usec;
  this->cur_pts=pts;

  this->last_time.tv_sec  = tv.tv_sec;
  this->last_time.tv_usec = tv.tv_usec;

  return ;
}

static int pvrscr_set_fine_speed (scr_plugin_t *scr, int speed) 
{
  pvrscr_t *this = (pvrscr_t*) scr;

  pthread_mutex_lock (&this->lock);

  pvrscr_set_pivot( this );
  this->xine_speed     = speed;
  this->speed_factor   = (double) speed * (double)this->scr_speed_base /*90000.0*/ / 
                         (1.0*XINE_FINE_SPEED_NORMAL) *
                         this->speed_tunning;

  pthread_mutex_unlock (&this->lock);

  return speed;
}

static void pvrscr_speed_tunning (pvrscr_t *this, double factor) 
{
  pthread_mutex_lock (&this->lock);

  pvrscr_set_pivot( this );
  this->speed_tunning = factor;
  this->speed_factor = (double) this->xine_speed * (double)this->scr_speed_base /*90000.0*/ / 
                       (1.0*XINE_FINE_SPEED_NORMAL) *
                       this->speed_tunning;

  pthread_mutex_unlock (&this->lock);
}

static void pvrscr_speed_base (pvrscr_t *this, int hz) 
{
  pthread_mutex_lock (&this->lock);

  pvrscr_set_pivot( this );
  this->scr_speed_base = hz;
  this->speed_factor = (double) this->xine_speed * (double)this->scr_speed_base /*90000.0*/ / 
                       (1.0*XINE_FINE_SPEED_NORMAL) *
                       this->speed_tunning;

  pthread_mutex_unlock (&this->lock);
}

static void pvrscr_skip_frame (pvrscr_t *this) 
{
  pthread_mutex_lock (&this->lock);

  pvrscr_set_pivot( this );
  this->cur_pts += (90000/25)*1ULL;

  pthread_mutex_unlock (&this->lock);
}

static void pvrscr_adjust (scr_plugin_t *scr, int64_t vpts) 
{
  pvrscr_t *this = (pvrscr_t*) scr;
  struct   timeval tv;

  pthread_mutex_lock (&this->lock);

  xine_monotonic_clock(&tv,NULL);
  this->cur_time.tv_sec=tv.tv_sec;
  this->cur_time.tv_usec=tv.tv_usec;
  this->cur_pts = vpts;

  this->last_time.tv_sec  = tv.tv_sec;
  this->last_time.tv_usec = tv.tv_usec;

  pthread_mutex_unlock (&this->lock);
}

static void pvrscr_start (scr_plugin_t *scr, int64_t start_vpts) 
{
  pvrscr_t *this = (pvrscr_t*) scr;

  pthread_mutex_lock (&this->lock);

  xine_monotonic_clock(&this->cur_time, NULL);
  this->cur_pts = start_vpts;

  this->last_time.tv_sec  = this->cur_time.tv_sec;
  this->last_time.tv_usec = this->cur_time.tv_usec;

  pthread_mutex_unlock (&this->lock);

  pvrscr_set_fine_speed (&this->scr, XINE_FINE_SPEED_NORMAL);
}

static int64_t pvrscr_get_current (scr_plugin_t *scr) 
{
  pvrscr_t *this = (pvrscr_t*) scr;

  struct   timeval tv;
  int64_t pts;
  double   pts_calc;
  pthread_mutex_lock (&this->lock);

  xine_monotonic_clock(&tv,NULL);

#ifdef LOG_SCR
  if(this->last_time.tv_sec+3 < tv.tv_sec && this->last_time.tv_sec) {
    LOGMSG("ERROR - CLOCK JUMPED FORWARDS ? "
	   "(pvrscr_get_current diff %d.%06d sec)\n",
           (int)(tv.tv_sec - this->last_time.tv_sec),
	   (int)(tv.tv_usec - this->last_time.tv_usec));
    pthread_mutex_unlock (&this->lock);
    pvrscr_adjust(scr,this->cur_pts);
    pthread_mutex_lock (&this->lock);
  }
  else if(this->last_time.tv_sec > tv.tv_sec) {
    LOGMSG("ERROR - CLOCK JUMPED BACKWARDS ! "
	   "(pvrscr_get_current diff %d.%06d sec)\n",
           (int)(tv.tv_sec - this->last_time.tv_sec),
	   (int)(tv.tv_usec - this->last_time.tv_usec));
    pthread_mutex_unlock (&this->lock);
    pvrscr_adjust(scr,this->cur_pts);
    pthread_mutex_lock (&this->lock);
  }
#endif

  pts_calc = (tv.tv_sec  - this->cur_time.tv_sec) * this->speed_factor;
  pts_calc += (tv.tv_usec - this->cur_time.tv_usec) * this->speed_factor / 1e6;
  
  pts = this->cur_pts + pts_calc;

  this->last_time.tv_sec  = tv.tv_sec;
  this->last_time.tv_usec = tv.tv_usec;

  pthread_mutex_unlock (&this->lock);

  return pts;
}

static void pvrscr_exit (scr_plugin_t *scr) 
{
  pvrscr_t *this = (pvrscr_t*) scr;

  pthread_mutex_destroy (&this->lock);
  free(this);
}

static pvrscr_t* pvrscr_init (void) 
{
  pvrscr_t *this;

  this = malloc(sizeof(*this));
  memset(this, 0, sizeof(*this));

  this->scr.interface_version = 3;
  this->scr.set_fine_speed    = pvrscr_set_fine_speed;
  this->scr.get_priority      = pvrscr_get_priority;
  this->scr.adjust            = pvrscr_adjust;
  this->scr.start             = pvrscr_start;
  this->scr.get_current       = pvrscr_get_current;
  this->scr.exit              = pvrscr_exit;

  pthread_mutex_init (&this->lock, NULL);

  this->scr_speed_base = 90000;

  pvrscr_speed_tunning(this, 1.0 );
  pvrscr_set_fine_speed (&this->scr, XINE_SPEED_PAUSE);

  LOGSCR("SCR init complete");

  return this;
}

/*
 *  SCR tunning 
 */

#define SCR_TUNNING_PAUSED -3
#define SCR_TUNNING_OFF     0

#ifdef LOG_SCR
static inline const char *scr_tunning_str(int value)
{
  switch(value) {
    case 2:  return "SCR +1.0%";
    case 1:  return "SCR +0.5%";
    case SCR_TUNNING_OFF:  return "SCR +0.0%";
    case -1: return "SCR -0.5%";
    case -2: return "SCR -1.0%";
    case SCR_TUNNING_PAUSED: return "SCR PAUSED";
    default: break;
  }
  return "ERROR";
}
#endif

static int64_t monotonic_time_ms (void) 
{
  static struct timeval tv_0;
  static int init_done = 0;
  struct timeval tv;
  int64_t ms;

  if(!init_done) {
    init_done = 1;
    xine_monotonic_clock(&tv_0, NULL);    
  }
  xine_monotonic_clock(&tv, NULL);

  ms = 1000LL * (tv.tv_sec - tv_0.tv_sec);
  ms += tv.tv_usec / 1000;
  return ms;
}

static void scr_tunning_set_paused(vdr_input_plugin_t *this)
{
  if(this->scr_tunning != SCR_TUNNING_PAUSED &&
     !this->slave_stream &&
     !this->is_trickspeed) {

    this->scr_tunning = SCR_TUNNING_PAUSED;  /* marked as paused */
    if(this->scr)
      pvrscr_speed_tunning(this->scr, 1.0);
    
    this->speed_before_pause = _x_get_fine_speed(this->stream);

#ifdef TEST_SCR_PAUSE
    if(_x_get_fine_speed(this->stream) != XINE_SPEED_PAUSE)
      _x_set_fine_speed(this->stream, XINE_SPEED_PAUSE);
#else
    _x_set_fine_speed(this->stream, 1000000 / 1000); /* -> speed to 0.1%  */
#endif
    this->I_frames = this->P_frames = this->B_frames = 0;
  }
}

static void reset_scr_tunning(vdr_input_plugin_t *this, int new_speed)
{
  if(this->scr_tunning != SCR_TUNNING_OFF) {
    this->scr_tunning = SCR_TUNNING_OFF; /* marked as normal */
    if(this->scr)
      pvrscr_speed_tunning(this->scr, 1.0);

    if(new_speed >= 0) {
      if(_x_get_fine_speed(this->stream) != new_speed) {
	_x_set_fine_speed(this->stream, XINE_FINE_SPEED_NORMAL);
      }
      pvrscr_set_fine_speed((scr_plugin_t*)this->scr, XINE_FINE_SPEED_NORMAL);
    }
  }
}

static void vdr_adjust_realtime_speed(vdr_input_plugin_t *this) 
{
  /*
   * Grab current buffer usage 
   */
  int num_used = this->buffer_pool->size(this->buffer_pool) + 
                 this->block_buffer->size(this->block_buffer);
  int num_free = this->buffer_pool->num_free(this->buffer_pool);
  int scr_tunning = this->scr_tunning;
  /*int num_vbufs = 0;*/

  if(this->hd_stream && this->hd_buffer) {
    num_free += this->hd_buffer->num_free(this->hd_buffer);
  }

  if(this->stream->audio_fifo)
    num_used += this->stream->audio_fifo->size(this->stream->audio_fifo);
  num_free -= (this->buffer_pool->buffer_pool_capacity - this->max_buffers);

#ifdef LOG_SCR
  /*
   * Trace current buffer and tunning status
   */
  {
    static int fcnt=0;
    if(!((fcnt++)%2500) || 
       (this->scr_tunning==SCR_TUNNING_PAUSED && !(fcnt%10)) ||
       (this->no_video        && !(fcnt%50))) {
      LOGSCR("Buffer %2d%% (%3d/%3d) %s", 
	     100*num_used/(num_used+num_free), 
	     num_used, num_used+num_free, 
	     scr_tunning_str(this->scr_tunning));
    }
  }

  if(this->scr_tunning==SCR_TUNNING_PAUSED) {
    if(_x_get_fine_speed(this->stream) != XINE_SPEED_PAUSE) { 
      LOGMSG("ERROR: SCR PAUSED ; speed=%d bool=%d", 
	     _x_get_fine_speed(this->stream), 
	     (int)_x_get_fine_speed(this->stream) == XINE_SPEED_PAUSE);
      _x_set_fine_speed(this->stream, XINE_SPEED_PAUSE);
    }
  }
#endif

  /*
   * SCR -> PAUSE
   *  - If buffer is empty, pause SCR (playback) for a while 
   */
  if( num_used < 1 && 
      scr_tunning != SCR_TUNNING_PAUSED && 
      !this->no_video && !this->still_mode && !this->is_trickspeed) {
/* 
   #warning TODO:
   - First I-frame can be delivered as soon as it is decoded 
   -> illusion of faster channel switches
   - Clock must still be paused, but stream can be in PLAYING state
   (if clock is not paused we will got a lot of discarded frames 
   as those are decoded too late according to running SCR)
*/
#if 0
    this->class->xine->port_ticket->acquire(this->class->xine->port_ticket, 0);
    num_vbufs = this->stream->video_out->get_property(this->stream->video_out, 
						      VO_PROP_BUFS_IN_FIFO);
    this->class->xine->port_ticket->release(this->class->xine->port_ticket, 0);
    if(num_vbufs < 3) {
      LOGSCR("SCR paused by adjust_speed (vbufs=%d)", num_vbufs);
#endif
      scr_tunning_set_paused(this);
#if 0
    } else {
      LOGSCR("adjust_speed: no pause, enough vbufs queued (%d)", num_vbufs);
    }
#endif


  /* SCR -> RESUME
   *  - If SCR (playback) is currently paused due to previous buffer underflow,
   *    revert to normal if buffer fill is > 66%
   */
  } else if( scr_tunning == SCR_TUNNING_PAUSED) {
/* 
   #warning TODO:
   - Using amount of buffers is not good trigger as it depends on channel bitrate
   - Wait time is not not good trigger as it depends on tuner lock time
   -> maybe keep track of PTSes or wait until decoder has complete IBBBP frame sequence ? 
   - First I-frame can be delivered as soon as it is decoded 
   -> illusion of faster channel switches
*/
#if 0
    /* causes random freezes with some post plugins */
    this->class->xine->port_ticket->acquire(this->class->xine->port_ticket, 0);
    num_vbufs = this->stream->video_out->get_property(this->stream->video_out, 
						      VO_PROP_BUFS_IN_FIFO);
    this->class->xine->port_ticket->release(this->class->xine->port_ticket, 0);
#endif

    if( num_used/2 > num_free 
	|| (this->no_video && num_used > 5)
	|| this->still_mode
	|| this->is_trickspeed
	|| ( this->I_frames > 0
	     && (this->I_frames > 2 || this->P_frames > 6 ))
	) {
      LOGSCR("SCR tunning resetted by adjust_speed, "
             "I %d B %d P %d", this->I_frames, this->B_frames, this->P_frames);

      this->I_frames = 0;
      reset_scr_tunning(this, this->speed_before_pause);
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
  } else if( _x_get_fine_speed(this->stream) == XINE_FINE_SPEED_NORMAL) {

    if(!this->scr_live_sync) {
        scr_tunning = SCR_TUNNING_OFF;

    } else if(this->no_video) {  /* radio stream ? */
      if( num_used >= (RADIO_MAX_BUFFERS-1))
        scr_tunning = +1; /* play faster */
      else if( num_used <= (RADIO_MAX_BUFFERS/3))
        scr_tunning = -1; /* play slower */
      else
        scr_tunning = SCR_TUNNING_OFF;
    } else {
      if( num_used > 4*num_free )
        scr_tunning = +2; /* play 1% faster */
      else if( num_used > 2*num_free )
        scr_tunning = +1; /* play .5% faster */
      else if( num_free > 4*num_used ) /* <20% */
        scr_tunning = -2; /* play 1% slower */
      else if( num_free > 2*num_used ) /* <33% */
        scr_tunning = -1; /* play .5% slower */
      else if( (scr_tunning > 0 && num_free > num_used) ||
	       (scr_tunning < 0 && num_used > num_free) )
        scr_tunning = SCR_TUNNING_OFF;
    }

    if( scr_tunning != this->scr_tunning ) {
      LOGSCR("scr_tunning: %s -> %s (buffer %d/%d)", 
	     scr_tunning_str(this->scr_tunning), 
	     scr_tunning_str(scr_tunning), num_used, num_free );
      this->scr_tunning = scr_tunning;

      /* make it play .5% / 1% faster or slower */
      if(this->scr)
	pvrscr_speed_tunning(this->scr, 1.0 + (0.005 * scr_tunning) );
    }

  /*
   * SCR -> NORMAL
   *  - If we are in replay (or trick speed) mode, switch SCR tunning off
   *    as we can always have complete control on incoming data rate
   */
  } else if( this->scr_tunning ) {
    reset_scr_tunning(this, -1);
  }
}

#else /* ADJUST_SCR_SPEED */

struct pvrscr_s { 
  int dummy; 
};

static void vdr_adjust_realtime_speed(vdr_input_plugin_t *this, 
				      fifo_buffer_t *fifo1, 
				      fifo_buffer_t *fifo2, 
				      int speed ) 
{
}

static void reset_scr_tunning(vdr_input_plugin_t *this, int new_speed) 
{
}

static void scr_tunning_set_paused(vdr_input_plugin_t *this, 
				   int speed_before_pause) 
{
}

#endif /* ADJUST_SCR_SPEED */

/******************************* TOOLS ***********************************/

#ifndef MIN
# define MIN(a,b) ((a)<(b)?(a):(b))
#endif
#ifndef MAX
# define MAX(a,b) ((a)>(b)?(a):(b))
#endif

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

static int64_t pts_from_pes(const uint8_t *buf, int size)
{
  int64_t pts = INT64_C(-1);
  if(size > 13 && (buf[7] & 0x80)) { /* pts avail */
    pts  = ((int64_t)( buf[ 9] & 0x0E)) << 29;
    pts |=  (int64_t)( buf[10]         << 22 );
    pts |=  (int64_t)((buf[11] & 0xFE) << 14 );
    pts |=  (int64_t)( buf[12]         <<  7 );
    pts |=  (int64_t)((buf[13] & 0xFE) >>  1 );
  }
  return pts;
}

#if 0
static int64_t dts_from_pes(const uint8_t *buf, int size)
{
  int64_t dts = INT64_C(-1);
  if(size > 18 && (buf[7] & 0x40)) { /* dts avail */
    dts  = ((int64_t)( buf[14] & 0x0E)) << 29 ;
    dts |=  (int64_t)( buf[15]         << 22 );
    dts |=  (int64_t)((buf[16] & 0xFE) << 14 );
    dts |=  (int64_t)( buf[17]         <<  7 );
    dts |=  (int64_t)((buf[18] & 0xFE) >>  1 );
  }
  return dts;
}
#endif

static void pes_strip_pts(uint8_t *buf, int size)
{
  if(size > 13 && buf[7] & 0x80) { /* pts avail */
    int pes_len = (buf[4] << 8) | buf[5];
    if ((buf[6] & 0xC0) != 0x80)
      return;
    if ((buf[6] & 0x30) != 0)
      return;
    pes_len -= 5;     /* update packet len */
    buf[4]   = pes_len >> 8;   /* packet len (hi) */
    buf[5]   = pes_len & 0xff; /* packet len (lo) */
    buf[7]  &= 0x7f;  /* clear pts flag */
    buf[8]  -= 5;     /* update header len */
    memmove(buf+9, buf+14, size-14);
  }
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
    if(errno == EINTR || errno == EAGAIN)
      return XIO_TIMEOUT;
    return XIO_ERROR;
  }
  if(FD_ISSET(fd,&eset))
    return XIO_ERROR;
  if(FD_ISSET(fd,&fdset))
    return XIO_READY;

  return XIO_TIMEOUT;
}

static void write_control_data(vdr_input_plugin_t *this, const char *str, size_t len)
{
  size_t ret;
  while(len>0) {

    if(!this->control_running) {
      LOGERR("write_control aborted");
      return;  
    }

#if 1
    fd_set fdset, eset;
    struct timeval select_timeout;
    FD_ZERO (&fdset);
    FD_ZERO (&eset);
    FD_SET  (this->fd_control, &fdset);
    FD_SET  (this->fd_control, &eset);   
    select_timeout.tv_sec  = 0;
    select_timeout.tv_usec = 500*1000; /* 500 ms */
    errno = 0;
    if(1 != select (this->fd_control + 1, NULL, &fdset, &eset, &select_timeout) ||
       !FD_ISSET(this->fd_control, &fdset) ||
       FD_ISSET(this->fd_control, &eset)) {
      LOGERR("write_control failed (poll timeout or error)");
      this->control_running = 0;
      return;
    }
#endif

    if(!this->control_running) {
      LOGERR("write_control aborted");
      return;  
    }

    errno = 0;
    ret = write(this->fd_control, str, len);

    if(ret <= 0) {
      if(ret == 0) {
	LOGMSG("write_control: disconnected");
      } else if(errno == EAGAIN) {
	LOGERR("write_control failed: EAGAIN");
	continue;
      } else if(errno == EINTR) {
	LOGERR("write_control failed: EINTR");
	pthread_testcancel();
        continue;
      } else {
	LOGERR("write_control failed");
      }
      this->control_running = 0;
      return;
    }
    len -= ret;
    str += ret;
  }
}

static void write_control(vdr_input_plugin_t *this, const char *str)
{
  size_t len = (size_t)strlen(str);
  pthread_mutex_lock (&this->fd_control_lock);
  write_control_data(this, str, len);
  pthread_mutex_unlock (&this->fd_control_lock);
}

static void printf_control(vdr_input_plugin_t *this, const char *fmt, ...)
{
  va_list argp;
  char buf[512];

  va_start(argp, fmt);
  vsnprintf(buf, sizeof(buf), fmt, argp);
  buf[sizeof(buf)-1] = 0;

  write_control(this, buf);
  va_end(argp);
}

static int readline_control(vdr_input_plugin_t *this, char *buf, int maxlen,
			    int timeout)
{
  int num_bytes = 0, total_bytes = 0, err;

  *buf = 0;
  while(total_bytes < maxlen-1 ) {

    if(!this->control_running && timeout<0)
      return -1;

    pthread_testcancel();
    err = io_select_rd(this->fd_control);
    pthread_testcancel();

    if(!this->control_running && timeout<0)
      return -1;

    if(err == XIO_TIMEOUT) {
      if(timeout==0)
	return 0;
      if(timeout>0)
	timeout--;
      continue;
    }
    if(err == XIO_ABORTED) {
      LOGERR("readline_control: XIO_ABORTED at [%d]", num_bytes);
      continue;
    }
    if(err != XIO_READY /* == XIO_ERROR */) {
      LOGERR("readline_control: poll error at [%d]", num_bytes);
      return -1;
    }

    errno = 0;
    num_bytes = read (this->fd_control, buf + total_bytes, 1);
    pthread_testcancel();

    if(!this->control_running && timeout<0)
      return -1;

    if (num_bytes <= 0) {
      if(num_bytes==0)
	LOGERR("Control stream disconnected");
      else
	LOGERR("readline_control: read error at [%d]", num_bytes);
      if(num_bytes < 0 && (errno == EINTR || errno==EAGAIN))
	continue;
      return -1;
    }
      
    if(buf[total_bytes]) {
      if(buf[total_bytes] == '\r') {
	buf[total_bytes] = 0;
      } else if(buf[total_bytes] == '\n') {
	buf[total_bytes] = 0;
	break;
      } else {
	total_bytes ++;
	buf[total_bytes] = 0;
      }
    }
    TRACE("readline_control: %d bytes ... %s\n", 
	  total_bytes, buf);
  }

  TRACE("readline_control: %d bytes (max %d)\n", total_bytes, maxlen);

  return total_bytes;
}


static int read_control(vdr_input_plugin_t *this, uint8_t *buf, int len)
{
  int num_bytes, total_bytes = 0, err;

  while(total_bytes < len) {
    pthread_testcancel();
    err = io_select_rd(this->fd_control);
    pthread_testcancel();

    if(!this->control_running)
      return -1;

    if(err == XIO_TIMEOUT) {
      continue;
    }
    if(err == XIO_ABORTED) {
      LOGERR("read_control: XIO_ABORTED");
      continue;
    }
    if(err == XIO_ERROR) {
      LOGERR("read_control: poll error");
      return -1;
    }

    errno = 0;
    num_bytes = read (this->fd_control, buf + total_bytes, len - total_bytes);
    pthread_testcancel();

    if (num_bytes <= 0) {
      if(this->control_running && num_bytes<0)
	LOGERR("read_control read() error"); 
      return -1;
    }
    total_bytes += num_bytes;
  }

  return total_bytes;
}

static void queue_nosignal(vdr_input_plugin_t *this)
{
#define extern static
#include "nosignal_720x576.c"
#undef extern
  char          *data = NULL, *tmp = NULL;
  int            datalen = 0, pos = 0;
  buf_element_t *buf = NULL;
  char *path, *home;

  if(this->stream->video_fifo->num_free(this->stream->video_fifo) < 10) {
    LOGMSG("queue_nosignal: not enough free buffers (%d) !", 
	   this->stream->video_fifo->num_free(this->stream->video_fifo));
    return;
  }

  if (asprintf(&home,"%s/.xine/nosignal.mpg", xine_get_homedir()) < 0)
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
    buf = this->stream->video_fifo->buffer_pool_try_alloc(this->stream->video_fifo);
    if(buf) {
      buf->content = buf->mem;
      buf->size = MIN(datalen - pos, buf->max_size);
      buf->type = BUF_VIDEO_MPEG;
      xine_fast_memcpy(buf->content, &data[pos], buf->size);
      pos += buf->size;
      if(pos >= datalen)
        buf->decoder_flags |= BUF_FLAG_FRAME_END;
      this->stream->video_fifo->put(this->stream->video_fifo, buf);
    } else {
      LOGMSG("Error: queue_nosignal: no buffers !");
      break;
    }
  }

  /* sequence end */
  buf = this->stream->video_fifo->buffer_pool_try_alloc(this->stream->video_fifo);
  if (buf) {
    static const uint8_t seq_end[] = {0x00, 0x00, 0x01, 0xb7}; /* mpeg2 */
    buf->type = BUF_VIDEO_MPEG;
    buf->size = sizeof(seq_end);
    buf->decoder_flags = BUF_FLAG_FRAME_END;
    memcpy(buf->content, seq_end, sizeof(seq_end));
    this->stream->video_fifo->put(this->stream->video_fifo, buf);

    /*put_control_buf(this->stream->video_fifo, this->stream->video_fifo, BUF_CONTROL_FLUSH_DECODER);*/
    /*put_control_buf(this->stream->video_fifo, this->stream->video_fifo, BUF_CONTROL_NOP);*/
  }

  free(tmp);
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

#if 0
static buf_element_t *buffer_pool_timed_alloc(fifo_buffer_t *fifo, int timeoutMs, int buffer_limit)
{
  struct timespec    abstime;
  create_timeout_time(&abstime, timeoutMs);

  pthread_mutex_lock(&fifo->buffer_pool_mutex);

  while(fifo->buffer_pool_num_free <= buffer_limit) {
    if(pthread_cond_timedwait (&fifo->buffer_pool_cond_not_empty, &fifo->buffer_pool_mutex, &abstime) == ETIMEDOUT)
      break;
  }

  pthread_mutex_unlock(&fifo->buffer_pool_mutex);

  return fifo->buffer_pool_try_alloc(fifo);
}
#endif

#if 0
static buf_element_t *fifo_buffer_timed_get(fifo_buffer_t *fifo, int timeoutMs)
{
  struct timespec    abstime;
  create_timeout_time(&abstime, timeoutMs);
  
  return NULL;
}
#endif

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

#if 0
static int fifo_buffer_clear(fifo_buffer_t *fifo)
{
  int bytes = 0;
  buf_element_t *buf, *next, *prev=NULL;
  pthread_mutex_lock (&fifo->mutex);
  buf = fifo->first;
  while (buf != NULL) {
    next = buf->next;
    if ((buf->type & BUF_MAJOR_MASK) !=  BUF_CONTROL_BASE) {
      /* remove this buffer */
      if (prev)
	prev->next = next;
      else
	fifo->first = next;

      if (!next)
	fifo->last = prev;

      fifo->fifo_size--;
      bytes += buf->size;
      fifo->fifo_data_size -= buf->size;
      
      buf->free_buffer(buf);
    } else {
      prev = buf;
    }
    buf = next;
  }
  pthread_mutex_unlock (&fifo->mutex);

  return bytes;
}
#endif

static void signal_buffer_pool_not_empty(vdr_input_plugin_t *this)
{
  if(this->buffer_pool) {
    pthread_mutex_lock(&this->buffer_pool->buffer_pool_mutex);
    pthread_cond_broadcast(&this->buffer_pool->buffer_pool_cond_not_empty);
    pthread_mutex_unlock(&this->buffer_pool->buffer_pool_mutex);
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

  this->buffer_pool_num_free  = 0;
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
  this->alloc_cb[0]              = NULL;
  this->get_cb[0]                = NULL;
  this->put_cb[0]                = NULL;
  this->alloc_cb_data[0]         = NULL;
  this->get_cb_data[0]           = NULL;
  this->put_cb_data[0]           = NULL;

  LOGDBG("fifo_buffer_new done.");
  return this;
}
#endif

static buf_element_t *get_buf_element(vdr_input_plugin_t *this, int size, int force)
{
  buf_element_t *buf = NULL;

  /* HD buffer */
  if(this->hd_stream && size <= HD_BUF_ELEM_SIZE)
    buf = this->hd_buffer->buffer_pool_try_alloc(this->hd_buffer);

  /* limit max. buffered data */
  if(!force && !buf) {
    int buffer_limit = this->buffer_pool->buffer_pool_capacity - this->max_buffers;
    if(this->buffer_pool->buffer_pool_num_free < buffer_limit) 
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
      if(!this->big_buffer)
	this->big_buffer = fifo_buffer_new(this->stream, 4, 512*1024);
      buf = this->big_buffer->buffer_pool_try_alloc(this->big_buffer);
      LOGDBG("get_buf_element: jumbo PES (%d bytes) !", size);
    }
  }

  /* final try from audio fifo */
  if(!buf)
    buf = this->stream->audio_fifo->buffer_pool_try_alloc(this->stream->audio_fifo);

  if(buf) {
    buf->content = buf->mem;
    buf->size = 0;
    buf->type = BUF_DEMUX_BLOCK;
    buf->pts  = 0;

    buf->free_buffer = buffer_pool_free;
  }

  return buf;
}

static void strip_network_headers(vdr_input_plugin_t *this, buf_element_t *buf)
{
  if(buf->type == BUF_MAJOR_MASK) {
    if(this->udp||this->rtp) {
      stream_udp_header_t *header = (stream_udp_header_t *)buf->content;
      this->curpos = header->pos;
      buf->content += sizeof(stream_udp_header_t);
      buf->size -= sizeof(stream_udp_header_t);
    } else {
      stream_tcp_header_t *header = (stream_tcp_header_t *)buf->content;
      this->curpos = header->pos;
      buf->content += sizeof(stream_tcp_header_t);
      buf->size -= sizeof(stream_tcp_header_t);
    }
    buf->type = BUF_DEMUX_BLOCK;
  }
}

static buf_element_t *make_padding_frame(vdr_input_plugin_t *this)
{
  static const uint8_t padding[] = {0x00,0x00,0x01,0xBE,0x00,0x02,0xff,0xff};
  buf_element_t *buf;

  buf = get_buf_element(this, 8, 1);

  if(!buf && this->stream->audio_fifo)
    buf = this->stream->audio_fifo->buffer_pool_try_alloc(this->stream->audio_fifo);

  if(buf) {
    buf->size = 8;
    buf->content = buf->mem;
    buf->type = BUF_DEMUX_BLOCK;
    memcpy(buf->content, padding, 8);
  }

  return buf;
}

void put_control_buf(fifo_buffer_t *buffer, fifo_buffer_t *pool, int cmd)
{
  buf_element_t *buf = pool->buffer_pool_try_alloc(pool);
  if(buf) {
    buf->type = cmd;
    buffer->put(buffer, buf);
  }
}

static void queue_blank_yv12(vdr_input_plugin_t *this)
{
  int ratio = _x_stream_info_get(this->stream, XINE_STREAM_INFO_VIDEO_RATIO);
  double dratio;

  if(!this || !this->stream)
    return;

  if(ratio > 13300 && ratio < 13400) dratio = 4.0/3.0;
  else if(ratio > 17700 && ratio < 17800) dratio = 16.0/9.0;
  else if(ratio > 21000 && ratio < 22000) dratio = 2.11/1.0;
  else dratio = ((double)ratio)/10000.0;

  if(this->stream && this->stream->video_out) {
    /* our video size is size _after_ cropping, so generate 
       larger image if cropping is active. This will result 
       in right sized image after cropping ...*/
    int width  = this->video_width;
    int height = this->video_height;
    vo_frame_t *img = NULL;

    width  += xine_get_param(this->stream, XINE_PARAM_VO_CROP_LEFT);
    width  += xine_get_param(this->stream, XINE_PARAM_VO_CROP_RIGHT);
    height += xine_get_param(this->stream, XINE_PARAM_VO_CROP_TOP);
    height += xine_get_param(this->stream, XINE_PARAM_VO_CROP_BOTTOM);

    if(width >= 360 && height >= 288 && width <= 1920 && height <= 1024) {
      this->class->xine->port_ticket->acquire(this->class->xine->port_ticket, 1);
      img = this->stream->video_out->get_frame (this->stream->video_out,
						width, height,
						dratio, XINE_IMGFMT_YV12, 
						VO_BOTH_FIELDS);
      this->class->xine->port_ticket->release(this->class->xine->port_ticket, 1);
    }

    if(img) {
      if(img->format == XINE_IMGFMT_YV12 && img->base[0] && img->base[1] && img->base[2]) {
	if(img->pitches[0] < width)
	  width = img->pitches[0];
	if(img->width < width)
	  width = img->width;
	if(img->height < height)
	  height = img->height;
	memset( img->base[0], 0x00, width * height);
	memset( img->base[1], 0x80, width * height / 4 );
	memset( img->base[2], 0x80, width * height / 4 );
	img->duration  = 3600;
	img->pts       = 3600;
	img->bad_frame = 0;
	img->draw(img, this->stream);
      }
      img->free(img);
    }
  }

  this->still_mode = 0;
  _x_stream_info_set(this->stream, XINE_STREAM_INFO_VIDEO_HAS_STILL, this->still_mode);
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
{ return "xvdr+slave:"; }

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

  while(!this->stream->demux_action_pending) {
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

static int update_video_size(vdr_input_plugin_t *this)
{
#if 0
  int w = 0, h = 0;
  int64_t duration;

  this->class->xine->port_ticket->acquire(this->class->xine->port_ticket, 1);
  this->stream->video_out->status(this->stream->video_out, 
				  this->stream, &w, &h, &duration);
  this->class->xine->port_ticket->release(this->class->xine->port_ticket, 1);

  if(w>0 && h>0) {
    if(this->video_width  != w ||
       this->video_height != h) {
    
      LOGOSD("update_video_size: new video size (%dx%d->%dx%d)",
	     this->video_width, this->video_height, w, h);
      this->video_width = w;
      this->video_height = h;
      this->video_changed = 1;
      return 1;
    }
  }
#endif
  return 0;
}

#define saturate(x,min,max) ( (x)<(min) ? (min) : (x)>(max) ? (max) : (x))

static void palette_rgb_to_yuy(xine_clut_t *clut, int colors)
{
  if (clut && colors>0) {
    int c;
    for (c=0; c<colors; c++) {
      int R  = clut[c].r;
      int G  = clut[c].g;
      int B  = clut[c].b;
      int Y  = (( +  66*R + 129*G +  25*B + 0x80) >> 8) +  16;
      int CR = (( + 112*R -  94*G -  18*B + 0x80) >> 8) + 128;
      int CB = (( -  38*R -  74*G + 112*B + 0x80) >> 8) + 128;
      clut[c].y  = saturate( Y, 16, 235);
      clut[c].cb = saturate(CB, 16, 240);
      clut[c].cr = saturate(CR, 16, 240);
    }
  }
}

static xine_rle_elem_t *uncompress_osd_net(uint8_t *raw, int elems, int datalen)
{
  xine_rle_elem_t *data = (xine_rle_elem_t*)malloc(elems*sizeof(xine_rle_elem_t));
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


/*#define NEW_SCALING*/
#ifdef NEW_SCALING
#include "tools/rle.h"
#else
typedef enum {
  scale_fast = 0,         /* simple pixel doubling/dropping */
  scale_good_BW = 1,      /* linear interpolation, palette re-generation */
} scale_mode_t;
#endif

/* re-scale compressed RLE image */
static xine_rle_elem_t *scale_rle_image(osd_command_t *osdcmd,
                                        int new_w, int new_h, 
					scale_mode_t mode)
{
  #define FACTORBASE      0x100
  #define FACTOR2PIXEL(f) ((f)>>8)
  #define SCALEX(x) FACTOR2PIXEL(factor_x*(x))
  #define SCALEY(y) FACTOR2PIXEL(factor_y*(y))

  xine_rle_elem_t *old_rle = osdcmd->data;
  int old_w = osdcmd->w, old_h = osdcmd->h;
  int old_y = 0, new_y = 0;
  int factor_x = FACTORBASE*new_w/old_w;
  int factor_y = FACTORBASE*new_h/old_h;

  xine_rle_elem_t *new_rle_start, *new_rle, *tmp;
  int rle_size = 8128;
  int num_rle = 0;

#ifdef NEW_SCALING
  /* try better quality grayscale 100%...200% */
  if(mode != scale_fast &&
     old_w <= new_w && old_w*2 >= new_w &&
     old_h <= new_h && old_h*2 >= new_h) {
    tmp = upscale_grayscale_rle_image(osdcmd, new_w, new_h);
    if(tmp)
      return tmp;
  }
#endif

  new_rle_start = new_rle = (xine_rle_elem_t*)malloc(4*rle_size);  

  /* we assume rle elements are breaked at end of line */
  while(old_y < old_h) {
    int elems_current_line = 0;
    int old_x = 0, new_x = 0;

    while(old_x < old_w) { 
      int new_x_end = SCALEX(old_x + old_rle->len);

      if(new_x_end > new_w) {
	new_x_end = new_w;
      }

      new_rle->len   = new_x_end - new_x;
      new_rle->color = old_rle->color;

      old_x += old_rle->len;
      old_rle++; /* may be incremented to last element + 1 (element is not accessed anymore) */

      if(new_rle->len > 0) { 
	new_x += new_rle->len;
	new_rle++;

	num_rle++;
	elems_current_line++;
	
	if( (num_rle + 1) >= rle_size ) {
	  rle_size *= 2;
	  new_rle_start = (xine_rle_elem_t*)realloc( new_rle_start, 4*rle_size);
	  new_rle = new_rle_start + num_rle;
	}
      }
    }
    if(new_x < new_w)
      (new_rle-1)->len += new_w - new_x;
    old_y++; 
    new_y++;

    if(factor_y > FACTORBASE) {
      /* scale up -- duplicate current line ? */
      int dup = SCALEY(old_y) - new_y;

      /* if no lines left in (old) rle, copy all lines still missing from new */
      if(old_y == old_h) 
	dup = new_h - new_y - 1;

      while(dup-- && (new_y+1<new_h)) {
	xine_rle_elem_t *prevline;
	int n;
	if( (num_rle + elems_current_line + 1) >= rle_size ) {
	  rle_size *= 2;
	  new_rle_start = (xine_rle_elem_t*)realloc( new_rle_start, 4*rle_size);
	  new_rle = new_rle_start + num_rle;
	}
	
	/* duplicate previous line */
	prevline = new_rle - elems_current_line;
	for(n = 0; n < elems_current_line; n++) {
	  *new_rle++ = *prevline++;
	  num_rle++;
	}
	new_y++;
      }

    } else if(factor_y < FACTORBASE) {
      /* scale down -- drop next line ? */
      int skip = new_y - SCALEY(old_y);
      if(old_y == old_h-1) {
	/* one (old) line left ; don't skip it if new rle is not complete */
	if(new_y < new_h)
	  skip = 0;
      }
      while(skip-- && 
	    old_y<old_h /* rounding error may add one line, filter it out */) {
	for(old_x = 0; old_x < old_w;) {
	  old_x += old_rle->len;
	  old_rle++;
	}
	old_y++;
      }
    }
  }

  tmp = osdcmd->data;

  osdcmd->data = new_rle_start;
  osdcmd->datalen = num_rle*4;

  if(old_w != new_w) {
    osdcmd->x = (0x100*osdcmd->x * new_w/old_w)>>8;
    osdcmd->w = new_w;
  }
  if(old_h != new_h) {
    osdcmd->y = (0x100*osdcmd->y * new_h/old_h)>>8;
    osdcmd->h = new_h;
  }

  return tmp;
}

static int exec_osd_command(vdr_input_plugin_t *this, osd_command_t *cmd)
{
  video_overlay_event_t   ov_event;
  vo_overlay_t            ov_overlay;
  video_overlay_manager_t *ovl_manager;
  xine_stream_t           *stream = this->slave_stream ?: this->stream;
  int handle = -1, i;

  /* Caller must have locked this->osd_lock ! */

  LOGOSD("exec_osd_command %d", cmd ? cmd->cmd : -1); 

  /* Check parameters */

  if(!cmd || !this || !stream) {
    LOGMSG("exec_osd_command: Stream not initialized !");
    return CONTROL_DISCONNECTED;
  }

  if(cmd->wnd < 0 || cmd->wnd >= MAX_OSD_OBJECT) {
    LOGMSG("exec_osd_command: OSD window handle %d out of range !", cmd->wnd);
    return CONTROL_PARAM_ERROR;
  }

  handle = this->osdhandle[cmd->wnd];

  if(handle < 0 && cmd->cmd == OSD_Close) {
    LOGMSG("exec_osd_command: Attempt to close non-existing OSD (%d) !", cmd->wnd);
    return CONTROL_PARAM_ERROR;
  }

  /* we already have port ticket */
  ovl_manager = 
      stream->video_out->get_overlay_manager(stream->video_out);

  if(!ovl_manager) {
    LOGMSG("exec_osd_command: Stream has no overlay manager !");
    return CONTROL_DISCONNECTED;
  }

  memset(&ov_event, 0, sizeof(ov_event));

  /* calculate exec time */
  if(cmd->pts || cmd->delay_ms) {
    int64_t vpts = xine_get_current_vpts(stream);
    if(cmd->pts) {
      ov_event.vpts = cmd->pts + 
          stream->metronom->get_option(stream->metronom, METRONOM_VPTS_OFFSET);
    } else {
      if(this->last_changed_vpts[cmd->wnd]) 
	ov_event.vpts = this->last_changed_vpts[cmd->wnd] + cmd->delay_ms*90;
    }
    /* execution time must be in future */
    if(ov_event.vpts < vpts)
      ov_event.vpts = 0;
    /* limit delay to 5 seconds (because of seeks and channel switches ...) */
    if(ov_event.vpts > vpts + 5*90000)
      ov_event.vpts = vpts + 5*90000;
  }

  /* Execute command */

  if(cmd->cmd == OSD_Size) {
    this->vdr_osd_width  = cmd->w;
    this->vdr_osd_height = cmd->h;

  } else if(cmd->cmd == OSD_Nop) {
    this->last_changed_vpts[cmd->wnd] = xine_get_current_vpts(stream);

  } else if(cmd->cmd == OSD_SetPalette) {
    /* TODO */
  } else if(cmd->cmd == OSD_Move) {
    /* TODO */
  } else if(cmd->cmd == OSD_Set_YUV) {
    /* TODO */
  } else if(cmd->cmd == OSD_Close) {
    ov_event.event_type = OVERLAY_EVENT_FREE_HANDLE;
    ov_event.object.handle = handle;
    this->osdhandle[cmd->wnd] = -1;
    free(this->osddata[cmd->wnd].data);
    this->osddata[cmd->wnd].data = NULL;
    free(this->osddata[cmd->wnd].palette);
    this->osddata[cmd->wnd].palette = NULL;

    do {
      int r = ovl_manager->add_event(ovl_manager, (void *)&ov_event);
      if(r<0) {
	LOGDBG("OSD_Close(%d): overlay manager queue full !", cmd->wnd);
	ovl_manager->flush_events(ovl_manager);
	continue;
      }
      break;
    } while(1);

    this->last_changed_vpts[cmd->wnd] = 0;

    if((cmd->wnd==0 || this->osdhandle[cmd->wnd-1]<0) &&
       (cmd->wnd==MAX_OSD_OBJECT || this->osdhandle[cmd->wnd+1]<0)) {
      /*LOGMSG("OSD_Close(%d): last, flush ovl manager");*/
      ovl_manager->flush_events(ovl_manager);
    }

  } else if(cmd->cmd == OSD_Set_RLE) {

    int use_unscaled = 0;
    int rle_scaled = 0;
    int xmove = 0, ymove = 0;
    int unscaled_supported = 1;

    stream->video_out->enable_ovl(stream->video_out, 1);

    if(handle < 0)
      handle = this->osdhandle[cmd->wnd] = 
	ovl_manager->get_handle(ovl_manager,0);

    ov_event.event_type = OVERLAY_EVENT_SHOW;
    ov_event.object.handle = handle;
    ov_event.object.overlay = &ov_overlay;
    ov_event.object.object_type = 1; /* menu */
    memset( ov_event.object.overlay, 0, sizeof(*ov_event.object.overlay) );

#if XINE_VERSION_CODE < 10101  
    ov_event.object.overlay->clip_top    = -1;
    ov_event.object.overlay->clip_bottom = 0;
    ov_event.object.overlay->clip_left   = 0;
    ov_event.object.overlay->clip_right  = 0;
#else
    ov_event.object.overlay->hili_top    = -1;
    ov_event.object.overlay->hili_bottom = 0;
    ov_event.object.overlay->hili_left   = 0;
    ov_event.object.overlay->hili_right  = 0;
#endif

    /* palette must contain YUV values for each color index */
    for(i=0; i<cmd->colors; i++) {
      uint32_t *tmp = (uint32_t*)(cmd->palette + i);
      ov_event.object.overlay->color[i] = *tmp & 0xffffff;
      ov_event.object.overlay->trans[i] = (cmd->palette[i].alpha + 0x7)/0xf;
    }

    if(!(stream->video_out->get_capabilities(stream->video_out) &
	 VO_CAP_UNSCALED_OVERLAY))
      unscaled_supported = 0;
    else if(cmd->flags & OSDFLAG_UNSCALED)
      use_unscaled = 1;

    /* store osd for later rescaling (done if video size changes) */
    free(this->osddata[cmd->wnd].data);
    this->osddata[cmd->wnd].data = NULL;
    free(this->osddata[cmd->wnd].palette);
    this->osddata[cmd->wnd].palette = NULL;

    memcpy(&this->osddata[cmd->wnd], cmd, sizeof(osd_command_t));
    this->osddata[cmd->wnd].data = NULL;
    if(cmd->palette) {
      this->osddata[cmd->wnd].palette = malloc(sizeof(xine_clut_t)*cmd->colors);
      memcpy(this->osddata[cmd->wnd].palette, cmd->palette, 4*cmd->colors);
    }

    /* if video size differs from expected (VDR osd is designed for 720x576),
       scale osd to video size or use unscaled (display resolution)
       blending */
    if(!use_unscaled) {
      int w_hi = this->vdr_osd_width  * 1100 / 1000;
      int w_lo = this->vdr_osd_width  *  950 / 1000;
      int h_hi = this->vdr_osd_height * 1100 / 1000;
      int h_lo = this->vdr_osd_height *  950 / 1000;
      int width_diff = 0, height_diff = 0;
#if 0
      int video_changed = update_video_size(this);
      LOGOSD("video size %dx%d, margins %d..%dx%d..%d (changed: %s)", 
	     this->video_width, this->video_height, w_lo, w_hi, h_lo, h_hi
	     video_changed ? "yes" : "no");
#endif
      if(this->video_width  < w_lo)  width_diff  = -1;
      else if(this->video_width  > w_hi)  width_diff  =  1;
      if(this->video_height < h_lo) height_diff = -1;
      else if(this->video_height > h_hi) height_diff =  1;

      if(width_diff || height_diff) {
	int new_w = (0x100*cmd->w * this->video_width 
		     / this->vdr_osd_width)>>8;
	int new_h = (0x100*cmd->h * this->video_height 
		     / this->vdr_osd_height)>>8;
	LOGOSD("Size out of margins, rescaling rle image");
	if(width_diff < 0 || height_diff < 0)
	  if(unscaled_supported && (cmd->flags & OSDFLAG_UNSCALED_LOWRES))
	    use_unscaled = 1;

	if(!use_unscaled && cmd->scaling > 0) {

	  if(height_diff || width_diff) {
	    this->osddata[cmd->wnd].data = cmd->data;
	    this->osddata[cmd->wnd].datalen = cmd->datalen;
	    
	    rle_scaled = 1;
	    scale_rle_image(cmd, new_w, new_h, this->class->fast_osd_scaling ? 0 : 1);
	  } else {
	    LOGOSD("osd_command: size out of margins, using UNSCALED");
	    use_unscaled = unscaled_supported;
	  }
	}
      }
      if(!use_unscaled && !rle_scaled) {
	/* no scaling required, but may still need to re-center OSD */
	if(this->video_width != this->vdr_osd_width)
	  xmove = (this->video_width - this->vdr_osd_width)/2;
	if(this->video_height != this->vdr_osd_height)
	  ymove = (this->video_height - this->vdr_osd_height)/2;
      }
    }

    if(use_unscaled) {
      int win_width  = stream->video_out->get_property(stream->video_out, 
						       VO_PROP_WINDOW_WIDTH);
      int win_height = stream->video_out->get_property(stream->video_out, 
						       VO_PROP_WINDOW_HEIGHT);
      if(cmd->scaling > 0) {
	/* it is not nice to have subs in _middle_ of display when using 1440x900 etc... */
	
	if(win_width > 240 && win_height > 196) {
	  if(cmd->scaling > 0) {
	    /*LOGMSG("Scaling unscaled OSD to %dx%d", win_width, win_height);*/
	    if(win_width != this->vdr_osd_width || win_height != this->vdr_osd_height) {
	      int new_w = (0x100*cmd->w * win_width 
			   / this->vdr_osd_width)>>8;
	      int new_h = (0x100*cmd->h * win_height 
			   / this->vdr_osd_height)>>8;

	      this->osddata[cmd->wnd].data = cmd->data;
	      this->osddata[cmd->wnd].datalen = cmd->datalen;
	    
	      rle_scaled = 1;
	      scale_rle_image(cmd, new_w, new_h, this->class->fast_osd_scaling ? 0 : 1);
	    }
	  }
	}
      }
      if(!rle_scaled) {
	/* no scaling required, but may still need to re-center OSD */
	if(win_width != this->vdr_osd_width)
	  xmove = (win_width - this->vdr_osd_width)/2;
	if(win_height != this->vdr_osd_height)
	  ymove = (win_height - this->vdr_osd_height)/2;
      }
    }

    /* set position and size for this overlay */
    ov_event.object.overlay->x = cmd->x + xmove;
    ov_event.object.overlay->y = cmd->y + ymove;
    ov_event.object.overlay->width = cmd->w;
    ov_event.object.overlay->height = cmd->h;

    /* RLE image */
    ov_event.object.overlay->unscaled = use_unscaled;
    ov_event.object.overlay->rle = (rle_elem_t*)cmd->data;
    ov_event.object.overlay->num_rle = cmd->datalen/4; /* two uint_16's in one element */
    ov_event.object.overlay->data_size = cmd->datalen;

    /* store rle for later scaling (done if video size changes) */
    if(/*!use_unscaled &&*/
       /*!this->osddata[cmd->wnd].data && */
       !rle_scaled /*if scaled, we already have a copy (original data)*/ ) {
      this->osddata[cmd->wnd].data = malloc(cmd->datalen);
      memcpy(this->osddata[cmd->wnd].data, cmd->data, cmd->datalen);
    }
    cmd->data = NULL;/* we 'consume' data (ownership goes for osd manager) */

    /* send event to overlay manager */
    do {
      int r = ovl_manager->add_event(ovl_manager, (void *)&ov_event);
      if(r<0) {
	LOGDBG("OSD_Set_RLE(%d): overlay manager queue full !", cmd->wnd);
	ovl_manager->flush_events(ovl_manager);
	continue;
      }
      break;
    } while(1);

    this->last_changed_vpts[cmd->wnd] =  xine_get_current_vpts(stream);

  } else {
    LOGMSG("Unknown OSD command %d", cmd->cmd);
    return CONTROL_PARAM_ERROR;
  }

  LOGOSD("OSD command %d done", cmd->cmd); 
  return CONTROL_OK;
}

static void vdr_scale_osds(vdr_input_plugin_t *this, 
			   int video_width, int video_height)
{  
  if(! pthread_mutex_lock(&this->osd_lock)) {

    if((this->video_width  != video_width ||
	this->video_height != video_height ||
	this->video_changed) &&
       video_width > 0 && video_height > 0) {
      int i, ticket = 0;

      LOGOSD("New video size (%dx%d->%dx%d)",
	     this->video_width, this->video_height, 
	     video_width, video_height);

      this->video_width = video_width;
      this->video_height = video_height;
      this->video_changed = 0;

      /* just call exec_osd_command for all stored osd's.
         scaling is done automatically if required. */
      for(i=0; i<MAX_OSD_OBJECT; i++)
	if(this->osdhandle[i] >= 0 &&
	   this->osddata[i].data &&
	   this->osddata[i].scaling > 0) {
	  osd_command_t tmp;
	  memcpy(&tmp, &this->osddata[i], sizeof(osd_command_t));
	  memset(&this->osddata[i], 0, sizeof(osd_command_t));

	  if(!ticket) {
	    this->class->xine->port_ticket->acquire(this->class->xine->port_ticket, 1);
	    ticket++;
	  }
	  exec_osd_command(this, &tmp);

	  free(tmp.palette);
	  free(tmp.data);
	}
      if(ticket)
	this->class->xine->port_ticket->release(this->class->xine->port_ticket, 1);
    }
    pthread_mutex_unlock(&this->osd_lock);

  } else {
    LOGERR("vdr_scale_osds: pthread_mutex_lock failed");
  }
}

static int vdr_plugin_exec_osd_command(input_plugin_t *this_gen, 
				       osd_command_t *cmd)
{
  vdr_input_plugin_t *this = (vdr_input_plugin_t *) this_gen;
  int result = CONTROL_DISCONNECTED;
  int video_changed = 0;

  if (this->fd_control >= 0 &&  /* remote mode */
      this->funcs.intercept_osd /* frontend handles OSD */ ) {
    return this->funcs.intercept_osd(this->funcs.fe_handle, cmd) ? CONTROL_OK : CONTROL_DISCONNECTED;
  }

  if(!pthread_mutex_lock (&this->osd_lock)) {
    if(!(cmd->flags & OSDFLAG_YUV_CLUT))
      palette_rgb_to_yuy(cmd->palette, cmd->colors);
    cmd->flags &= ~OSDFLAG_YUV_CLUT;

    video_changed = update_video_size(this);
    this->class->xine->port_ticket->acquire(this->class->xine->port_ticket, 1);
    result = exec_osd_command(this, cmd);
    this->class->xine->port_ticket->release(this->class->xine->port_ticket, 1);	  
    pthread_mutex_unlock (&this->osd_lock);
  } else {
    LOGERR("vdr_plugin_exec_osd_command: pthread_mutex_lock failed");
  }

  if(video_changed)
    vdr_scale_osds(this, this->video_width, this->video_height);

  return result;
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

static void suspend_demuxer(vdr_input_plugin_t *this)
{
  this->stream->demux_action_pending = 1;
  signal_buffer_not_empty(this);
  if(this->is_paused) 
    LOGMSG("WARNING: called suspend_demuxer in paused mode !");
  pthread_mutex_lock( &this->stream->demux_lock );
  this->stream->demux_action_pending = 0;
  /* must be paired with resume_demuxer !!! */
}

static void resume_demuxer(vdr_input_plugin_t *this)
{
  /* must be paired with suspend_demuxer !!! */
  DEMUX_RESUME_SIGNAL;
  pthread_mutex_unlock( &this->stream->demux_lock );
}

#if 0
static void vdr_x_demux_flush_engine (xine_stream_t *stream, vdr_input_plugin_t *this) 
{
  buf_element_t *buf;

  if(this->curpos > this->discard_index) {
#if 0
    LOGMSG("Possibly flushing too much !!! (diff=%"PRIu64" bytes, "
	   "guard @%" PRIu64 ")", 
	   this->curpos - this->discard_index, this->guard_index);
#endif
    if(this->curpos < this->guard_index) {
      LOGMSG("Guard > current position, decoder flush skipped");
      return;
    }
  }

  stream->xine->port_ticket->acquire(stream->xine->port_ticket, 1);

  if (stream->video_out)
    stream->video_out->set_property(stream->video_out, VO_PROP_DISCARD_FRAMES, 1);
  if (stream->audio_out)
    stream->audio_out->set_property(stream->audio_out, AO_PROP_DISCARD_BUFFERS, 1);

  fifo_buffer_clear(stream->video_fifo);
  fifo_buffer_clear(stream->audio_fifo);

  DEMUX_MUTEX_LOCK;

  buf = stream->video_fifo->buffer_pool_alloc (stream->video_fifo);
  buf->type = BUF_CONTROL_RESET_DECODER;
  stream->video_fifo->put (stream->video_fifo, buf);

  buf = stream->audio_fifo->buffer_pool_alloc (stream->audio_fifo);
  buf->type = BUF_CONTROL_RESET_DECODER;
  stream->audio_fifo->put (stream->audio_fifo, buf);

  DEMUX_MUTEX_UNLOCK;

  this->prev_audio_stream_id = 0;

  /* on seeking we must wait decoder fifos to process before doing flush.
   * otherwise we flush too early (before the old data has left decoders)
   */
  _x_demux_control_headers_done (stream);

  if (stream->video_out) {
    stream->video_out->flush(stream->video_out);
    stream->video_out->set_property(stream->video_out, 
				    VO_PROP_DISCARD_FRAMES, 0);
  }

  if (stream->audio_out) {
    stream->audio_out->flush(stream->audio_out);
    stream->audio_out->set_property(stream->audio_out, 
				    AO_PROP_DISCARD_BUFFERS, 0);
  }

  stream->xine->port_ticket->release(stream->xine->port_ticket, 1);
}
#endif

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
  if(this->stream_start) {
    LOGMSG("vdr_flush_engine: stream_start, flush skipped");
    return;
  }

  if(this->curpos > discard_index) {
    if(this->curpos < this->guard_index) {
      LOGMSG("vdr_flush_engine: guard > curpos, flush skipped");
      return;
    }
    LOGMSG("vdr_flush_engine: %"PRIu64" < current position, flush skipped", 
	   discard_index);
    return;
  }

  /* reset speed */
  if(xine_get_param(this->stream, XINE_PARAM_FINE_SPEED) <= 0) {
    LOGMSG("vdr_flush_engine: playback is paused <0>");
    xine_set_param(this->stream, XINE_PARAM_FINE_SPEED, XINE_FINE_SPEED_NORMAL);
  }

  /* suspend demuxer */
  this->stream->demux_action_pending = 1;
  if(pthread_mutex_unlock( &this->lock )) /* to let demuxer return from vdr_plugin_read_* */
    LOGERR("pthread_mutex_unlock failed !");
  suspend_demuxer(this);
  pthread_mutex_lock( &this->lock );

  reset_scr_tunning(this, this->speed_before_pause);

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
  this->prev_audio_stream_id = 0;
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
  xine_set_param(this->stream, XINE_PARAM_VO_DEINTERLACE, method ? 1 : 0);
  
  return 0;
}

static int set_video_properties(vdr_input_plugin_t *this, 
				int hue, int saturation, 
				int brightness, int contrast,
                                int vo_aspect_ratio)
{
  pthread_mutex_lock(&this->lock);

  /* when changed first time, save original/default values */
  if(!this->video_properties_saved && 
     (hue>=0 || saturation>=0 || contrast>=0 || brightness>=0 || 
      vo_aspect_ratio>=0)) {
    this->video_properties_saved = 1;
    this->orig_hue        = xine_get_param(this->stream, 
					   XINE_PARAM_VO_HUE );
    this->orig_saturation = xine_get_param(this->stream, 
					   XINE_PARAM_VO_SATURATION );
    this->orig_brightness = xine_get_param(this->stream, 
					   XINE_PARAM_VO_BRIGHTNESS );
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
  if(contrast>=0 || this->video_properties_saved)
    xine_set_param(this->stream, XINE_PARAM_VO_CONTRAST, 
		   contrast>=0 ? contrast : this->orig_contrast );
  if(vo_aspect_ratio>=0 || this->video_properties_saved)
    xine_set_param(this->stream, XINE_PARAM_VO_ASPECT_RATIO,
                   vo_aspect_ratio>=0 ? vo_aspect_ratio : this->orig_vo_aspect_ratio );

  if(hue<0 && saturation<0 && contrast<0 && brightness<0 && vo_aspect_ratio<0)
    this->video_properties_saved = 0;

  pthread_mutex_unlock(&this->lock);
  return 0;
}

static int set_live_mode(vdr_input_plugin_t *this, int onoff)
{
  pthread_mutex_lock(&this->lock);

  if(this->live_mode != onoff) {
    config_values_t *config = this->class->xine->config;
    this->live_mode = onoff;

    this->stream->metronom->set_option(this->stream->metronom, 
				       METRONOM_PREBUFFER, METRONOM_PREBUFFER_VAL);

    if(this->live_mode || (this->fd_control >= 0 && !this->slave_stream)) 
      config->update_num(config, "audio.synchronization.av_sync_method", 1);
#if 0
    /* does not work after playing music files (?) */
    else 
      config->update_num(config, "audio.synchronization.av_sync_method", 0);
#endif

  }

  /* set buffer usage limits */
  this->max_buffers = this->buffer_pool->buffer_pool_capacity;
  if(this->live_mode && this->fd_control < 0) 
    this->max_buffers >>= 1;
  this->max_buffers -= 10;

  if(this->no_video)
    this->max_buffers = RADIO_MAX_BUFFERS;

  /* SCR tunning */
  if(this->live_mode) {
#ifndef TEST_SCR_PAUSE
    LOGSCR("pause scr tunning by set_live_mode");
    scr_tunning_set_paused(this);
#endif
  } else {
    LOGSCR("reset scr tunning by set_live_mode");
    reset_scr_tunning(this, this->speed_before_pause=XINE_FINE_SPEED_NORMAL);
  }

  this->still_mode = 0;
  _x_stream_info_set(this->stream, XINE_STREAM_INFO_VIDEO_HAS_STILL, this->still_mode);

  pthread_mutex_unlock(&this->lock);

  signal_buffer_pool_not_empty(this);
  return 0;
}

static int  set_playback_speed(vdr_input_plugin_t *this, int speed)
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
  pthread_mutex_lock(&this->lock);
  this->is_paused = 0;
  if(speed == 0) {
    this->is_paused = 1;
  } else if(speed>64 || speed<-64) {
    pthread_mutex_unlock(&this->lock);
    return -2;
  }

  if(speed > 1 || speed < -1) {
    reset_scr_tunning(this, -1);
    this->is_trickspeed = 1;
  } else {
    this->is_trickspeed = 0;
  }

  _x_stream_info_set(this->stream, XINE_STREAM_INFO_VIDEO_HAS_STILL, this->still_mode || speed==0);

  if(speed>0)
    speed = this->speed_before_pause = XINE_FINE_SPEED_NORMAL/speed;
  else
    speed = this->speed_before_pause = XINE_FINE_SPEED_NORMAL*(-speed);

  if(this->scr_tunning != SCR_TUNNING_PAUSED && 
     _x_get_fine_speed(this->stream) != speed) {
    _x_set_fine_speed (this->stream, speed);
  }

  if(this->slave_stream) 
    _x_set_fine_speed (this->slave_stream, speed);

  pthread_mutex_unlock(&this->lock);
  return 0;
}

static void send_meta_info(vdr_input_plugin_t *this)
{
  if(this->slave_stream) {

    /* send stream meta info */
    char *meta         = NULL;
    char *title        = (char *)xine_get_meta_info(this->slave_stream, XINE_META_INFO_TITLE);
    char *artist       = (char *)xine_get_meta_info(this->slave_stream, XINE_META_INFO_ARTIST);
    char *album        = (char *)xine_get_meta_info(this->slave_stream, XINE_META_INFO_ALBUM);
    char *tracknumber  = (char *)xine_get_meta_info(this->slave_stream, XINE_META_INFO_TRACK_NUMBER);

    if (asprintf(&meta,
                 "INFO METAINFO title=@%s@ artist=@%s@ album=@%s@ tracknumber=@%s@\r\n",
                 title?:"", artist?:"", album?:"", tracknumber?:"") < 0)
      return;

    if(this->fd_control < 0)
      this->funcs.xine_input_event(meta, NULL);
    else
      write_control(this, meta);

    free(meta);
  }
}

static void send_cd_info(vdr_input_plugin_t *this)
{
#if 0
  // get_autoplay_list stops replay ...
  int count = 0;
  input_class_t *c = this->slave_stream->input_plugin->input_class;
  char **list = c->get_autoplay_list(c, &count);
  if(list) {
    int i;
    LOGMSG("cdda: %d entries", count);
    for(i=0; i<count && list[i]; i++)
      LOGMSG("cdda: %d: %s", i, list[i]);
  }
  this->autoplay_size = count;
#endif
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

  memset(&sghdr, 0, sizeof(sghdr));
  memset(buffer, 0, sizeof(buffer));
  memset(sense, 0, sizeof(sense));
  memset(cmd, 0, sizeof(cmd));
  memset(&st, 0, sizeof(st));

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
    this->slave_stream->spu_channel_user = SPU_CHANNEL_AUTO;
    this->slave_stream->spu_channel = this->slave_stream->spu_channel_auto;
  } else {
    LOGDBG("dvd_menu_domain(0)");
    this->dvd_menu = 0;
  }
}

static int handle_control_playfile(vdr_input_plugin_t *this, const char *cmd)
{
  const char *pt = cmd + 9;
  char filename[4096], av[256], *pav = av;
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

    if(this->slave_stream)
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
	  char mrl[sizeof(filename)+256], mrlbase[256];
	  char *host = strdup(strstr(this->mrl, "//")+2);
	  char *port = strchr(host, ':');
	  char *sub  = strstr(filename, "#subtitle:");
	  int  iport = port ? atoi(port+1) : DEFAULT_VDR_PORT;
	  if(port) *port = 0;
	  if(sub) *sub = 0;
	  snprintf(mrlbase, sizeof(mrlbase), "http://%s:%d/PLAYFILE", 
		   host?:"127.0.0.1", iport);
	  sprintf(mrl, "%s%s", mrlbase, filename + is_file_mrl);
	  if(sub) {
	    sub += 10; /*strlen("#subtitle:");*/
	    strcat(mrl, "#subtitle:");
	    strcat(mrl, mrlbase);
	    strcat(mrl, sub);
	  }
	  free(host);
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

    if(!this->slave_stream) {
      this->slave_stream = xine_stream_new(this->class->xine, 
					   this->stream->audio_out, 
					   this->stream->video_out);
    }

    if(!this->slave_event_queue) {
      this->slave_event_queue = xine_event_new_queue (this->slave_stream);
      xine_event_create_listener_thread (this->slave_event_queue, 
					 vdr_event_cb, this);
    }
    select_spu_channel(this->slave_stream, SPU_CHANNEL_AUTO);
    this->dvd_menu = 0;

    errno = 0;
    err = !xine_open(this->slave_stream, filename);
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
      set_playback_speed(this, 1);
      this->live_mode = 1;
      set_live_mode(this, 0);
      set_playback_speed(this, 1);
      reset_scr_tunning(this, this->speed_before_pause = XINE_FINE_SPEED_NORMAL);
      this->slave_stream->metronom->set_option(this->slave_stream->metronom, 
					       METRONOM_PREBUFFER, 90000);
#endif
      if(!strncmp(filename, "cdda:", 5))
	send_cd_info(this);

      this->loop_play = loop;
      err = !xine_play(this->slave_stream, 0, 1000 * pos);
      if(err) {
	LOGMSG("Error playing file");
	*filename = 0; /* this triggers stop */
      } else {
	send_meta_info(this);

	if(this->funcs.fe_control) {
	  char tmp[128];
	  int has_video;
	  sprintf(tmp, "SLAVE 0x%lx %s\r\n", 
		  (unsigned long int)this->slave_stream,
		  mix_streams ? av : "");
	  this->funcs.fe_control(this->funcs.fe_handle, tmp);
	  has_video = _x_stream_info_get(this->slave_stream, XINE_STREAM_INFO_HAS_VIDEO);

	  if(has_video) {
	    int stream_width, stream_height;
	    stream_width = _x_stream_info_get(this->slave_stream, XINE_STREAM_INFO_VIDEO_WIDTH);
	    stream_height = _x_stream_info_get(this->slave_stream, XINE_STREAM_INFO_VIDEO_HEIGHT);

	    if(stream_width != this->video_width || stream_height != this->video_height) {
	      this->video_width = stream_width;
	      this->video_height = stream_height;
	      this->video_changed = 1;
	    }
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
    if(this->slave_stream) {
      int stream_width, stream_height;
      xine_stop(this->slave_stream);

      if (this->slave_event_queue) {
	xine_event_dispose_queue (this->slave_event_queue);
	this->slave_event_queue = NULL;
      }

      if(this->funcs.fe_control) {
	this->funcs.fe_control(this->funcs.fe_handle, "POST 0 Off\r\n");
	this->funcs.fe_control(this->funcs.fe_handle, "SLAVE 0x0\r\n");
      }
      xine_close(this->slave_stream);
      xine_dispose(this->slave_stream);
      this->slave_stream = NULL;

      if(this->funcs.fe_control)
	this->funcs.fe_control(this->funcs.fe_handle, "SLAVE CLOSED\r\n");

      stream_width = _x_stream_info_get(this->stream, XINE_STREAM_INFO_VIDEO_WIDTH);
      stream_height = _x_stream_info_get(this->stream, XINE_STREAM_INFO_VIDEO_HEIGHT);

      if(stream_width != this->video_width || stream_height != this->video_height) {
	this->video_width = stream_width;
	this->video_height = stream_height;
	this->video_changed = 1;
      }

      _x_demux_control_start(this->stream);

#ifdef DVD_STREAMING_SPEED
      dvd_set_speed(NULL, -1);
#endif
    }
  }

  return err ? CONTROL_PARAM_ERROR : CONTROL_OK;
}

static int handle_control_grab(vdr_input_plugin_t *this, const char *cmd)
{
  int quality, width, height, jpeg;
  jpeg = !strcmp(cmd+5,"JPEG");

  if(3 == sscanf(cmd+5+4, "%d %d %d", &quality, &width, &height)) {
 
    if(this->fd_control >= 0) {

      grab_data_t *data = NULL; 
      uint64_t t = monotonic_time_ms();
      LOGDBG("GRAB: jpeg=%d quality=%d w=%d h=%d", jpeg, quality, width, height);  

      /* grab takes long time and we don't want to lose data connection 
	 or interrupt video ... */
      if(pthread_mutex_unlock(&this->vdr_entry_lock))
	LOGERR("pthread_mutex_unlock failed");

      if(this->funcs.fe_control)
	data = (grab_data_t*)(this->funcs.fe_control(this->funcs.fe_handle, cmd));
      
      if(data && data->size>0 && data->data) {
	char s[128];
	sprintf(s, "GRAB %d %d\r\n", this->token, data->size);
	pthread_mutex_lock (&this->fd_control_lock);
	write_control_data(this, s, strlen(s));
	write_control_data(this, data->data, data->size);
	if(pthread_mutex_unlock (&this->fd_control_lock))
	  LOGERR("pthread_mutex_unlock failed");
      } else {
	/* failed */
	printf_control(this, "GRAB %d 0\r\n", this->token);
      }

      pthread_mutex_lock(&this->vdr_entry_lock);

      if(data)
	free(data->data);
      free(data);

      LOGDBG("grab took %d ms", (int)(monotonic_time_ms() - t));
      return CONTROL_OK;
    }
  }

  return CONTROL_PARAM_ERROR;
}

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

static int handle_control_osdcmd(vdr_input_plugin_t *this)
{
  osd_command_t osdcmd;
  int err = CONTROL_OK;

  if(!this->control_running)
    return CONTROL_DISCONNECTED;

  if(read_control(this, (unsigned char*)&osdcmd, sizeof(osd_command_t))
     != sizeof(osd_command_t)) {
    LOGMSG("control: error reading OSDCMD data");
    return CONTROL_DISCONNECTED;
  }

  ntoh_osdcmd(osdcmd);

  if(osdcmd.palette && osdcmd.colors>0) {
    int bytes = sizeof(xine_clut_t)*(osdcmd.colors);
    osdcmd.palette = malloc(bytes);
    if(read_control(this, (unsigned char *)osdcmd.palette, bytes)
       != bytes) {
      LOGMSG("control: error reading OSDCMD palette");
      err = CONTROL_DISCONNECTED;
    }
  } else {
    osdcmd.palette = NULL;
  }

  if(err == CONTROL_OK && osdcmd.data && osdcmd.datalen>0) {
    osdcmd.data = (xine_rle_elem_t*)malloc(osdcmd.datalen);
    if(read_control(this, (unsigned char *)osdcmd.data, osdcmd.datalen)
       != osdcmd.datalen) {
      LOGMSG("control: error reading OSDCMD bitmap");
      err = CONTROL_DISCONNECTED;
    } else {
      uint8_t *raw = osdcmd.raw_data;
      osdcmd.data = uncompress_osd_net(raw, osdcmd.num_rle, osdcmd.datalen);
      osdcmd.datalen = osdcmd.num_rle*4;
      free(raw);
    }
  } else {
    osdcmd.data = NULL;
  }

  if(err == CONTROL_OK) 
    err = vdr_plugin_exec_osd_command((input_plugin_t*)this, &osdcmd);

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

static int vdr_plugin_poll(vdr_input_plugin_t *this, int timeout_ms)
{
  struct timespec abstime;
  int result = 0;

  /* Caller must have locked this->vdr_entry_lock ! */

  if(this->slave_stream) {
    LOGMSG("vdr_plugin_poll: called while playing slave stream !");
    return 1;
  }

  TRACE("vdr_plugin_poll (%d ms), buffer_pool: blocks=%d, bytes=%d", 
	timeout_ms, this->buffer_pool->size(this->buffer_pool), 
	this->buffer_pool->data_size(this->buffer_pool));
#if 0
  if(this->is_paused) {
    VDR_ENTRY_UNLOCK();

    create_timeout_time(&abstime, timeout_ms);
    pthread_mutex_lock (&this->buffer_pool->buffer_pool_mutex);
    while(pthread_cond_timedwait (&this->buffer_pool->buffer_pool_cond_not_empty,
				  &this->buffer_pool->buffer_pool_mutex, 
				  &abstime) != ETIMEDOUT)
      ;
    pthread_mutex_unlock (&this->buffer_pool->buffer_pool_mutex);
    timeout_ms = 0;

    VDR_ENTRY_LOCK(0);
  }
#endif
  pthread_mutex_lock (&this->buffer_pool->buffer_pool_mutex);
  result = this->buffer_pool->buffer_pool_num_free - 
           (this->buffer_pool->buffer_pool_capacity - this->max_buffers);
  pthread_mutex_unlock (&this->buffer_pool->buffer_pool_mutex);

  if(timeout_ms > 0 && result <= 0) {
    if(timeout_ms > 250) {
      LOGMSG("vdr_plugin_poll: timeout too large (%d ms), forced to 250ms", timeout_ms);
      timeout_ms = 250;
    }
    create_timeout_time(&abstime, timeout_ms);
    pthread_mutex_lock(&this->lock);
    if(this->scr_tunning == SCR_TUNNING_PAUSED) {
      LOGSCR("scr tunning reset by POLL");
      reset_scr_tunning(this,this->speed_before_pause);
    }
    pthread_mutex_unlock(&this->lock);

    signal_buffer_not_empty(this);

    VDR_ENTRY_UNLOCK();

    pthread_mutex_lock (&this->buffer_pool->buffer_pool_mutex);
    while(result <= 5) {
      if(pthread_cond_timedwait (&this->buffer_pool->buffer_pool_cond_not_empty,
				 &this->buffer_pool->buffer_pool_mutex, 
				 &abstime) == ETIMEDOUT)
	break;      
      result = this->buffer_pool->buffer_pool_num_free - 
	       (this->buffer_pool->buffer_pool_capacity - this->max_buffers);
    }
    pthread_mutex_unlock (&this->buffer_pool->buffer_pool_mutex);
    VDR_ENTRY_LOCK(0);
  }

  TRACE("vdr_plugin_poll returns, %d free (%d used, %d bytes)\n", result, 
	this->buffer_pool->size(this->buffer_pool), 
	this->buffer_pool->data_size(this->buffer_pool));

 /* handle priority problem in paused mode when 
    data source has higher priority than control source */
  if(result <= 0) {
    result = 0;
    xine_usec_sleep(3*1000);
  }

  return result;
}

/*
 * Flush returns 0 if there is no data or frames in stream buffers 
 */
static int vdr_plugin_flush(vdr_input_plugin_t *this, int timeout_ms)
{
  struct timespec abstime;
  fifo_buffer_t *pool   = this->buffer_pool;
  fifo_buffer_t *buffer = this->block_buffer;
  int result = 0, waitresult=0;

  /* Caller must have locked this->vdr_entry_lock ! */

  if(this->slave_stream) {
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

  if(result>0) {
    put_control_buf(buffer, pool, BUF_CONTROL_FLUSH_DECODER);
    put_control_buf(buffer, pool, BUF_CONTROL_NOP);
  }

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

static int vdr_plugin_flush_remote(vdr_input_plugin_t *this, int timeout_ms, 
				   uint64_t offset, int frame)
{
  int r, live_mode;

  pthread_mutex_lock(&this->lock);

  live_mode = this->live_mode;
  this->live_mode = 0; /* --> 1 again when data arrives ... */

  LOGSCR("reset scr tunning by flush_remote");
  reset_scr_tunning(this, this->speed_before_pause);

  /* wait until all data has been received */
  while(this->curpos < offset && timeout_ms > 0) {
    TRACE("FLUSH: wait position (%" PRIu64 " ; need %" PRIu64 ")", 
	  this->curpos, offset);
    pthread_mutex_unlock(&this->lock);
    xine_usec_sleep(3*1000);
    pthread_mutex_lock(&this->lock);
    timeout_ms -= 3;
  }

  LOGSCR("reset scr tunning by flush_remote");
  reset_scr_tunning(this, this->speed_before_pause);

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

static int vdr_plugin_parse_control(input_plugin_t *this_gen, const char *cmd)
{
  vdr_input_plugin_t *this = (vdr_input_plugin_t *) this_gen;
  int err = CONTROL_OK, i, j;
  int /*int32_t*/ tmp32 = 0;
  uint64_t tmp64 = 0ULL;
  xine_stream_t *stream = this->stream;
  static const char str_poll[] = "POLL";
  char *pt;

  VDR_ENTRY_LOCK(CONTROL_DISCONNECTED);

  LOGCMD("vdr_plugin_parse_control: %s", cmd); 

  if( *((uint32_t*)cmd) == *((uint32_t*)str_poll) ||
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

  if(this->slave_stream)
    stream = this->slave_stream;

  if(NULL != (pt = strstr(cmd, "\r\n")))
    *((char*)pt) = 0; /* auts */

  if(!strncasecmp(cmd, "OSDCMD", 6)) {
    err = handle_control_osdcmd(this);

  } else if(!strncasecmp(cmd, "VIDEO_PROPERTIES ", 17)) {
    int hue, saturation, brightness, contrast, vo_aspect_ratio;
    if(5 == sscanf(cmd, "VIDEO_PROPERTIES %d %d %d %d %d", 
		   &hue, &saturation, &brightness, &contrast, &vo_aspect_ratio))
      err = set_video_properties(this, hue, saturation, brightness, contrast, vo_aspect_ratio);
    else
      err = CONTROL_PARAM_ERROR;

  } else if(!strncasecmp(cmd, "OVERSCAN ", 9)) {
    if(!this->funcs.fe_control)
      LOGMSG("No fe_control function! %s failed.", cmd);
    else
      this->funcs.fe_control(this->funcs.fe_handle, cmd);

  } else if(!strncasecmp(cmd, "VO_ASPECT ", 10)) {
    if(1 == sscanf(cmd, "VO_ASPECT %d", &tmp32)) {
      xine_set_param(stream, XINE_PARAM_VO_ASPECT_RATIO, tmp32);
    } else
      err = CONTROL_PARAM_ERROR;

  } else if(!strncasecmp(cmd, "DEINTERLACE ", 12)) {
    if(this->fd_control < 0)
      err = set_deinterlace_method(this, cmd+12);

  } else if(!strncasecmp(cmd, "EVENT ", 6)) {
    int i;
    char *pt = strchr(cmd, '\n');
    if(pt) *pt=0;
    pt = strstr(cmd+6, " CHAPTER");
    if(pt) {
      *pt = 0;
      this->class->xine->config->update_num(this->class->xine->config,
					    "media.dvd.skip_behaviour", 1);
    }
    pt = strstr(cmd+6, " TITLE");
    if(pt) {
      *pt = 0;
      this->class->xine->config->update_num(this->class->xine->config,
					    "media.dvd.skip_behaviour", 2);
    }
    for(i=0; i<sizeof(eventmap)/sizeof(eventmap[0]); i++)
      if(!strcmp(cmd+6, eventmap[i].name)) {
	xine_event_t ev;
	ev.type = eventmap[i].type;
	ev.stream = this->slave_stream ?: this->stream;
	/* tag event to prevent circular input events 
	   (vdr -> here -> event_listener -> vdr -> ...) */
	ev.data = "VDR"; 
	ev.data_length = 4;
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
    if(1 == sscanf(cmd, "HDMODE %d", &tmp32)) {
      pthread_mutex_lock(&this->lock);
      if(tmp32) {
	if(!this->hd_buffer)
	  this->hd_buffer = fifo_buffer_new(this->stream, HD_BUF_NUM_BUFS, HD_BUF_ELEM_SIZE);
	this->hd_stream = 1;
      } else {
	this->hd_stream = 0;
      }
      pthread_mutex_unlock(&this->lock);
    }

  } else if(!strncasecmp(cmd, "NOVIDEO ", 8)) {
    if(1 == sscanf(cmd, "NOVIDEO %d", &tmp32)) {
      pthread_mutex_lock(&this->lock);
      this->no_video = tmp32;
      if(this->no_video) {
        this->max_buffers = RADIO_MAX_BUFFERS;
      } else {
        this->max_buffers = this->buffer_pool->buffer_pool_capacity;
        if(!this->live_mode && this->fd_control < 0) 
	  this->max_buffers >>= 1;
        this->max_buffers -= 10;
      }
      pthread_mutex_unlock(&this->lock);
    } else
      err = CONTROL_PARAM_ERROR;

    signal_buffer_pool_not_empty(this);

  } else if(!strncasecmp(cmd, "DISCARD ", 8)) {
    if(2 == sscanf(cmd, "DISCARD %" PRIu64 " %d", &tmp64, &tmp32)) {
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
    if(1 == sscanf(cmd, "STREAMPOS %" PRIu64, &tmp64)) {
      pthread_mutex_lock(&this->lock);
      vdr_flush_engine(this, tmp64);
      this->curpos = tmp64;
      this->discard_index = this->curpos;
      this->guard_index = 0;
      pthread_mutex_unlock(&this->lock);
    } else 
      err = CONTROL_PARAM_ERROR;

  } else if(!strncasecmp(cmd, "TRICKSPEED ", 11)) {
    err = (1 == sscanf(cmd, "TRICKSPEED %d", &tmp32)) ? 
      set_playback_speed(this, tmp32) : 
      CONTROL_PARAM_ERROR;    

  } else if(!strncasecmp(cmd, "STILL ", 6)) {
    pthread_mutex_lock(&this->lock);
    /*if(this->fd_control >= 0) {*/
      if(1 == sscanf(cmd, "STILL %d", &tmp32)) {
	this->still_mode = tmp32;
	if(this->still_mode)
	  reset_scr_tunning(this, this->speed_before_pause);
	_x_stream_info_set(this->stream, XINE_STREAM_INFO_VIDEO_HAS_STILL, this->still_mode);
	this->stream_start = 1;
      } else
	err = CONTROL_PARAM_ERROR;
    /*}*/
    pthread_mutex_unlock(&this->lock);

  } else if(!strncasecmp(cmd, "SCR ", 4)) {
    pthread_mutex_lock(&this->lock);
    if(1 == sscanf(cmd, "SCR Sync %d", &tmp32)) {
      this->scr_live_sync = 1;
      pvrscr_speed_base(this->scr, tmp32);
    }
    else if(1 == sscanf(cmd, "SCR NoSync %d", &tmp32)) {
      this->scr_live_sync = 0;
      pvrscr_speed_base(this->scr, tmp32);
      reset_scr_tunning(this, -1);
    }
    pthread_mutex_unlock(&this->lock);

  } else if(!strncasecmp(cmd, "LIVE ", 5)) {
    this->still_mode = 0;
    _x_stream_info_set(this->stream, XINE_STREAM_INFO_VIDEO_HAS_STILL, this->still_mode);
    err = (1 == sscanf(cmd, "LIVE %d", &tmp32)) ?
           set_live_mode(this, tmp32) : -2 ;
    
  } else if(!strncasecmp(cmd, "MASTER ", 7)) {
    if(1 == sscanf(cmd, "MASTER %d", &tmp32))
      this->fixed_scr = tmp32 ? 1 : 0;
    else
      err = CONTROL_PARAM_ERROR;

  } else if(!strncasecmp(cmd, "VOLUME ", 7)) {
    if(1 == sscanf(cmd, "VOLUME %d", &tmp32)) {
      int sw = strstr(cmd, "SW") ? 1 : 0;
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
	  xine_set_param(stream, XINE_PARAM_AUDIO_MUTE, 0);
	} else {
	  xine_set_param(stream, XINE_PARAM_AUDIO_AMP_LEVEL, 100);
	  xine_set_param(stream, XINE_PARAM_AUDIO_AMP_MUTE, 0);
	}
      }
    } else
      err = CONTROL_PARAM_ERROR;

  } else if(!strncasecmp(cmd, "AUDIOCOMPRESSION ",17)) {
    if(1 == sscanf(cmd, "AUDIOCOMPRESSION %d", &tmp32)) {
      xine_set_param(stream, XINE_PARAM_AUDIO_COMPR_LEVEL, tmp32);
    } else
      err = CONTROL_PARAM_ERROR;

  } else if(!strncasecmp(cmd, "AUDIOSURROUND ",14)) {
    if(1 == sscanf(cmd, "AUDIOSURROUND %d", &tmp32)) {
      this->class->xine->config->update_num(this->class->xine->config,
					    "audio.a52.surround_downmix", tmp32?1:0);
    } else
      err = CONTROL_PARAM_ERROR;

  } else if(!strncasecmp(cmd, "SPEAKERS ",9)) {
    if(1 == sscanf(cmd, "SPEAKERS %d", &tmp32)) {
      this->class->xine->config->update_num(this->class->xine->config,
					    "audio.output.speaker_arrangement", tmp32);
    } else
      err = CONTROL_PARAM_ERROR;

  } else if(!strncasecmp(cmd, "EQUALIZER ", 10)) {
    int eqs[XINE_PARAM_EQ_16000HZ - XINE_PARAM_EQ_30HZ + 2] = {0};
    sscanf(cmd,"EQUALIZER %d %d %d %d %d %d %d %d %d %d",
	   eqs,eqs+1,eqs+2,eqs+3,eqs+4,eqs+5,eqs+6,eqs+7,eqs+8,eqs+9);
    for(i=XINE_PARAM_EQ_30HZ,j=0; i<=XINE_PARAM_EQ_16000HZ; i++,j++)
      xine_set_param(stream, i, eqs[j]);

  } else if(!strncasecmp(cmd, "AUDIOSTREAM ", 12)) {
    if(!this->slave_stream) {
      int ac3 = strncmp(cmd+12, "AC3", 3) ? 0 : 1;
      if(1 == sscanf(cmd+12 + 4*ac3, "%d", &tmp32)) {
	pthread_mutex_lock(&this->lock);
	this->audio_stream_id = tmp32;
	pthread_mutex_unlock(&this->lock);
      } else {
	err = CONTROL_PARAM_ERROR;
      }
    } else {
      if(1 == sscanf(cmd, "AUDIOSTREAM AC3 %d", &tmp32)) {
	tmp32 &= 0xff;
	LOGDBG("Audio channel -> [%d]", tmp32);
	xine_set_param(stream, XINE_PARAM_AUDIO_CHANNEL_LOGICAL, tmp32);
      }
      LOGDBG("Audio channel selected: [%d]", _x_get_audio_channel (stream));
    }

  } else if(!strncasecmp(cmd, "SPUSTREAM ", 10)) {
    int old_ch  = _x_get_spu_channel (stream);
    int max_ch  = xine_get_stream_info(stream, XINE_STREAM_INFO_MAX_SPU_CHANNEL);
    int ch      = old_ch;
    int ch_auto = strstr(cmd+10, "auto") ? 1 : 0;

    if(strstr(cmd, "NEXT"))
      ch = ch < max_ch ? ch+1 : -2;
    else if(strstr(cmd, "PREV"))
      ch = ch > -2 ? ch-1 : max_ch-1;
    else if(1 == sscanf(cmd, "SPUSTREAM %d", &tmp32)) {
      ch = tmp32;
    } else if(cmd[10] && cmd[11] && (cmd[12] < 'a' || cmd[12] > 'z')) {
      /* ISO 639-1 language code */
      const char spu_lang[3] = {cmd[10], cmd[11], 0};
      LOGMSG("Preferred SPU language: %s", spu_lang);
      this->class->xine->config->update_string(this->class->xine->config,
					    "media.dvd.language", spu_lang);
      ch = old_ch = 0;
    } else
      err = CONTROL_PARAM_ERROR;

    if (old_ch == SPU_CHANNEL_AUTO)
      old_ch = stream->spu_channel_auto;

    if (ch != old_ch) {
      if (ch_auto && stream->spu_channel_user == SPU_CHANNEL_AUTO) {
	LOGDBG("Automatic SPU channel %d->%d ignored", old_ch, ch);
      } else {
	LOGDBG("Forced SPU channel %d->%d", old_ch, ch);
	select_spu_channel(stream, ch);
      }
      LOGDBG("SPU channel selected: [%d]", _x_get_spu_channel (stream));
    }

  } else if(!strncasecmp(cmd, "AUDIODELAY ", 11)) {
    if(1 == sscanf(cmd, "AUDIODELAY %d", &tmp32))
      xine_set_param(stream, XINE_PARAM_AV_OFFSET, tmp32*90000/1000);
    else
      err = CONTROL_PARAM_ERROR;

  } else if(!strncasecmp(cmd, "SYNC ", 5)) {
    if(this->fd_control >= 0)
      write_control(this, cmd);

  } else if(!strncasecmp(cmd, "GETSTC", 6)) {
    int64_t pts = xine_get_current_vpts(stream) -
                  stream->metronom->get_option(stream->metronom, 
					       METRONOM_VPTS_OFFSET);
    if(this->fd_control >= 0) {
      printf_control(this, "STC %" PRId64 "\r\n", pts);
    } else {
      *((int64_t *)cmd) = pts;
    }

  } else if(!strncasecmp(cmd, "FLUSH ", 6)) {
    if(1 == sscanf(cmd, "FLUSH %d", &tmp32)) {
      if(this->fd_control >= 0) {
	uint32_t frame = 0;
	tmp64 = 0ULL; 
	tmp32 = 0;
	sscanf(cmd, "FLUSH %d %" PRIu64 " %d", &tmp32, &tmp64, &frame);
	err = vdr_plugin_flush_remote(this, tmp32, tmp64, frame);
      } else {
	err = vdr_plugin_flush(this, tmp32);
      }
    } else
      err = CONTROL_PARAM_ERROR;

  } else if(!strncasecmp(cmd, "TOKEN ", 6)) {
    this->token = atoi(cmd+6);

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
    if(this->slave_stream) {
      int pos_stream=0, pos_time=0, length_time=0;
      xine_get_pos_length(this->slave_stream, 
			  &pos_stream, &pos_time, &length_time);
      if(cmd[5]=='+')
	pos_time += atoi(cmd+6) * 1000;
      else if(cmd[5]=='-')
	pos_time -= atoi(cmd+6) * 1000;
      else
	pos_time = atoi(cmd+5) * 1000;
      err = xine_play (this->slave_stream, 0, pos_time);
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
      char **list;
      if(this->slave_stream &&
	 this->slave_stream->input_plugin &&
	 this->slave_stream->input_plugin->input_class)
	list = this->slave_stream->input_plugin->input_class->
	  get_autoplay_list(this->slave_stream->input_plugin->input_class, &this->autoplay_size);
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
    if(this->slave_stream) {
      int vpos = 0;
      if(1 == sscanf(cmd, "SUBTITLES %d", &vpos))
	this->class->xine->config->update_num(this->class->xine->config,
		          "subtitles.separate.vertical_offset", vpos);
      else
	err = CONTROL_PARAM_ERROR;
    }

  } else if(!strncasecmp(cmd, "EXTSUBSIZE ", 11)) {
    int size = 0;
    if(1 == sscanf(cmd, "EXTSUBSIZE %d", &size))
      /* size of separate subtitles :
	 -1 = xine default 
	 0...6 = { tiny  small  normal  large  very large  huge } */
      this->class->xine->config->update_num(this->class->xine->config, 
					    "subtitles.separate.subtitle_size", size);
    else
      err = CONTROL_PARAM_ERROR;

  } else if(!strncasecmp(cmd, "GRAB ", 5)) {
    handle_control_grab(this, cmd);

  /* next ones need to be synchronized to data stream */
  } else if(!strncasecmp(cmd, "BLANK", 5)) {
    put_control_buf(this->block_buffer, this->buffer_pool, CONTROL_BUF_BLANK);

  } else if(!strncasecmp(cmd, "CLEAR", 5)) {
    /* #warning should be delayed and executed in read_block */

  } else {
    LOGMSG("unknown control %s", cmd);
    err = CONTROL_UNKNOWN;
  }

  LOGCMD("vdr_plugin_parse_control: DONE (%d): %s", err, cmd);

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
    switch(err = vdr_plugin_parse_control(this_gen, line)) {
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

  if(this->slave_stream)
    xine_stop(this->slave_stream);

  LOGDBG("Control thread terminated");
  pthread_exit(NULL);
}

/**************************** Control to VDR ********************************/

static void slave_track_maps_changed(vdr_input_plugin_t *this)
{
  char tracks[1024], lang[128];
  int i, current, n = 0;
  size_t cnt;

  /* DVD title and menu domain detection */  
#ifdef XINE_STREAM_INFO_DVD_TITLE_NUMBER
  i = _x_stream_info_get(this->slave_stream, XINE_STREAM_INFO_DVD_TITLE_NUMBER);
  if(i >= 0) {
    if (i == 0)
      dvd_menu_domain(this, 1);
    sprintf(tracks, "INFO DVDTITLE %d\r\n", i);
    if(this->funcs.xine_input_event)
      this->funcs.xine_input_event(tracks, NULL);
    else
      write_control(this, tracks);
    LOGDBG("%s", tracks);
  }
#endif

  /* Audio tracks */
  
  strcpy(tracks, "INFO TRACKMAP AUDIO ");
  cnt = strlen(tracks);
  current = xine_get_param(this->slave_stream, XINE_PARAM_AUDIO_CHANNEL_LOGICAL);
  for(i=0; i<32 && cnt<sizeof(tracks)-32; i++)
    if(xine_get_audio_lang(this->slave_stream, i, lang)) {
      while(lang[0]==' ') strcpy(lang, lang+1);
      cnt += snprintf(tracks+cnt, sizeof(tracks)-cnt-32, 
		      "%s%d:%s ", i==current?"*":"", i, lang);
      n++;
    }
  tracks[sizeof(tracks)-1] = 0;
  if(n>1)
    LOGDBG("%s", tracks);

  if(this->funcs.xine_input_event) {
    /* local mode: -> VDR */
    this->funcs.xine_input_event(tracks, NULL);
  } else {
    /* remote mode: -> connection -> VDR */
    strcpy(tracks+cnt, "\r\n");
    write_control(this, tracks);
  }
  
  /* DVD SPU tracks */

  n = 0;  
  strcpy(tracks, "INFO TRACKMAP SPU ");
  cnt = strlen(tracks);
  current = _x_get_spu_channel (this->slave_stream);
  if(current < 0) {
    /* -2 == none, -1 == auto */
    cnt += snprintf(tracks+cnt, sizeof(tracks)-cnt-32,
		    "*%d:%s ", current, 
		    current==SPU_CHANNEL_NONE ? "none" : "auto");
    n++;
    if(current == SPU_CHANNEL_AUTO)
      current = this->slave_stream->spu_channel_auto;
  }
  for(i=0; i<32 && cnt<sizeof(tracks)-32; i++)
    if(xine_get_spu_lang(this->slave_stream, i, lang)) {
      while(lang[0]==' ') strcpy(lang, lang+1);
      cnt += snprintf(tracks+cnt, sizeof(tracks)-cnt-32,
		      "%s%d:%s ", i==current?"*":"", i, lang);
      n++;
    }
  tracks[sizeof(tracks)-1] = 0;
  if(n>1)
    LOGDBG("%s", tracks);
  
  if(this->funcs.xine_input_event) {
    this->funcs.xine_input_event(tracks, NULL);
  } else {
    strcpy(tracks+cnt, "\r\n");
    write_control(this, tracks);
  }
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
  int i;

  for(i=0; i < sizeof(vdr_keymap)/sizeof(vdr_keymap[0]); i++) {
    if(event->type == vdr_keymap[i].event) {
      if(event->data && event->data_length == 4 && 
	 !strncmp(event->data, "VDR", 4)) {
	/*LOGMSG("Input event created by self, ignoring");*/
	return;
      }
      LOGDBG("XINE_EVENT (input) %d --> %s", 
	     event->type, vdr_keymap[i].name);

      if(this->funcs.input_control) {
	/* remote mode: -> input_plugin -> connection -> VDR */
	this->funcs.input_control((input_plugin_t *)this,
				  NULL, vdr_keymap[i].name, 0, 0);
      }
      if(this->funcs.xine_input_event) {
	/* local mode: -> VDR */
	this->funcs.xine_input_event(NULL, vdr_keymap[i].name);
      }
      return;
    }
  }

  switch (event->type) {
    case XINE_EVENT_UI_SET_TITLE:
      if(event->stream==this->slave_stream) {
	char msg[256], titlen[64] = "";
	xine_ui_data_t *data = (xine_ui_data_t *)event->data;
	LOGMSG("XINE_EVENT_UI_SET_TITLE: %s", data->str);

#ifdef XINE_STREAM_INFO_DVD_TITLE_NUMBER
	int tt = _x_stream_info_get(this->slave_stream,XINE_STREAM_INFO_DVD_TITLE_NUMBER);
	snprintf(titlen, sizeof(titlen), "INFO DVDTITLE %d\r\n", tt);
	if (tt == 0) 
	  dvd_menu_domain(this, 1);
#endif
	snprintf(msg, sizeof(msg), "INFO TITLE %s\r\n%s", data->str, titlen);
	msg[sizeof(msg)-1] = 0;
	if(this->funcs.xine_input_event) 
	  this->funcs.xine_input_event(msg, NULL);
	else
	  write_control(this, msg);
	break;
      }

    case XINE_EVENT_UI_NUM_BUTTONS:
      if (event->stream == this->slave_stream) {
	xine_ui_data_t *data = (xine_ui_data_t*)event->data;
	char msg[64];
	dvd_menu_domain(this, data->num_buttons > 0);
	snprintf(msg, sizeof(msg), "INFO DVDBUTTONS %d\r\n", data->num_buttons);
	msg[sizeof(msg)-1] = 0;
	if (this->funcs.xine_input_event) 
	  this->funcs.xine_input_event(msg, NULL);
	else
	  write_control(this, msg);
	break;
      }

    case XINE_EVENT_UI_CHANNELS_CHANGED:
      if(event->stream==this->slave_stream) 
	slave_track_maps_changed(this);
      break;

    case XINE_EVENT_FRAME_FORMAT_CHANGE:
      {
        xine_format_change_data_t *frame_change = 
	    (xine_format_change_data_t *)event->data;
        LOGOSD("XINE_EVENT_FRAME_FORMAT_CHANGE (%dx%d, aspect=%d)", 
	       frame_change->width, frame_change->height, 
	       frame_change->aspect);
	if(!frame_change->aspect) /* from frontend */
	  vdr_scale_osds(this, frame_change->width, frame_change->height);
#if 0
	if(frame_change->aspect)
	  queue_blank_yv12(this);
#endif
      }
      break;

    case XINE_EVENT_UI_PLAYBACK_FINISHED:
      if(event->stream == this->stream) {
	LOGMSG("XINE_EVENT_UI_PLAYBACK_FINISHED");
	this->control_running = 0;
#if 1
	if(iSysLogLevel > 2) {
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
      } else if(event->stream == this->slave_stream) {
	LOGMSG("XINE_EVENT_UI_PLAYBACK_FINISHED (slave stream)");
	if(this->fd_control >= 0) {
	  write_control(this, "ENDOFSTREAM\r\n");
	} else {
	  if(this->funcs.fe_control) 
	    this->funcs.fe_control(this->funcs.fe_handle, "ENDOFSTREAM\r\n");
#if 0
	  if(!this->loop_play) {
	    /* forward to vdr-fe (listening only VDR stream events) */
	    xine_event_t event;
	    event.data_length = 0;
	    event.type        = XINE_EVENT_UI_PLAYBACK_FINISHED;
	    xine_event_send (this->stream, &event);
	  } else {
# if 0
	    xine_usec_sleep(500*1000);
	    xine_play(this->slave_stream, 0, 0);
# endif
	  }
#endif
	}
      }
      break;

    default:
      LOGCMD("Got an xine event, type 0x%08x", event->type);
      break;
    }
}

/**************************** Data Stream *********************************/

static void data_stream_parse_control(vdr_input_plugin_t *this, char *cmd)
{
  char *tmp;

  cmd[64] = 0;
  if(NULL != (tmp=strchr(cmd, '\r')))
    *tmp = '\0';
  if(NULL != (tmp=strchr(cmd, '\n')))
    *tmp = '\0';

  if(!strncasecmp(cmd, "DISCARD ", 8)) {
    uint64_t index;
    if(1 == sscanf(cmd, "DISCARD %" PRIu64, &index)) {
      struct timespec abstime;
      create_timeout_time(&abstime, 100);

      this->block_buffer->clear(this->block_buffer);

      pthread_mutex_lock(&this->lock);
      while(this->control_running &&
	    this->discard_index < index) {
	LOGDBG("data_stream_parse_control: waiting for engine_flushed condition %"PRIu64"<%"PRIu64,
	       this->discard_index, index);
	pthread_cond_timedwait(&this->engine_flushed, &this->lock, &abstime);
      } 
      pthread_mutex_unlock(&this->lock);
      LOGDBG("data_stream_parse_control: streams synced at %"PRIu64"/%"PRIu64,
	     index, this->discard_index);
    }
    return;
  }

  vdr_plugin_parse_control((input_plugin_t*)this, cmd);
}

static int vdr_plugin_read_net_tcp(vdr_input_plugin_t *this)
{
  buf_element_t *read_buffer = NULL;
  int cnt = 0, todo = 0, n, result, retries = 0;

 retry:
  while(XIO_READY == (result = io_select_rd(this->fd_data))) {

    if(!this->control_running)
      break;

    /* Allocate buffer */
    if(!read_buffer) {

      /* can't cancel if read_buffer != NULL (disposing fifos would freeze) */
      pthread_testcancel();

      read_buffer = get_buf_element(this, 0, 0);
      if(!read_buffer) {
	VDR_ENTRY_LOCK(XIO_ERROR);
	vdr_plugin_poll(this, 100);
	VDR_ENTRY_UNLOCK();

	if(!this->control_running)
	  break;

	read_buffer = get_buf_element(this, 0, 0);
	if(!read_buffer) {
	  /* do not drop any data here ; dropping is done only at server side. */
	  if(!this->is_paused)
	    LOGDBG("TCP: fifo buffer full");
	  xine_usec_sleep(3*1000);
	  continue; /*  must call select to check fd for errors / closing */
	}
      }

      todo = sizeof(stream_tcp_header_t);
      cnt = 0;
    }

    /* Read data */
    errno = 0;
    n = read(this->fd_data, &read_buffer->mem[cnt], todo-cnt);
    if(n <= 0) {
      if(!n || (errno != EINTR && errno != EAGAIN)) {
	if(n<0 && this->fd_data>=0)
	  LOGERR("TCP read error (data stream %d : %d)", this->fd_data, n);
	if(n==0)
	  LOGMSG("Data stream disconnected");
	result = XIO_ERROR;
	break;
      }
      continue;
    }

    cnt += n;

    if(cnt == sizeof(stream_tcp_header_t)) {
      /* Header complete */
      stream_tcp_header_t *hdr = ((stream_tcp_header_t *)read_buffer->content);
      hdr->len = ntohl(hdr->len);
      hdr->pos = ntohull(hdr->pos);

      todo = cnt + hdr->len;
      if(todo+cnt >= read_buffer->max_size) {
	LOGMSG("TCP: Buffer too small (%d ; incoming frame %d bytes)", 
	       read_buffer->max_size, todo + cnt);
	todo = read_buffer->max_size - cnt - 1;
      }
    }

    if(cnt >= todo) {
      /* Buffer complete */
      stream_tcp_header_t *hdr = ((stream_tcp_header_t *)read_buffer->content);
      if(hdr->pos == (uint64_t)(-1ULL) /*0xffffffff ffffffff*/) {
	/* control data */
	uint8_t *pkt_data = read_buffer->content + sizeof(stream_tcp_header_t);
	if(pkt_data[0]) { /* -> can't be pes frame */
	  data_stream_parse_control(this, (char*)pkt_data);

	  /* read next block */
	  todo = sizeof(stream_tcp_header_t);
	  cnt = 0;
	  continue;
	}
      }
 
      /* frame ready */
      read_buffer->size = cnt;
      read_buffer->type = BUF_MAJOR_MASK;
      this->block_buffer->put(this->block_buffer, read_buffer);
      read_buffer = NULL;
    }
  }

  if(read_buffer) {
    if(cnt && this->control_running && result == XIO_TIMEOUT && (++retries < 10)) {
      LOGMSG("TCP: Warning: long delay (>500ms) !");
      goto retry;
    }

    read_buffer->free_buffer(read_buffer);
    read_buffer = NULL;
    if(cnt && this->fd_data >= 0 && result == XIO_TIMEOUT) {
      LOGMSG("TCP: Delay too long, disconnecting");
      this->control_running = 0;
      return XIO_ERROR;
    }
  }

  return result;
}

static int vdr_plugin_read_net_udp(vdr_input_plugin_t *this)
{
  struct sockaddr_in server_address;
  socklen_t address_len = sizeof(server_address);
  udp_data_t *udp = this->udp_data;
  stream_udp_header_t *pkt;
  stream_rtp_header_impl_t *rtp_pkt;
  uint8_t *pkt_data;
  int result = XIO_ERROR, n, current_seq, timeouts = 0;
  buf_element_t *read_buffer = NULL;

  while(this->control_running && this->fd_data >= 0) {

    result = _x_io_select(this->stream, this->fd_data,
			  XIO_READ_READY, 20);

    if(result != XIO_READY) {
      if(result == XIO_TIMEOUT) {
	if(timeouts++ > 25)
	  return XIO_TIMEOUT;
	continue;
      }
      return result;
    }
    timeouts = 0;

    if(!this->control_running)
      break;

    /* 
     * allocate buffer and read incoming UDP packet from socket 
     */

    if(!read_buffer) {
      
      pthread_testcancel();

      read_buffer = get_buf_element(this, 0, 0);
      if(!read_buffer) {
	/* if queue is full, skip (video) frame. 
	   Waiting longer for free buffers just makes things worse ... */
	if(!this->is_paused) {
	  LOGDBG("UDP Fifo buffer full !");
	  if(this->scr && !udp->scr_jump_done) {
	    pvrscr_skip_frame (this->scr);
	    LOGMSG("SCR jump: +40 ms (live=%d, tunning=%d) time %ds", 
		   this->live_mode, this->scr_tunning, 
		   (int)(monotonic_time_ms()/1000));
	    udp->scr_jump_done = 50;
	    xine_usec_sleep(5*1000);
	  }
	}

	VDR_ENTRY_LOCK(XIO_ERROR);
	vdr_plugin_poll(this, 100);
	VDR_ENTRY_UNLOCK();

	if(!this->control_running)
	  break;

	read_buffer = get_buf_element(this, 0, 0);
	if(!read_buffer) {
	  if(!this->is_paused)
	    LOGMSG("Fifo buffer still full after poll !");
	  xine_usec_sleep(5*1000);
	  return result;
	}
      }

      if(udp->scr_jump_done)
	udp->scr_jump_done --;
    }

    /* Receive frame from socket and check for errors */
    n = recvfrom(this->fd_data, read_buffer->mem, 
		 read_buffer->max_size, MSG_TRUNC,
		 &server_address, &address_len);
    if(n <= 0) {
      if(n<0 && this->control_running && errno != EINTR)
	LOGERR("read_net_udp recv() error");
      if(!n || errno != EINTR)
	result = XIO_ERROR;
      break;
    }

    /* check source address */
    if((server_address.sin_addr.s_addr != 
	udp->server_address.sin_addr.s_addr) ||
       server_address.sin_port != udp->server_address.sin_port) {
#ifdef LOG_UDP
      uint32_t tmp_ip = ntohl(server_address.sin_addr.s_addr);
      LOGUDP("Received data from unknown sender: %d.%d.%d.%d:%d",
	     ((tmp_ip>>24)&0xff), ((tmp_ip>>16)&0xff), 
	     ((tmp_ip>>8)&0xff), ((tmp_ip)&0xff),
	     server_address.sin_port);
#endif
      continue;
    }

    /* Check if frame size is valid */
    if(n < sizeof(stream_udp_header_t)) {
      LOGMSG("received invalid UDP packet (too short)");
      continue;
    }
    if(n > read_buffer->max_size) {
      LOGMSG("received too large UDP packet ; part of data was discarded");
      n = read_buffer->max_size;
    }

    read_buffer->size = n;
    read_buffer->type = BUF_MAJOR_MASK;

    pkt = (stream_udp_header_t*)read_buffer->mem;
    pkt_data = read_buffer->mem + sizeof(stream_udp_header_t);
    
    if(this->rtp) {
      if(n < sizeof(stream_rtp_header_impl_t)) {
	LOGMSG("received invalid RTP packet (too short)");
	continue;
      }

      /* check if RTP header is valid. If not, assume UDP without RTP. */
      rtp_pkt = (stream_rtp_header_impl_t*)read_buffer->mem;
      if(rtp_pkt->rtp_hdr.raw[0] == (RTP_VERSION_BYTE | RTP_HDREXT_BIT) && 
	 rtp_pkt->rtp_hdr.raw[1] == RTP_PAYLOAD_TYPE &&
	 rtp_pkt->hdr_ext.hdr.size == htons(RTP_HEADER_EXT_X_SIZE) &&
	 rtp_pkt->hdr_ext.hdr.type == htons(RTP_HEADER_EXT_X_TYPE)) {

	/* strip RTP header but leave UDP header (carried inside RTP header extension) */
	pkt = (stream_udp_header_t*)(read_buffer->mem + 
				     sizeof(stream_rtp_header_impl_t) - 
				     sizeof(stream_udp_header_t));
	pkt_data = read_buffer->mem + sizeof(stream_rtp_header_impl_t);
	
	read_buffer->content += sizeof(stream_rtp_header_impl_t) - sizeof(stream_udp_header_t);
	read_buffer->size -= sizeof(stream_rtp_header_impl_t) - sizeof(stream_udp_header_t);
      }
    }

    pkt->seq = ntohs(pkt->seq);
    pkt->pos = ntohull(pkt->pos);

    /* Check for control messages */
    if(/*pkt->seq == (uint16_t)(-1) &&*/ /*0xffff*/
       pkt->pos == (uint64_t)(-1ULL) && /*0xffffffff ffffffff*/
       pkt_data[0]) { /* -> can't be PES frame */
      pkt_data[64] = 0;
      if(!strncmp((char*)pkt_data, "UDP MISSING", 11)) {
	/* Re-send failed */ 
	int seq1 = 0, seq2 = 0;
	uint64_t rpos = 0ULL;
	sscanf((char*)pkt_data, "UDP MISSING %d-%d %" PRIu64, 
	       &seq1, &seq2, &rpos);
	read_buffer->size = sizeof(stream_udp_header_t);
	read_buffer->type = BUF_MAJOR_MASK;
	pkt->pos = rpos;
	LOGUDP("Got UDP MISSING %d-%d (currseq=%d)", seq1, seq2, udp->next_seq);
	if(seq1 == udp->next_seq) {
	  /* this is the one we are expecting ... */
	  int n = ADDSEQ(seq2 + 1, -seq1);
	  udp->missed_frames += n;
	  seq2 &= UDP_SEQ_MASK;
	  pkt->seq = seq2;
	  udp->next_seq = seq2;
	  LOGUDP("  accepted: now currseq %d", udp->next_seq);
	  /* -> drop frame thru as empty ; it will trigger queue to continue */
	} else {
	  LOGUDP("  rejected: not expected seq ???");
	  continue;
	}
      } else {
	data_stream_parse_control(this, (char*)pkt_data);
	continue;
      }
    } else {
      /* Check for PES header */
      if(pkt_data[0] || pkt_data[1] || pkt_data[2] != 1) {
	LOGMSG("received invalid UDP packet (PES header 0x000001 missing)");
	continue;
      }
    }

    /* Check if header is valid */
    if(pkt->seq > UDP_SEQ_MASK) {
      LOGMSG("received invalid UDP packet (sequence number too big)");
      continue;
    }

    /*
     * handle re-ordering and retransmissios 
     */

    current_seq = pkt->seq & UDP_SEQ_MASK;
    /* first received frame initializes sequence counter */
    if(udp->received_frames == -1) { 
      udp->next_seq = current_seq;
      udp->received_frames = 0;
    }

    /* check if received sequence number is inside allowed window 
       (half of whole range) */
    if(ADDSEQ(current_seq, -udp->next_seq) > ((UDP_SEQ_MASK+1) >> 1)/*0x80*/) {
      struct sockaddr_in sin;
      LOGUDP("Received SeqNo out of window (%d ; [%d..%d])", 
	     current_seq, udp->next_seq, 
	     (udp->next_seq+((UDP_SEQ_MASK+1) >> 1)/*0x80*/) & UDP_SEQ_MASK);
      /* reset link */
      LOGDBG("UDP: resetting link");
      memcpy(&sin, &udp->server_address, sizeof(sin));
      free_udp_data(udp);
      udp = this->udp_data = init_udp_data();
      memcpy(&udp->server_address, &sin, sizeof(sin));
      continue;
    }

    /* Add received frame to incoming queue */
    if(udp->queue[current_seq]) {
      /* Duplicate packet or lot of dropped packets */
      LOGUDP("Got duplicate or window exceeded ? (queue slot %d in use) !",
	     current_seq);
      udp->queue[current_seq]->free_buffer(udp->queue[current_seq]);
      udp->queue[current_seq] = NULL;
      if(!udp->queued) 
	LOGERR("UDP queue corrupt !!!");
      else
	udp->queued--;
    }
    udp->queue[current_seq] = read_buffer;
    read_buffer = NULL;
    udp->queued ++;

    /* stay inside receiving window:
       If window exceeded, skip missing frames */
    if(udp->queued > ((UDP_SEQ_MASK+1)>>2)) {
#ifdef LOG_UDP
      int start = udp->next_seq;
#endif
      while(!udp->queue[udp->next_seq]) {
	INCSEQ(udp->next_seq);
	udp->missed_frames++;
      }
      udp->resend_requested = 0;
      LOGUDP("Re-ordering window exceeded, skipped missed frames %d-%d", 
	     start, udp->next_seq-1);
    }

    /* flush continous part of queue to demuxer queue */
    while(udp->queued > 0 && udp->queue[udp->next_seq]) {
      pkt = (stream_udp_header_t*)udp->queue[udp->next_seq]->content;
      udp->queue_input_pos = pkt->pos + udp->queue[udp->next_seq]->size - 
	                     sizeof(stream_udp_header_t);
      if(udp->queue[udp->next_seq]->size > sizeof(stream_udp_header_t))
	this->block_buffer->put(this->block_buffer, udp->queue[udp->next_seq]);
      else
	udp->queue[udp->next_seq]->free_buffer(udp->queue[udp->next_seq]);
      
      udp->queue[udp->next_seq] = NULL;
      udp->queued --;
      INCSEQ(udp->next_seq);
      if(udp->resend_requested)
	udp->resend_requested --;
    }

    /* no new resend requests until previous has been completed or failed */
    if(udp->resend_requested) 
      continue;

    /* If frames are missing, request re-send */
    if(NEXTSEQ(current_seq) != udp->next_seq  &&  udp->queued) {

      if(!udp->resend_requested) {
	int max_req = 20;

	while(!udp->queue[current_seq] && --max_req > 0) 
	  INCSEQ(current_seq);
	
	printf_control(this, "UDP RESEND %d-%d %" PRIu64 "\r\n", 
		       udp->next_seq, PREVSEQ(current_seq), 
		       udp->queue_input_pos);
	udp->resend_requested = 
	  (current_seq + (UDP_SEQ_MASK+1) - udp->next_seq) & UDP_SEQ_MASK;
	
	LOGUDP("%d-%d missing, requested re-send for %d frames",
	       udp->next_seq, PREVSEQ(current_seq), udp->resend_requested);
      }
    }

#ifdef LOG_UDP
    /* Link quality statistics */
    udp->received_frames++;
    if(udp->received_frames >= 1000) {
      if(udp->missed_frames)
	LOGUDP("packet loss %d.%d%% (%4d/%4d)",
	       udp->missed_frames*100/udp->received_frames, 
	       (udp->missed_frames*1000/udp->received_frames)%10, 
	       udp->missed_frames, udp->received_frames);
      udp->missed_frames = udp->received_frames = 0;
    }
#endif
  }
  
  if(read_buffer)
    read_buffer->free_buffer(read_buffer);

  return result;
}


static void *vdr_data_thread(void *this_gen)
{
  vdr_input_plugin_t *this = (vdr_input_plugin_t *) this_gen;

  LOGDBG("Data thread started");

  if (nice(-1) < 0) ;

  if(this->udp || this->rtp) {
    while(this->control_running) {
      if(vdr_plugin_read_net_udp(this) == XIO_ERROR)
        break;
      pthread_testcancel();
    }
  } else {
    while(this->control_running) {
      if(vdr_plugin_read_net_tcp(this) == XIO_ERROR)
        break;
      pthread_testcancel();
    }
  }

  this->control_running = 0;
  LOGDBG("Data thread terminated");
  pthread_exit(NULL);
}

#ifdef TEST_PIP
static int write_slave_stream(vdr_input_plugin_t *this, const char *data, int len)
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

static int vdr_plugin_write(input_plugin_t *this_gen, const char *data, int len)
{
  vdr_input_plugin_t *this = (vdr_input_plugin_t *) this_gen;
  buf_element_t      *buf = NULL;
  static int overflows = 0;

  if(this->slave_stream)
    return len;

#ifdef TEST_PIP
  /* some (older?) VDR recordings have video PID != 0xE0 ... */

  /* slave */
  if(((uint8_t*)data)[3] > 0xe0 && ((uint8_t*)data)[3] <= 0xef) 
    return write_slave_stream(this, data, len);
#endif

  TRACE("vdr_plugin_write (%d bytes)", len); 

  VDR_ENTRY_LOCK(0);

  buf = get_buf_element(this, len, 0);
  if(!buf) {
    /* need counter to filter non-fatal overflows
       (VDR is not polling for every PES packet) */
    if(overflows++ > 1)
      LOGMSG("vdr_plugin_write: buffer overflow ! (%d bytes)", len);
    VDR_ENTRY_UNLOCK();
    xine_usec_sleep(5*1000);
    return 0; /* EAGAIN */
  }
  overflows = 0;

  if(len > buf->max_size) {
    LOGMSG("vdr_plugin_write: PES too long (%d bytes, max size "
	   "%d bytes), data ignored !", len, buf->max_size);
    buf->free_buffer(buf);
/* curr_pos will be invalid when this point is reached ! */
    VDR_ENTRY_UNLOCK();
    return len;
  }

  buf->size = len;
  xine_fast_memcpy(buf->content, data, len);
  this->block_buffer->put(this->block_buffer, buf);

  VDR_ENTRY_UNLOCK();

  TRACE("vdr_plugin_write returns %d", len);

  return len;
}

static int vdr_plugin_keypress(input_plugin_t *this_gen, 
			       const char *map, const char *key,
			       int repeat, int release)
{
  vdr_input_plugin_t *this = (vdr_input_plugin_t *) this_gen;
  if(pthread_mutex_lock(&this->lock)) {
    LOGERR("vdr_plugin_keypress: pthread_mutex_lock failed");
    return -1;
  }

  if(key && this->fd_control >= 0) {
    if(map)
      printf_control(this, "KEY %s %s %s %s\r\n", map, key, 
		      repeat?"Repeat":"", release?"Release":"");
    else
      printf_control(this, "KEY %s\r\n", key);
  }

  pthread_mutex_unlock(&this->lock);
  return 0;
}


/******************************* Plugin **********************************/

static void track_audio_stream_change(vdr_input_plugin_t *this, buf_element_t *buf)
{
  /* track audio stream changes */
  int audio_changed = 0;
  if(buf->content[3] >= 0xc0 && buf->content[3] < 0xe0) {
    /* audio */
    if(this->prev_audio_stream_id != (buf->content[3] << 8)) {
      /*LOGDBG("Audio changed -> %d (%02X)", buf->content[3] - 0xc0, buf->content[3]);*/
      this->prev_audio_stream_id = buf->content[3] << 8;
      audio_changed = 1;
    }
  }
  else if(buf->content[3] == 0xbd) {
    /* PS1 */
    int PayloadOffset  = buf->content[8] + 9;
    int SubStreamId    = buf->content[PayloadOffset];
    int SubStreamType  = SubStreamId & 0xF0;
    int SubStreamIndex = SubStreamId & 0x1F;
    switch (SubStreamType) {
      case 0x20: /* SPU */
      case 0x30: /* SPU */
	/*LOGMSG("SPU %d", SubStreamId);*/
        break;
      case 0x80: /* AC3 & DTS */
	if(this->prev_audio_stream_id != ((0xbd<<8) | SubStreamId)) {
	  LOGDBG("Audio changed -> AC3 %d (BD:%02X)", SubStreamIndex, SubStreamId);
	  this->prev_audio_stream_id = ((0xbd<<8) | SubStreamId);
	  audio_changed = 1;
	}
	break;
      case 0xA0: /* LPCM */
	if(this->prev_audio_stream_id != ((0xbd<<8) | SubStreamId)) {
	  LOGDBG("Audio changed -> LPCM %d (BD:%02X)", SubStreamIndex, SubStreamId);
	  this->prev_audio_stream_id = ((0xbd<<8) | SubStreamId);
	  audio_changed = 1;
	}
	break;
      default:
	/*LOGMSG("Unknown PS1 substream %d", SubStreamId);*/
	break;
    }
  } else {
    /* no audio */
    return;
  }

  if(audio_changed) {
#if !defined(BUF_CONTROL_RESET_TRACK_MAP)
#  warning xine-lib is older than 1.1.2. Multiple audio streams are not supported.
#else
    put_control_buf(this->stream->audio_fifo,
		    this->stream->audio_fifo,
		    BUF_CONTROL_RESET_TRACK_MAP);
#endif
#if 0
    put_control_buf(this->stream->audio_fifo,
		    this->stream->audio_fifo,
		    BUF_CONTROL_RESET_DECODER);
    put_control_buf(this->stream->audio_fifo,
		    this->stream->audio_fifo,
		    BUF_CONTROL_START);
#endif
#if 0
    LOGMSG("VDR-Given stream: %04x", this->audio_stream_id);
#endif
  }
}

#if XINE_VERSION_CODE < 10190
static off_t vdr_plugin_read (input_plugin_t *this_gen, char *buf_gen, off_t len)
#else
static off_t vdr_plugin_read (input_plugin_t *this_gen, void *buf_gen, off_t len)
#endif
{
  /* from xine_input_dvd.c: */
  /* FIXME: Tricking the demux_mpeg_block plugin */
  if(len > 3) {
    uint8_t *buf = (uint8_t*)buf_gen;
    buf[0] = 0;
    buf[1] = 0;
    buf[2] = 0x01;
    buf[3] = 0xba;
    return 4;
  }
  return 0;
}

/*#define CACHE_FIRST_IFRAME*/
#ifdef CACHE_FIRST_IFRAME
#  include "cache_iframe.c"
#endif

static void pts_wrap_workaround(vdr_input_plugin_t *this, buf_element_t *buf)
{
#if 1
  /* PTS wrap workaround for mpeg_block demuxer */
  int64_t pts = pts_from_pes(buf->content, buf->size);
  if(pts >= 0) {
    int video = ((buf->content[3] & 0xf0) == 0xe0);
    if(video)
      this->last_delivered_vid_pts = pts;
    if(!video) {
      if(pts > 0x40400000 && 
	 this->last_delivered_vid_pts < 0x40000000 && 
	 this->last_delivered_vid_pts > 0) {
	LOGMSG("VIDEO pts wrap before AUDIO, ignoring audio pts %" PRId64, pts);
	pes_strip_pts(buf->content, buf->size);
      }
    }
  }
#endif
}

static void post_frame_end(vdr_input_plugin_t *this, int type)
{
  /* signal FRAME_END to video decoder */ 
  buf_element_t *cbuf = get_buf_element (this, 0, 1);
  if (!cbuf) {
    LOGMSG("get_buf_element() for BUF_FLAG_FRAME_END failed - retrying");
    xine_usec_sleep (10*1000);
    cbuf = get_buf_element (this, 0, 1);
  }
  if (cbuf) {
    cbuf->type = type;
    cbuf->decoder_flags = BUF_FLAG_FRAME_END;
    this->stream->video_fifo->put (this->stream->video_fifo, cbuf);
  } else if (type == BUF_VIDEO_H264) {
    /* Should not be here ... 
       Failing to send BUF_FLAG_FRAME_END 's freezes the decoder */
    LOGERR("get_buf_element() for H.264 BUF_FLAG_FRAME_END failed - aborting");
  }
}

static int update_frames(vdr_input_plugin_t *this, uint8_t *data, int len)
{
  int i = 8;

  if (!this->I_frames)
    this->P_frames = this->B_frames = 0;
  i += data[i] + 1;   /* possible additional header bytes */
  for (; i < len-5; i++) {
    if (data[i] == 0 && data[i + 1] == 0 && data[i + 2] == 1 && data[i + 3] == 0) {
      uint8_t type = ((data[i + 5] >> 3) & 0x07);
      switch (type) {
        case 1: this->I_frames++; LOGSCR("I"); break;
        case 2: this->P_frames++; LOGSCR("P"); break;
        case 3: this->B_frames++; LOGSCR("B"); break;
        default: return 0;
      }
      return type;
    }
  }
  return 0;
}

#ifdef TEST_H264
static int update_frames_h264(vdr_input_plugin_t *this, uint8_t *data, int len)
{
  int i = 8;
  if (!this->I_frames)
    this->P_frames = this->B_frames = 0;
  i += data[i] + 1;   /* possible additional header bytes */
  for (; i < len-5; i++) {
    if (data[i] == 0 && data[i + 1] == 0 && data[i + 2] == 1 && data[i + 3] == 9) {
      uint8_t type = (data[i + 4] >> 5);
      switch (type) {
        case 0: case 3: case 5: this->I_frames++; LOGSCR("I %d", type); break;
        case 1: case 4: case 6: this->P_frames++; LOGSCR("P %d", type); break;
        case 2: case 7:         this->B_frames++; LOGSCR("B %d", type); break;
        default: return 0;
      }
      return type;
    }
  }
  return 0;
}
#endif

#ifdef TEST_H264
static int detect_h264(vdr_input_plugin_t *this, uint8_t *data, int len)
{
  int i = 8;
  i += data[i] + 1;   /* possible additional header bytes */

  /* H.264 detection */
  if (data[i] == 0 && data[i + 1] == 0 && data[i + 2] == 1) {
    if (data[i + 3] == 0x09) {
      LOGMSG("H.264 scanner: Possible H.264 NAL AUD");
      return 1;
    }
    if (data[i + 3] == 0) {
      LOGDBG("H.264 scanner: Possible MPEG2 start code PICTURE (0x00)");
      return 0;
    }
    if (data[i + 3] >= 0x80) {
      LOGDBG("H.264 scanner: Possible MPEG2 start code (0x%02x)", data[i + 3]);
      return 0;
    }
    LOGMSG("H.264 scanner: Unregonized header 00 00 01 %02x", data[i + 3]);
  }

#if 0
  if (this->h264 < 0)
    LOGDBG("H.264 scanner: unregonized video packet");
#endif

  return this->h264;
}
#endif /* TEST_H264 */

#ifdef TEST_H264
buf_element_t *post_frame_h264(vdr_input_plugin_t *this, buf_element_t *buf)
{
  int64_t pts = pts_from_pes (buf->content, buf->size);
  uint8_t *data = buf->content;
  int i = 8;

  /* Detect video frame boundaries */

  i += data[i] + 1;   /* possible additional header bytes */

  if (data[i] == 0 && data[i + 1] == 0 && data[i + 2] == 1 ) {

    /* Access Unit Delimiter */
    if (data[i + 3] == 0x09)
      post_frame_end (this, BUF_VIDEO_H264);

    if (data[i + 3] >= 0x80) {
      LOGMSG("H.264: Possible MPEG2 start code (0x%02x)", data[i + 3]);
      /* Should do something ... ? */
    }

    if(this->live_mode && this->I_frames < 4)
      /*if((buf->content[3] & 0xf0) == 0xe0 && buf->size > 32)*/
        update_frames_h264(this, buf->content, buf->size);
  }

  /* Handle PTS and DTS */

  buf->decoder_info[0] = 0;
  if (pts >= INT64_C(0)) {
#if 0
    int64_t dts = dts_from_pes (buf->content, buf->size);
    if (dts < INT64_C(0)) {
      buf->decoder_info[0] = pts;
    } else {
      buf->decoder_info[0] = (pts - dts);
      buf->decoder_flags |= BUF_FLAG_FRAMERATE;
    }
    LOGMSG("H.264: dts %"PRId64"  DIFF %d [stream video step %d]", 
	   dts, (int)(pts-dts), 
	   _x_stream_info_get(this->stream, XINE_STREAM_INFO_FRAME_DURATION));
#endif
    if (this->send_pts) {
      LOGMSG("H.264: post pts %"PRId64, pts);
      vdr_x_demux_control_newpts (this->stream, pts, BUF_FLAG_SEEK);
      this->send_pts = 0;
    } else if (this->last_delivered_vid_pts > 0 && 
	       abs(pts - this->last_delivered_vid_pts) > 270000 /* 3 sec */) {
      LOGMSG("H.264: post pts %"PRId64" diff %d", pts, (int)(pts-this->last_delivered_vid_pts));
      vdr_x_demux_control_newpts (this->stream, pts, BUF_FLAG_SEEK);
    }
#if 0
    /* xine ffmpeg decoder does not handle pts <-> dts difference very well if P/B frames have pts */
    if (abs(pts - this->last_delivered_vid_pts) < 90000 && pts < this->last_delivered_vid_pts) {
      LOGDBG("H.264:    -> pts %"PRId64"  <- 0", pts);
      /*buf->pts = 0;*/
    } else if (dts>0) {
      LOGDBG("H.264:    -> pts %"PRId64"  <- 0 (DTS %"PRId64")", pts, dts);
      /*buf->pts = 0;*/
    } else {
      LOGDBG("H.264:    -> pts %"PRId64, pts);
      buf->pts = pts;   
    }
#else
    buf->pts = pts;   
#endif
    this->last_delivered_vid_pts = pts;
  }

  /* bypass demuxer ... */

  buf->type = BUF_VIDEO_H264;
  buf->content += i;
  buf->size -= i;

  this->stream->video_fifo->put (this->stream->video_fifo, buf);

  return NULL;
}
#endif /* TEST_H264 */

static buf_element_t *vdr_plugin_read_block (input_plugin_t *this_gen,
					     fifo_buffer_t *fifo, off_t todo) 
{
  vdr_input_plugin_t *this = (vdr_input_plugin_t *) this_gen;
  buf_element_t      *buf  = NULL;
#ifdef TEST_SCR_PAUSE
  int need_pause = 0;
#endif

  TRACE("vdr_plugin_read_block");

  /* check for disconnection/termination */
  if(!this->funcs.push_input_write /* reading from socket */ &&
     !this->control_running) {
    LOGMSG("read_block: no data source, returning NULL");
    if(this->block_buffer)
      this->block_buffer->clear(this->block_buffer);
    if(this->big_buffer)
      this->big_buffer->clear(this->big_buffer);
    if(this->hd_buffer)
      this->hd_buffer->clear(this->hd_buffer);
    set_playback_speed(this, 1);
    this->live_mode = 0;
    reset_scr_tunning(this, XINE_FINE_SPEED_NORMAL);
    this->stream->emergency_brake = 1;
    return NULL; /* disconnected ? */
  }

  /* Return immediately if demux_action_pending flag is set */
  if(this->stream->demux_action_pending) {
    if(NULL != (buf = make_padding_frame(this)))
      return buf;
    LOGMSG("vdr_plugin_read_block: demux_action_pending, make_padding_frame failed");
  }

#ifdef CACHE_FIRST_IFRAME
  if(NULL != (buf = get_cached_iframe(this)))
    return buf;
#endif

  /* adjust SCR speed */
#ifdef ADJUST_SCR_SPEED
  if(pthread_mutex_lock(&this->lock)) {
    LOGERR("read_block: pthread_mutex_lock failed");
    return NULL;
  }

  if( (!this->live_mode && (this->fd_control < 0 || 
			    this->fixed_scr)) ||
      this->slave_stream) {
    if(this->scr_tunning)
      reset_scr_tunning(this, this->speed_before_pause);
  } else {
# ifdef TEST_SCR_PAUSE
    if(this->stream_start || this->send_pts) {
      reset_scr_tunning(this, this->speed_before_pause);
      need_pause = 1;
    } else {
# endif
      vdr_adjust_realtime_speed(this);
# ifdef TEST_SCR_PAUSE
    }
# endif
  }
  pthread_mutex_unlock(&this->lock); 
#endif

  do {
    buf = fifo_buffer_try_get(this->block_buffer);
    if(!buf) {
      struct timespec abstime;
      create_timeout_time(&abstime, 500);
      pthread_mutex_lock(&this->block_buffer->mutex);
      if(this->block_buffer->fifo_size <= 0)
	pthread_cond_timedwait (&this->block_buffer->not_empty, 
				&this->block_buffer->mutex, &abstime);
      pthread_mutex_unlock(&this->block_buffer->mutex);
#if 1
      if(!this->is_paused && 
	 !this->still_mode &&
	 !this->is_trickspeed &&
	 !this->slave_stream && 
	 this->stream->video_fifo->fifo_size <= 0) {
	this->padding_cnt++;

	if(this->padding_cnt > 16) {
	  LOGMSG("No data in 8 seconds, queuing no signal image");
	  queue_nosignal(this);
	  this->padding_cnt = 0;
	}
      } else {
	this->padding_cnt = 0;
      }
#endif
      if(NULL != (buf = make_padding_frame(this)))
	return buf;
      LOGMSG("make_padding_frame FAILED");
      continue;
    }
    this->padding_cnt = 0;

    /* internal control bufs */
    if(buf->type == CONTROL_BUF_BLANK) {
      buf->free_buffer(buf);
      buf = NULL;

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

      continue;
    }

    /* control buffers go always to demuxer */
    if ((buf->type & BUF_MAJOR_MASK) ==  BUF_CONTROL_BASE)
      return buf;

    pthread_mutex_lock(&this->lock);

    /* Update stream position and remove network headers */
    strip_network_headers(this, buf);

    /* Update stream position */
    this->curpos += buf->size;
    this->curframe ++;

    /* Handle discard */
    if(this->discard_index > this->curpos && this->guard_index < this->curpos) {
      this->last_delivered_vid_pts = INT64_C(-1);
      pthread_mutex_unlock(&this->lock);
      buf->free_buffer(buf);
      buf = NULL;
      continue;
    }

    /* ignore UDP/RTP "idle" padding */
    if(buf->content[3] == 0xbe) {
      pthread_mutex_unlock(&this->lock);
      return buf;
    }

    /* Send current PTS ? */
    if(this->stream_start) {
      this->last_delivered_vid_pts = INT64_C(-1);
      this->send_pts = 1;
      this->stream_start = 0;
      this->h264 = -1;
      pthread_mutex_lock (&this->stream->first_frame_lock);
      this->stream->first_frame_flag = 2;
      pthread_mutex_unlock (&this->stream->first_frame_lock);
    }

    pthread_mutex_unlock(&this->lock);

#ifdef TEST_H264
    /* H.264 */
    if(this->h264 && (buf->content[3] & 0xf0) == 0xe0) {
      if(this->h264 < 0)
	this->h264 = detect_h264(this, buf->content, buf->size);
      
      if(this->h264 > 0) {
	buf = post_frame_h264(this, buf);
	if(!buf)
	  continue;
      }
    }
#endif

  } while(!buf);

#ifdef CACHE_FIRST_IFRAME
  cache_iframe(this, buf);
#endif

  track_audio_stream_change(this, buf);

  pts_wrap_workaround(this, buf);

  /* Send current PTS ? */
  if(this->send_pts) {
    int64_t pts = pts_from_pes(buf->content, buf->size);
    if(pts > 0) {
#ifdef TEST_SCR_PAUSE
      if(need_pause)
	scr_tunning_set_paused(this);
#endif
      vdr_x_demux_control_newpts(this->stream, pts, BUF_FLAG_SEEK);
      this->send_pts = 0;
    } else if(pts == 0) {
      /* Still image? do nothing, leave send_pts ON */
    }
  }

#ifdef LOG_FIRSTFRAME_FLAG
  {
    /* trace flag changes */
    static uint64_t timer = 0;
    static int tfd = 0;
    pthread_mutex_lock (&this->stream->first_frame_lock);
    if(tfd != this->stream->first_frame_flag) {
      uint64_t now = monotonic_time_ms();
      if(tfd)
	LOGMSG("FIRST FRAME FLAG %d -> %d (%d ms)", 
	       tfd, this->stream->first_frame_flag, (int)(now-timer));
      timer = now;
      tfd = this->stream->first_frame_flag;
    }
    pthread_mutex_unlock (&this->stream->first_frame_lock);
  }
#endif

  if(this->still_mode && buf->size == 14) {
    /* generated still images start with empty video PES, PTS = 0.
       Reset metronom pts so images will be displayed */
    int64_t pts = pts_from_pes(buf->content, buf->size);
    if(pts==0) {
      vdr_x_demux_control_newpts(this->stream, pts, BUF_FLAG_SEEK);
      /* delay frame 10ms (9000 ticks) */
      /*buf->content[12] = (uint8_t)((10*90) >> 7);*/
    }
  }

#ifndef FFMPEG_DEC
  if(this->live_mode && this->I_frames < 4)
    if((buf->content[3] & 0xf0) == 0xe0 && buf->size > 32)
      update_frames(this, buf->content, buf->size);
#else /* FFMPEG_DEC */
  if(this->ffmpeg_video_decoder || (this->live_mode && this->I_frames < 4))
    if((buf->content[3] & 0xf0) == 0xe0 && buf->size > 32) {
      int type = update_frames(this, buf->content, buf->size);
      if(type && this->ffmpeg_video_decoder) {
	/* signal FRAME_END to decoder */
	post_frame_end(this, BUF_VIDEO_MPEG);
	/* for some reason ffmpeg mpeg2 decoder does not understand pts'es in B frames ? 
	 * (B-frame pts's are smaller than in previous P-frame) 
	 * Anyway, without this block of code B frames with pts are dropped. */
	if(type == 3 && pts_from_pes(buf->content, buf->size) > INT64_C(0))
	  pes_strip_pts(buf->content, buf->size);
      }
    }
#endif

  TRACE("vdr_plugin_read_block: return data, pos end = %" PRIu64, this->curpos);
  return buf;
}

static off_t vdr_plugin_seek (input_plugin_t *this_gen, off_t offset,
			      int origin) 
{
  return -1;
}

static off_t vdr_plugin_get_length (input_plugin_t *this_gen) 
{
  return -1;
}

static uint32_t vdr_plugin_get_capabilities (input_plugin_t *this_gen) 
{
  vdr_input_plugin_t *this = (vdr_input_plugin_t *) this_gen;

  if(!this->stream->demux_plugin) {
    return
      INPUT_CAP_PREVIEW |
#ifdef INPUT_CAP_NOCACHE
      INPUT_CAP_NOCACHE |
#endif
      INPUT_CAP_SEEKABLE | /* help demux detection */
      INPUT_CAP_BLOCK;
  }

  return
    INPUT_CAP_PREVIEW |
#ifdef INPUT_CAP_NOCACHE
    INPUT_CAP_NOCACHE |
#endif
    INPUT_CAP_BLOCK;
}

static uint32_t vdr_plugin_get_blocksize (input_plugin_t *this_gen) 
{
  vdr_input_plugin_t *this = (vdr_input_plugin_t *) this_gen;
  int ret = 2048;

  if(this->block_buffer) {
    pthread_mutex_lock(&this->block_buffer->buffer_pool_mutex);
    if(this->block_buffer->first &&  this->block_buffer->first->size > 0)
      ret = this->block_buffer->first->size;
    pthread_mutex_unlock(&this->block_buffer->buffer_pool_mutex);
  }

  return ret;
}

static off_t vdr_plugin_get_current_pos (input_plugin_t *this_gen)
{
  vdr_input_plugin_t *this = (vdr_input_plugin_t *) this_gen;
  return this->discard_index > this->curpos ? this->discard_index : this->curpos;
}

static void vdr_plugin_dispose (input_plugin_t *this_gen) 
{
  vdr_input_plugin_t *this = (vdr_input_plugin_t *) this_gen;
  int i, local;
  int fd = -1, fc = -1;

  if(!this_gen)
    return;

  LOGDBG("vdr_plugin_dispose");

  /* stop slave stream */
  if (this->slave_stream) {
    LOGMSG("dispose: Closing slave stream");
    if (this->slave_event_queue) 
      xine_event_dispose_queue (this->slave_event_queue);
    this->slave_event_queue = NULL;
    if(this->funcs.fe_control) {
      this->funcs.fe_control(this->funcs.fe_handle, "POST 0 Off\r\n");
      this->funcs.fe_control(this->funcs.fe_handle, "SLAVE 0x0\r\n");
    }
    xine_stop(this->slave_stream);
    xine_close(this->slave_stream);
    xine_dispose(this->slave_stream);
    this->slave_stream = NULL;
  }

  if(this->fd_control>=0)
    write_control(this, "CLOSE\r\n");

  this->control_running = 0;

  local = this->funcs.push_input_write ? 1 : 0;
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
    LOGDBG("Cancel data thread ...");
    /*pthread_cancel(this->data_thread);*/
    pthread_join (this->data_thread, &p);   
    LOGDBG("Threads joined");
  }

  /* event queue(s) and listener threads */
  LOGDBG("Disposing event queues");
  if (this->event_queue) 
    xine_event_dispose_queue (this->event_queue);
  this->event_queue = NULL;

  pthread_cond_broadcast(&this->engine_flushed);
  while(pthread_cond_destroy(&this->engine_flushed) == EBUSY) {
    LOGMSG("discard_signal busy !");
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
  while(pthread_mutex_destroy(&this->osd_lock) == EBUSY) {
    LOGMSG("osd_lock busy ...");
    pthread_mutex_lock(&this->osd_lock);
    pthread_mutex_unlock(&this->osd_lock);
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
  for(i=0; i<MAX_OSD_OBJECT; i++) {
    if(this->osdhandle[i] != -1) {
      osd_command_t cmd;
      LOGDBG("Closing osd %d", i);
      memset(&cmd,0,sizeof(cmd));
      cmd.cmd = OSD_Close;
      cmd.wnd = i;
      exec_osd_command(this, &cmd);
    }
  }

  /* restore video properties */
  if(this->video_properties_saved)
    set_video_properties(this, -1,-1,-1,-1,-1); /* restore defaults */

  signal_buffer_pool_not_empty(this);
  signal_buffer_not_empty(this);

  /* SCR */
  if (this->scr) {
    this->class->xine->clock->unregister_scr(this->class->xine->clock, 
					     &this->scr->scr);
    this->scr->scr.exit(&this->scr->scr);
  }

  free (this->mrl);

  if(this->udp_data)
    free_udp_data(this->udp_data);

  /* fifos */

  /* need to get all buffer elements back before disposing own buffers ... */
  LOGDBG("Disposing fifos");
  if(this->stream && this->stream->audio_fifo)
    this->stream->audio_fifo->clear(this->stream->audio_fifo);
  if(this->stream && this->stream->video_fifo)
    this->stream->video_fifo->clear(this->stream->video_fifo);

  if(this->iframe_buffer)
    this->iframe_buffer->clear(this->iframe_buffer);
  if(this->block_buffer)
    this->block_buffer->clear(this->block_buffer);
  if(this->big_buffer) 
    this->big_buffer->clear(this->big_buffer);
  if(this->hd_buffer)
    this->hd_buffer->clear(this->hd_buffer);

  if(this->iframe_buffer)
    this->iframe_buffer->dispose(this->iframe_buffer);
  if(this->big_buffer) 
    this->big_buffer->dispose(this->big_buffer);
  if(this->block_buffer)
    this->block_buffer->dispose(this->block_buffer);
  if(this->hd_buffer)
    this->hd_buffer->dispose(this->hd_buffer);

  memset(this, 0, sizeof(this));

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

  if(!this->stream->demux_plugin) {
    /* help in demuxer selection */
    static char fake[128] = {0};
    snprintf(fake, sizeof(fake)-1, "%s/.vob", this->mrl);
    fake[sizeof(fake)-1] = 0;
    return fake;
  }

  return this->mrl;
}

static int vdr_plugin_get_optional_data (input_plugin_t *this_gen,
					 void *data, int data_type) 
{
  if(data_type == INPUT_OPTIONAL_DATA_PREVIEW) {

    static const uint8_t preview_data[] = {0x00,0x00,0x01,0xBA, /* sequence start */
					   0x00,0x00,0x01,0xBE, /* padding */
					   0x00,0x02,0xff,0xff};
#if MAX_PREVIEW_SIZE < 12
#  warning MAX_PREVIEW_SIZE < 12 !
    memcpy(data, preview_data, MAX_PREVIEW_SIZE);
    return MAX_PREVIEW_SIZE;
#else
    memcpy(data, preview_data, sizeof(preview_data));
    return sizeof(preview_data);
#endif
  }

  return INPUT_OPTIONAL_UNSUPPORTED;
}

static int vdr_plugin_open(input_plugin_t *this_gen)
{
  vdr_input_plugin_t *this = (vdr_input_plugin_t *) this_gen;
  xine_t *xine = this->class->xine;

  this->event_queue = xine_event_new_queue (this->stream);
  xine_event_create_listener_thread (this->event_queue, vdr_event_cb, this);

  this->buffer_pool = this->stream->video_fifo;

#ifdef ADJUST_SCR_SPEED
  /* enable resample method */
  xine->config->update_num(xine->config,
			   "audio.synchronization.av_sync_method",1);

  /* register our own scr provider */
 {
    uint64_t time;
    time = xine->clock->get_current_time(xine->clock);
    this->scr = pvrscr_init();
    this->scr->scr.start(&this->scr->scr, time);
    if(xine->clock->register_scr(this->class->xine->clock, &this->scr->scr))
      LOGMSG("xine->clock->register_scr FAILED !");
    this->scr_live_sync = 1;
 }
#endif
  this->scr_tunning = SCR_TUNNING_OFF;
  this->curpos = 0;
  return 1;
}

static int vdr_plugin_open_local (input_plugin_t *this_gen) 
{
  LOGDBG("vdr_plugin_open_local");
  return vdr_plugin_open(this_gen);
}

static void set_recv_buffer_size(int fd, int max_buf)
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
    LOGMSG("Maybe host address is missing from server-side svdrphosts.conf ?");
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
  int fd=-1, one = 1, retries = 0, n;
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
#if 0
  LOGDBG("got address: %s int=0x%x net=0x%x translated=0x%x port=%d",
	 cmd+4, (ip0<<24)|(ip1<<16)|(ip2<<8)|ip3, 
	 htonl((ip0<<24)|(ip1<<16)|(ip2<<8)|ip3),
	 inet_addr("224.0.1.9"), port);
#endif

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

  n = recvfrom(fd, &tmp_rtp, sizeof(tmp_rtp), 0, &sin, &len);
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
  int n, retries = 0, port = -1, fd = -1;

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

  n = recvfrom(fd, &tmp_udp, sizeof(tmp_udp), 0, &sin, &len);
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
  } else if((n=read(fd_data, tmpbuf, sizeof(tmpbuf))) <= 0) {
    LOGERR("Data stream read failed (TCP)");
  } else if(n<6 || strncmp(tmpbuf, "DATA\r\n", 6)) {
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
      LOGMSG(" different host, pipe won't work");      
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
  if((!strncasecmp(this->mrl, "xvdr+tcp://", 11) && (this->tcp=1)) ||
     (!strncasecmp(this->mrl, "xvdr+udp://", 11) && (this->udp=1)) ||
     (!strncasecmp(this->mrl, "xvdr+rtp://", 11) && (this->rtp=1)) ||
     (!strncasecmp(this->mrl, "xvdr+pipe://", 12)) ||
     (!strncasecmp(this->mrl, "xvdr:tcp://", 11) && (this->tcp=1)) ||
     (!strncasecmp(this->mrl, "xvdr:udp://", 11) && (this->udp=1)) ||
     (!strncasecmp(this->mrl, "xvdr:rtp://", 11) && (this->rtp=1)) ||
     (!strncasecmp(this->mrl, "xvdr:pipe://", 12)) ||
     (!strncasecmp(this->mrl, "xvdr://", 7))) {

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
	this->tcp = this->udp = this->tcp = 0;
	LOGMSG("Data stream connected (PIPE)");
      }
    }

    /* try RTP ? */

    if(this->fd_data < 0 && !this->udp && !this->tcp) {
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
  if ((err = pthread_create (&this->data_thread,
			     NULL, vdr_data_thread, (void*)this)) != 0) {
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
  int                local_mode, i;

  LOGDBG("vdr_class_get_instance");

  if (strncasecmp (mrl, "xvdr:",5) && strncasecmp (mrl, "xvdr+",5))
    return NULL;

  if(!strncasecmp(mrl, "xvdr+slave://0x", 15)) {
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
  this->max_buffers  = 10;
  this->ffmpeg_video_decoder = -1;
  this->last_delivered_vid_pts = INT64_C(-1);
  this->autoplay_size = -1;

  for(i=0; i<MAX_OSD_OBJECT; i++)
    this->osdhandle[i] = -1;
  this->video_width  = this->vdr_osd_width  = 720;
  this->video_height = this->vdr_osd_height = 576;

  local_mode         = ( (!strncasecmp(mrl, "xvdr://", 7)) && 
			 (strlen(mrl)==7))
                       || (!strncasecmp(mrl, "xvdr:///", 8));

  if(!bSymbolsFound) {
    /* not running under VDR or vdr-sxfe/vdr-fbfe */
    if(local_mode) {
      LOGDBG("vdr or vdr-??fe not detected, forcing remote mode");
      local_mode = 0;
    }
    if(!strcasecmp(mrl, "xvdr:") ||
       !strcasecmp(mrl, "xvdr:/") ||
       !strcasecmp(mrl, "xvdr://") ||
       !strcasecmp(mrl, "xvdr:///")) {
      /* default to local host */
      free(this->mrl);
      this->mrl = strdup("xvdr://127.0.0.1");
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
    this->funcs.input_control     = vdr_plugin_keypress;
  }
  
  /* buffer */
  this->block_buffer = fifo_buffer_new(this->stream, 4, 0x10000+64); /* dummy buf to be used before first read and for big PES frames */
  
  /* sync */
  pthread_mutex_init (&this->lock, NULL);
  pthread_mutex_init (&this->osd_lock, NULL);
  pthread_mutex_init (&this->vdr_entry_lock, NULL);
  pthread_mutex_init (&this->fd_control_lock, NULL);
  pthread_cond_init  (&this->engine_flushed, NULL);

#ifdef FFMPEG_DEC
  if(this->ffmpeg_video_decoder < 0) {
    xine_cfg_entry_t ffmpegprio, mpeg2prio;
    this->ffmpeg_video_decoder = 0;
    if (xine_config_lookup_entry(this->class->xine, "engine.decoder_priorities.ffmpegvideo", &ffmpegprio) &&
	ffmpegprio.num_value > 0) {
      LOGMSG("ffmpeg video decoder priority: %d", ffmpegprio.num_value);
      this->ffmpeg_video_decoder = 1;
      if (xine_config_lookup_entry(this->class->xine, "engine.decoder_priorities.mpeg2", &mpeg2prio)) {
	LOGMSG("libmpeg2 video decoder priority: %d", mpeg2prio.num_value);
	if (mpeg2prio.num_value >= ffmpegprio.num_value)
	  this->ffmpeg_video_decoder = 0;
      }
      LOGMSG(" --> using %s mpeg2 video decoder", this->ffmpeg_video_decoder?"ffmpeg":"libmpeg2");
    }
  }
#endif

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
  return "xvdr";
}
#endif

static char **vdr_plugin_get_autoplay_list(input_class_t *this_gen, int *num_files) 
{
  vdr_input_class_t *this = (vdr_input_class_t *)this_gen;
  *num_files = 1;

  return this->mrls;
}

static void vdr_class_dispose (input_class_t *this_gen) 
{
  vdr_input_class_t *this = (vdr_input_class_t *) this_gen;

  this->xine->config->unregister_callback(this->xine->config,
					  "media.xvdr.default_mrl");
  this->xine->config->unregister_callback(this->xine->config,
					  "xvdr.osd.fast_scaling");

  free (this);
}

static void *init_class (xine_t *xine, void *data) 
{
  vdr_input_class_t  *this;
  config_values_t     *config = xine->config;

  SetupLogLevel();

  if(!bSymbolsFound) {
    if(xine->verbosity > 0) {
      iSysLogLevel = xine->verbosity + 1;
      LOGMSG("detected verbose logging xine->verbosity=%d, setting log level to %d:%s",
	     xine->verbosity, iSysLogLevel, 
	     iSysLogLevel==2?"INFO":"DEBUG");
    }
  }

  this = calloc(1, sizeof (vdr_input_class_t));

  this->xine   = xine;
  
  this->mrls[ 0 ] = config->register_string(config,                 
					    "media.xvdr.default_mrl",
                                            "xvdr://127.0.0.1#nocache;demux:mpeg_block",
                                            _("default VDR host"),
                                            _("The default VDR host"),
                                            10, vdr_class_default_mrl_change_cb, (void *)this);
  this->mrls[ 1 ] = 0;

  this->fast_osd_scaling = config->register_bool(config,
						 "input.xvdr.fast_osd_scaling", 0,
						 _("Fast (low-quality) OSD scaling"),
						 _("Enable fast (lower quality) OSD scaling.\n"
						   "Default is to use (slow) linear interpolation "
						   "to calculate pixels and full palette re-allocation "
						   "to optimize color palette.\n"
						   "Fast method only duplicates/removes rows and columns "
						   "and does not modify palette."),
						 10, vdr_class_fast_osd_scaling_cb, 
						 (void *)this);

  this->input_class.get_instance       = vdr_class_get_instance;
#if INPUT_PLUGIN_IFACE_VERSION < 18
  this->input_class.get_identifier     = vdr_class_get_identifier;
  this->input_class.get_description    = vdr_class_get_description;
#else
  this->input_class.identifier         = "xvdr";
  this->input_class.description        = N_("VDR (Video Disk Recorder) input plugin");
#endif
  this->input_class.get_autoplay_list  = vdr_plugin_get_autoplay_list;
  this->input_class.dispose            = vdr_class_dispose;

  LOGDBG("init class succeeded");

  return this;
}


/*
 * exported plugin catalog entry
 */

const plugin_info_t xine_plugin_info[] __attribute__((visibility("default"))) = {
  /* type, API, "name", version, special_info, init_function */
  { PLUGIN_INPUT, INPUT_PLUGIN_IFACE_VERSION, "XVDR", XINE_VERSION_CODE, NULL, init_class },
  { PLUGIN_NONE, 0, "", 0, NULL, NULL }
};

const plugin_info_t *xine_plugin_info_xvdr = xine_plugin_info;


