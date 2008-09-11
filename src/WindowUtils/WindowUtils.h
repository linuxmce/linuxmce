/*
     Copyright (C) 2008 Locale|Concept

     www.localeconcept.com

     Phone: +1 (617) 245-1354

	Author: Thom Cherryhomes <thom.cherryhomes@localeconcept.com>

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

	This code was based in part on:

	wmctrl
	A command line tool to interact with an EWMH/NetWM compatible X Window Manager.

	Author, current maintainer: Tomas Styblo <tripie@cpan.org>

	Copyright (C) 2003

*/

/** @file WindowUtils.h
 Header file for the Window Utils namespace
 */
#ifndef WINDOWUTILS
#define WINDOWUTILS

#include <string>
#include <vector>
#include <map>

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/cursorfont.h>
#include <X11/Xmu/WinUtil.h>

#define _NET_WM_STATE_REMOVE        0    /* remove/unset property */
#define _NET_WM_STATE_ADD           1    /* add/set property */
#define _NET_WM_STATE_TOGGLE        2    /* toggle property  */
#define MAX_PROPERTY_VALUE_LEN 	    4096

/**
@namespace WindowUtils
For Window Management using EWMH semantics.
*/
namespace WindowUtils
{
	using namespace std;
	using std::map;
	using std::vector;
	using std::string;


	/**
	 * GetProperty - Get an X Property.
	 */
	char *GetProperty(Display *disp, Window win, Atom xa_prop_type, string s_PropertyName, unsigned long& size);

	/**
	 * GetClientList - Get the list of clients attached to display.
	 */
	bool GetClientList(Display *disp,vector<Window>& v_wClientList);

	/**
	 * GetWindowTitle - Get Window Title given Window ID
	 */
	bool GetWindowTitle(Display *disp, Window win, string& s_WindowTitle);

	/**
	 * FindWindowMatching - Return first Window ID matching a title fragment. 
	 */
	bool FindWindowMatching(Display *disp, string windowName, Window& win);

	/**
	 * SendKeyToWindow - Send a Keypress to the specified window.	
	 */
	void SendKeyToWindow(Display *disp, unsigned long win, int iXKeySym, int serial_num);

}


#endif
