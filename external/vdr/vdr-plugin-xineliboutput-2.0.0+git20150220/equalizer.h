/*
 * equalizer.h: audio equalizer OSD control
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: equalizer.h,v 1.2 2012-03-18 19:17:07 phintuka Exp $
 *
 */

#ifndef __EQUALIZER_H
#define __EQUALIZER_H

#include <vdr/config.h>
#include <vdr/osdbase.h>

class cXinelibDevice;

class cEqualizer : public cOsdObject 
{
  private:
    cXinelibDevice *m_Dev;

    int *m_Values;
    int m_Current;

    cOsd *m_Osd;

  public:
    cEqualizer(cXinelibDevice *Dev);
    virtual ~cEqualizer();

    virtual void Show();
    virtual eOSState ProcessKey(eKeys Key);

    void DrawBackground(void);
    void DrawBar(int Index, bool Selected = false);
};

#endif // __EQUALIZER_H_
