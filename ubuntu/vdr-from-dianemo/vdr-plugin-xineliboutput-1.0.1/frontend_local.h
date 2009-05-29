/*
 * frontend_local.h:
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: frontend_local.h,v 1.5 2007/04/12 22:53:36 phintuka Exp $
 *
 */

#ifndef __XINELIB_FRONTEND_LOCAL_H
#define __XINELIB_FRONTEND_LOCAL_H

#include "frontend.h"

//----------------------------- cXinelibLocal --------------------------------

extern "C" {
  typedef struct frontend_s frontend_t;
}

class cXinelibLocal : public cXinelibThread 
{

  public:
    cXinelibLocal(const char *frontend_name);
    virtual ~cXinelibLocal();

    // Thread control
    virtual void Stop(void);

  protected:
    virtual void Action(void);


  public:

    // Data transfer
    virtual int  Play_PES(const uchar *buf, int len);
    virtual void OsdCmd(void *cmd);
    virtual int64_t GetSTC();

    // Playback files
    virtual bool EndOfStreamReached(void);

    // Image grabbing
    virtual uchar *GrabImage(int &Size, bool Jpeg, int Quality, 
			     int SizeX, int SizeY);

    // Configuration
    virtual void ConfigureWindow(int fullscreen, int width, int height, 
				 int modeswitch, const char *modeline, 
				 int aspect, int scale_video, int field_order);
    virtual void ConfigureDecoder(int pes_buffers);

  protected:

    // Playback control
    virtual int  Xine_Control(const char *cmd);

  protected:

    // Frontend access    
    frontend_t *load_frontend(const char *fe_name);

    // Data
    void       *h_fe_lib;  
    frontend_t *fe;
    cRwLock     m_feLock;
    bool        m_bReconfigRequest;
};


#endif // __XINELIB_FRONTEND_LOCAL_H
