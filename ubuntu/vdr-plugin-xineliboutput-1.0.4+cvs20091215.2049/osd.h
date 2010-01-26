/*
 * osd.h: Xinelib On Screen Display control
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: osd.h,v 1.5 2007/10/15 00:31:39 phintuka Exp $
 *
 */

#ifndef __XINELIB_OSD_H
#define __XINELIB_OSD_H

#include <vdr/osd.h>

class cXinelibDevice;

class cXinelibOsdProvider : public cOsdProvider 
{
  protected:
    cXinelibDevice *m_Device;

  public:
    cXinelibOsdProvider(cXinelibDevice *Device);
    virtual ~cXinelibOsdProvider();

    virtual cOsd *CreateOsd(int Left, int Top, uint Level);

    static void RefreshOsd(void);

    // VDR < 1.5.9 compability
    virtual cOsd *CreateOsd(int Left, int Top) { return CreateOsd(Left, Top, 0); }
};

#endif //__XINELIB_OSD_H

