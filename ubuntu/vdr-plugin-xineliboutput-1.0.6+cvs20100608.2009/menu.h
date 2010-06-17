/*
 * menu.h: Main Menu
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: menu.h,v 1.8 2010/01/19 22:45:45 phintuka Exp $
 *
 */

#ifndef __XINELIB_MENU_H
#define __XINELIB_MENU_H

#include "features.h"

#include <vdr/menuitems.h>

class cMenuXinelib : public cMenuSetupPage
{
  private:
    int compression;
    int autocrop;
    int overscan;
    int novideo;

    // Hotkeys
    enum { hkInit, hkSeen, hkNone } hotkey_state;
    static time_t g_LastHotkeyTime;
    static eKeys  g_LastHotkey;
    virtual eOSState ProcessHotkey(eKeys Key);

#ifdef HAVE_XV_FIELD_ORDER
    cOsdItem *video_ctrl_interlace_order;
#endif
    cOsdItem *audio_ctrl_compress;

    cOsdItem *ctrl_autocrop;
    cOsdItem *ctrl_overscan;
    cOsdItem *ctrl_novideo;

  protected:
    virtual void Store(void);

  public:
    cMenuXinelib(void);
    virtual ~cMenuXinelib();
    virtual eOSState ProcessKey(eKeys Key);

    static cOsdMenu *CreateMenuBrowseFiles(eMainMenuMode mode, bool Queue=true);
};

#endif //__XINELIB_SETUP_MENU_H
