/*
 * frontend_local.c:
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: frontend_local.c,v 1.61 2013-08-18 07:58:29 phintuka Exp $
 *
 */

#define __STDC_CONSTANT_MACROS

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <dlfcn.h>

#include <vdr/config.h>
#include <vdr/tools.h>
#include <vdr/shutdown.h>
#include <vdr/plugin.h>
#include <vdr/device.h>

#include "logdefs.h"
#include "config.h"

#include "xine_frontend.h"

#include "frontend_local.h"

//------------------------------ cRwLockBlock ---------------------------------

class cRwLockBlock 
{
  private:
    cRwLock& m_Lock;

  public:
    cRwLockBlock(cRwLock& lock, bool write) : m_Lock(lock)  
      { m_Lock.Lock(write);}

    ~cRwLockBlock()  
      { m_Lock.Unlock(); }
};

#define LOCK_FE     cRwLockBlock(m_feLock, false)
#define LOCK_FE_WR  cRwLockBlock(m_feLock, true)

//----------------- keyboard control handler (C callback) --------------------

extern "C" {
  static void keypress_handler(void *handle, const char *keymap, const char *key)
  {
    if (!handle) {
      LOGMSG("keypress_handler(): missing handle");
      return;
    }

    cXinelibThread *t = (cXinelibThread *)handle;

    if (!strncmp("INFO ", keymap, 5)) {
      t->InfoHandler(keymap+5);

    } else if (!xc.use_x_keyboard || !key) {

      /* Only X11 key events came this way in local mode.
	 Keyboard is handled by vdr. */
      LOGMSG("keypress_handler(%s): X11 Keyboard disabled in config", key);

    } else {
      t->KeypressHandler(keymap, key, false, false);

    }
  }
};

//----------------------------- cXinelibLocal --------------------------------

cXinelibLocal::cXinelibLocal(cXinelibDevice *Dev, const char *frontend_name) :
  cXinelibThread(Dev, "Local decoder/display (cXinelibThread)"), m_feLock(true)
{
  fe = NULL;
  h_fe_lib = NULL;
  m_bReconfigRequest = true;

  if (!xc.config_file &&
      0 < asprintf(&xc.config_file,
                   "%s/xineliboutput/config",
                   cPlugin::ConfigDirectory()))
    LOGMSG("cXinelibLocal: Using xine-lib configuration file %s", xc.config_file);
}

cXinelibLocal::~cXinelibLocal()
{
  TRACEF("cXinelibLocal::~cXinelibLocal");

  m_bReady = false;

  Cancel(-1);

  {
    LOCK_FE;
    m_bReady = false;
    if(fe)
      fe->fe_interrupt(fe);
  }

  Cancel(3);

  if (fe) {
    fe->fe_free(fe);
    fe = NULL;
  }
  if (h_fe_lib) {
    dlclose(h_fe_lib);
  }
}

//
// Data transfer
//

int cXinelibLocal::Play(const uchar *data, int len, eStreamId StreamId)
{
  TRACEF("cXinelibLocal::Play");

  {
    LOCK_FE;
    if (fe && Running()) {
      int done = fe->xine_queue_pes_packet(fe, StreamId, m_StreamPos, (const char *)data, len);
      if (done >= 0)
        return cXinelibThread::Play(data, done, StreamId);
    }
  }

  //cCondWait::SleepMs(5);
  return len;
}

void cXinelibLocal::OsdCmd(void *cmd)
{
  TRACEF("cXinelibLocal::OsdCmd");
  LOCK_FE;
  if(cmd && fe && m_bReady)
    fe->xine_osd_command(fe, (struct osd_command_s*)cmd);
}

uchar *cXinelibLocal::GrabImage(int &Size, bool Jpeg, 
				int Quality, int SizeX, 
				int SizeY)
{
  uchar *data;
  LOCK_FE;
  if(fe && fe->grab && m_bReady)
    if((data = (uchar*)fe->grab(fe, &Size, Jpeg, Quality, SizeX, SizeY)))
      return data;
  return NULL;
}

int64_t cXinelibLocal::GetSTC()
{
  TRACEF("cXinelibLocal::GetSTC");

  union {
    char    buf[32];
    int64_t pts;
  } u = {"GETSTC\r\n"};

  LOCK_FE;

  if (fe && m_bReady)
    if (0 == fe->xine_control(fe, u.buf))
      return u.pts;

  return INT64_C(-1);
}

//
// Playback files
//

bool cXinelibLocal::EndOfStreamReached(void) 
{
  LOCK_THREAD;
  if(fe && fe->xine_is_finished(fe, 1))
    return true;
  return cXinelibThread::EndOfStreamReached();
}

//
// Configuration
//

void cXinelibLocal::ConfigureWindow(int fullscreen, int width, int height, 
				    int modeswitch, const char *modeline, 
				    int aspect, int scale_video)
{
  LOCK_FE;
  if(fe)
    fe->fe_display_config(fe, -1, -1, width, height,
                          fullscreen, modeswitch, modeline,
                          aspect, scale_video);
}

void cXinelibLocal::ConfigureDecoder(int pes_buffers)
{
  // needs xine restart
  {
    LOCK_FE;
    xc.pes_buffers = pes_buffers;
    if(!fe)
      return;    
    m_bReady = false;
    m_bReconfigRequest = true;
    fe->fe_interrupt(fe);    
  }

  while (!m_bReady && Running())
    cCondWait::SleepMs(100);

  cCondWait::SleepMs(100);
}

int cXinelibLocal::SupportsTrueColorOSD(void)
{
  return !!xc.hud_osd;
}

//
// Xine control
//

int cXinelibLocal::Xine_Control(const char *cmd)
{
  TRACEF("cXinelibLocal::Xine_Control");
  if (cmd && *cmd && Running()) {
    char buf[4096];
    if(snprintf(buf, sizeof(buf), "%s\r\n", cmd) >= (int)sizeof(buf)) {
      buf[sizeof(buf)-1] = 0;
      LOGMSG("Xine_Control: message too long ! (%s)", buf);
      return 0;
    }
    LOCK_FE;
    if(fe)
      return fe->xine_control(fe, (char*)buf);
  }
  return 0;
}

//
// Frontend loader
//

frontend_t *cXinelibLocal::load_frontend(const char *fe_name)
{
  Dl_info info;
  struct stat statbuffer;
  char libname[4096]="";
  void *lib = NULL;
  fe_creator_f *fe_creator = NULL;
  static int my_marker = 0;

  if (!dladdr((void *)&my_marker, &info)) {
    LOGERR("Error searching plugin: dladdr() returned false (%s)", dlerror());
    return NULL;
  }
  LOGDBG("xineliboutput: plugin file is %s", info.dli_fname);

  int  fe_ind = strstra(fe_name, xc.s_frontends, FRONTEND_NONE);
  bool fe_try = false;
  if (fe_ind == FRONTEND_NONE) {
    LOGMSG("Front-end %s unknown!", fe_name);
    fe_ind = 0;
    fe_try = true;
  }

  strn0cpy(libname, info.dli_fname, sizeof(libname) - 128);
  if (strrchr(libname, '/'))
    *(strrchr(libname, '/')+1) = 0;

  LOGDBG("Searching frontend %s from %s", xc.s_frontends[fe_ind], libname);

  do {
    strncat(libname, xc.s_frontend_files[fe_ind], 64);
    LOGDBG("Probing %s", libname);

    if (stat(libname, &statbuffer)) {
      LOGERR("load_frontend: can't stat %s", libname);
    } else if((statbuffer.st_mode & S_IFMT) != S_IFREG) {
      LOGMSG("load_frontend: %s not regular file ! trying to load anyway ...",
	     libname);
    }

    if ( !(lib = dlopen (libname, RTLD_LAZY | RTLD_GLOBAL))) {
      LOGERR("load_frontend: cannot dlopen file %s: %s", libname, dlerror());

    } else if ( (fe_creator = (fe_creator_f*)dlsym(lib, "fe_creator"))) {
      LOGDBG("load_frontend: entry at %p", fe_creator);
      frontend_t *fe = (**fe_creator)();

      if (fe) {
	if (h_fe_lib)
	  dlclose(h_fe_lib);
	h_fe_lib = lib;

	LOGDBG("Using frontend %s (%s) from %s",
	       xc.s_frontends[fe_ind], xc.s_frontendNames[fe_ind],
	       xc.s_frontend_files[fe_ind]);

	return fe;
      } else {
	LOGMSG("Frontend %s (%s) creation failed",
	       xc.s_frontends[fe_ind], xc.s_frontendNames[fe_ind]);
      }
    } else {
      LOGERR("Frontend entry point not found");
      dlclose(lib);
    }

    fe_ind++;  // try next frontend ...

  } while (fe_try && fe_ind < FRONTEND_count);

  LOGMSG("No usable frontends found, giving up !");
  return NULL;
}

//
// Thread main loop
//

void cXinelibLocal::Action(void) 
{
  frontend_t *curr_fe = NULL;

  TRACEF("cXinelibLocal::Action");

  SetPriority(2); /* lower priority */

  // init frontend
  if(!curr_fe) {
    curr_fe = load_frontend(xc.local_frontend);
    if(!curr_fe) {
      LOGMSG("cXinelibLocal: Error initializing frontend");
      Cancel(-1);
    } else {
      LOGDBG("cXinelibLocal::Action - fe created");
      curr_fe->fe_message_cb = keypress_handler;
      curr_fe->fe_message_h  = this;
      if(!curr_fe->fe_display_open(curr_fe,
                                   xc.xpos, xc.ypos, xc.width, xc.height, xc.fullscreen,
                                   xc.hud_osd,
                                   xc.opengl,
                                   xc.modeswitch, xc.modeline, xc.display_aspect,
                                   0/*no_x_kbd*/, 0/*gui_hotkeys*/, 0/*touchscreen*/,
                                   xc.video_port,
                                   xc.scale_video,
                                   NULL,
                                   xc.window_id)) {
	LOGMSG("cXinelibLocal: Error initializing display");
        Cancel(-1);
      } else {
	LOGDBG("cXinelibLocal::Action - fe->fe_display_open ok");
      }
    }
  }

  // main loop
  while (Running()) {

    {
      // init and start xine engine
      LOCK_FE_WR;
      LOGDBG("cXinelibLocal::Action - xine_init");

      fe = curr_fe;
      if(m_bReconfigRequest) {
	if(!fe->xine_init(fe, xc.audio_driver, xc.audio_port,
			  xc.video_driver,
			  xc.pes_buffers,
			  xc.post_plugins, xc.config_file)) {
	  LOGMSG("cXinelibLocal: Error initializing frontend");
	  break;
	}
        LOGDBG("cXinelibLocal::Action - fe->xine_init ok");
        m_bReconfigRequest = false;
      }

      // open (xine) stream
      LOGDBG("cXinelibLocal::Action - xine_open");
      if(!fe->xine_open(fe, NULL)) {
        LOGMSG("cXinelibLocal: Error opening xvdr://");
        break;
      }
      LOGDBG("cXinelibLocal::Action - fe->xine_open ok");

      // start playing (xine) stream
      if(!fe->xine_play(fe)) {
        LOGMSG("cXinelibLocal: Error playing xvdr://");
        break;
      }
      LOGDBG("cXinelibLocal::Action - fe->xine_play ok");

      m_StreamPos = 0;
      Xine_Control("STREAMPOS 0"); 
      Xine_Control("VERSION " XINELIBOUTPUT_VERSION " " "\r\n");
    }

    // configure frontend and xine
    m_bNoVideo  = false;
    Configure();
    LOGDBG("cXinelibLocal::Action - fe config OK");

    LogoDisplay();
    LOGDBG("cXinelibLocal::Action - logo sent");

    {
      LOCK_THREAD;
      Xine_Control("NOVIDEO 0");
      Xine_Control("LIVE 1");
      Xine_Control("CLEAR");
      m_bNoVideo  = false;
      m_bLiveMode = true;
      m_bReady    = true;
    }

    // main event loop
    LOGDBG("cXinelibLocal:Action - Starting event loop");   
    {
      LOCK_FE;
      while (Running() && m_bReady &&
             (/*m_bLoopPlay ||*/ !fe->xine_is_finished(fe, 0)) &&
             fe->fe_run(fe))
	/*cCondWait::SleepMs(50)*/ ;
    }

    LOGDBG("cXinelibLocal::Action - event loop terminated, "
	   "xine_is_finished=%d", fe->xine_is_finished(fe, 0));

    {
      LOCK_THREAD;
      m_bReady = false;
      m_bEndOfStreamReached = true;
    }

    {
      LOCK_FE_WR;
      if(fe)
        fe->xine_close(fe);
      fe = NULL;
    }

    LOGMSG("cXinelibLocal::Action: Xine closed");

    if(!m_bReconfigRequest && xc.exit_on_close) {
      LOGMSG("Shutting down VDR");
      ShutdownHandler.RequestEmergencyExit();
      break;
    }
  }

  if(curr_fe) {
    LOCK_FE_WR;
    fe = NULL;
    curr_fe->xine_exit(curr_fe);
    curr_fe->fe_display_close(curr_fe);
    curr_fe->fe_free(curr_fe);
  }

  LOGMSG("cXinelibLocal::Action: thread finished");
}

