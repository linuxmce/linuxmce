/*
 * setup_menu.h: Setup Menu
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: setup_menu.h,v 1.2 2012-03-19 09:53:14 phintuka Exp $
 *
 */

#ifndef __XINELIB_SETUP_MENU_H
#define __XINELIB_SETUP_MENU_H

#include <vdr/menuitems.h>

class cXinelibDevice;

class cMenuSetupXinelib : public cMenuSetupPage {

  protected:
    cXinelibDevice *m_Dev;

    void Set(void);
    virtual void Store(void) {};

  public:
    cMenuSetupXinelib(cXinelibDevice *Dev);
    virtual eOSState ProcessKey(eKeys Key);
};

#endif //__XINELIB_SETUP_MENU_H
