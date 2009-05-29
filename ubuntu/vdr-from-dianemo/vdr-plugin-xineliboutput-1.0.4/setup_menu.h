/*
 * setup_menu.h: Setup Menu
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: setup_menu.h,v 1.1 2006/06/03 09:50:54 phintuka Exp $
 *
 */

#ifndef __XINELIB_SETUP_MENU_H
#define __XINELIB_SETUP_MENU_H

#include <vdr/menuitems.h>

class cMenuSetupXinelib : public cMenuSetupPage {

  protected:
    void Set(void);
    virtual void Store(void) {};
 
  public:
    cMenuSetupXinelib(void);
    virtual eOSState ProcessKey(eKeys Key);
};

#endif //__XINELIB_SETUP_MENU_H
